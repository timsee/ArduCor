/*!
 * ArduCor
 * Sample Sketch
 *
 * Supports Adafruit NeoPixels products.
 *
 * Provides a UDP interface to a set of lighting routines.
 * 
 * Version 3.3.0
 * Date: May 27, 2018
 * Github repository: http://www.github.com/timsee/ArduCor
 * License: MIT-License, LICENSE provided in root of git repo
 */
#include <ArduCor.h>

#include <Adafruit_NeoPixel.h>
#include <Bridge.h>

//================================================================================
// Settings
//================================================================================

const byte CONTROL_PIN       = 6;      // pin used by NeoPixels library
const int  LED_COUNT         = 64;

const byte BAR_SIZE          = 4;      // default length of a bar for bar routines
const byte GLIMMER_PERCENT   = 10;     // percent of "glimmering" LEDs in glimmer routines: range: 0 - 100

const byte DELAY_VALUE       = 10;      // amount of sleep time between loops

const int  DEFAULT_SPEED     = 100;    // default delay for LEDs update, suggested range: 0 (paused) - 200 (fast).
const int  MAX_SPEED_VALUE   = 200;    // max speed value allowed to be sent

const int  DEFAULT_TIMEOUT   = 120;    // number of minutes without packets until the arduino times out.

const int  DEFAULT_HW_INDEX  = 1;      // index for this particular microcontroller
const int  DEVICE_COUNT      = 1;      // number of LED devices connected, 1 for every sample except the multi sample

const bool USE_CRC           = true;   // true uses CRC, false ignores it.

//=======================
// Hardware Name
//=======================

// rename this whatever you want, but keep it under 16 characters
char name_buffer[] = "MyLights";

//=======================
// Hardware Type
//=======================

// enum for types of hardware that can be controlled.
enum ELightType {
  eSingleLED,
  eCube,
  e2DArray,
  eLightStrip,
  eRing
};

/*! 
 * Modify these to reflect the type of light hardware that is in use
 * by your sample. This does not affect the sample's functionality. Instead,
 * it is sent out during discovery packets to applications such as Corluma
 * and affects how the lights are displayed in those applications. 
 */
ELightType light_type = e2DArray;

//=======================
// Product Type
//=======================

// enum for types of lighting products (NeoPixel, Rainbowduino, etc.)
enum EProductType {
  eRainbowduino,
  eNeoPixels,
  eLED
};

EProductType product_type = eNeoPixels;


//=======================
// API level
//=======================
// The API level defines the messaging protocol. Major levels are incremented 
// when the API breaks and significant features are added that cannot be 
// supported in the old API. Minor levels are incremented when the API has 
// new functions added that do not significantly break the existing
// messaging protocol.
const uint8_t API_LEVEL_MAJOR = 3;
const uint8_t API_LEVEL_MINOR = 3;


//=======================
// Stored Values and States
//=======================

ERoutine current_routine = eSingleGlimmer;
EPalette current_palette = eCustom;

// set this to turn off echoing all together
bool skip_echo = false;
// the sample sets this when it receives a valid packet
bool should_echo = false;

// used in sketches with multiple hardware connected to one arduino.
uint8_t received_hardware_index;
uint8_t hardware_index = DEFAULT_HW_INDEX;

// value determines how quickly the LEDs udpate. Lower values lead to faster updates
int update_speed = DEFAULT_SPEED;
bool should_update_no_speed = false;


// timeout variables
unsigned long idle_timeout = (unsigned long)DEFAULT_TIMEOUT * 60 * 1000; // convert to milliseconds
unsigned long last_message_time = 0;

// counts each loop and uses it to determine
// when to update the LEDs.
unsigned long loop_counter = 0;

//=======================
// String Parsing
//=======================

// flag ued by parsing system. if TRUE, continue parsing, if FALSE,
// packet is either illegal or empty and parsing can be skipped.
bool packetReceived = false;

// ints used for determining how much memory to use
const int max_packet_size = 200;

// buffers for receiving messages
char current_packet[max_packet_size];
char echo_message  [max_packet_size];
char temp_packet   [max_packet_size];

// buffers for converting ASCII to an int array
const int max_number_of_ints = 15;
int packet_int_array[max_number_of_ints];

// used to manipulate the buffers for receiving messages and
// converting them to int arrays.
int multi_packet_size = 0;
int current_multi_packet = 0;
int int_array_size = 0;

// buffers for char arrays
char state_update_packet[110];

char discovery_packet[54];

// used for string manipulations
char num_buf[16];
const char value_delimiter[] = ",";
const char message_delimiter[] = "&";
const char crc_delimiter[] = "#";
const char packet_delimiter[] = ";";
const char names_delimiter[] = "@";
const char new_line[] = "\n";

int  single_glimmer_param = GLIMMER_PERCENT;
int  multi_glimmer_param  = GLIMMER_PERCENT;
bool sawtooth_param       = false;
bool fade_param           = false;
int  multi_bars_param     = BAR_SIZE;


bool reset_counter = false;

//=======================
// Yun Setup
//=======================

// used for communication over the Bridge Library
const char packet_read_string[] = "packet_read";

//=======================
// ArduCor Setup
//=======================
// Library used to generate the RGB LED routines.
ArduCor routines = ArduCor(LED_COUNT);

//=======================
// Hardware Setup
//=======================

//NOTE: you may need to change the NEO_GRB or NEO_KHZ2800 for this sample to work with your lights.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_COUNT, CONTROL_PIN, NEO_GRB + NEO_KHZ800);

//=======================
// CRC-32
//=======================
// Based on this guide http://excamera.com/sphinx/article-crc.html
// 8-bit CRC is too little, 32-bit is a bit overkill but this method
// is really elegant and uses very little PROGMEM

const PROGMEM uint32_t crcTable[16] = 
{
  0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
  0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
  0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
  0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

unsigned long crcUpdate(unsigned long crc, byte data)
{
  byte tableIndex;
  tableIndex = crc ^ (data >> (0 * 4));
  crc = pgm_read_dword_near(crcTable + (tableIndex & 0x0f)) ^ (crc >> 4);
  tableIndex = crc ^ (data >> (1 * 4));
  crc = pgm_read_dword_near(crcTable + (tableIndex & 0x0f)) ^ (crc >> 4);
  return crc;
}

unsigned long crcCalculator(const char* packet)
{
  unsigned long crc = ~0L;
  uint16_t i = 0;
  while (packet[i] != 0) {
    crc = crcUpdate(crc, packet[i]);
    ++i;
  }
  crc = ~crc;
  return crc;
}

//================================================================================
// Setup and Loop
//================================================================================

void setup()
{
  pixels.begin();

  Bridge.begin();
  Bridge.put(F("major_api"), itoa(API_LEVEL_MAJOR, num_buf, 10));
  Bridge.put(F("minor_api"), itoa(API_LEVEL_MINOR, num_buf, 10));
  Bridge.put(F("using_crc"), itoa(USE_CRC, num_buf, 10));
  Bridge.put(F("hardware_count"), itoa(DEVICE_COUNT, num_buf, 10));
  Bridge.put(F("hardware_name"), name_buffer);
  Bridge.put(F("hardware_type"), itoa(light_type, num_buf, 10));
  Bridge.put(F("product_type"), itoa(product_type, num_buf, 10));
  Bridge.put(F("max_packet_size"), itoa(max_packet_size, num_buf, 10));
  buildStateUpdatePacket();
  Bridge.put(F("state_update"), state_update_packet);
  buildCustomArrayUpdatePacket();
  Bridge.put(F("custom_array_update"),state_update_packet); 
  // choose the default color for the single
  // color routines. This can be changed at any time.
  // and its set it to green in sample routines.
  // If its not set, it defaults to a faint orange.
  routines.setMainColor(0, 255, 0);
  buildDiscoveryPacket();
}

void loop()
{
  packetReceived = false;
  Bridge.get("udp", current_packet, sizeof(current_packet));
  if (strcmp(current_packet, packet_read_string) != 0) {
    Bridge.put(F("udp"), packet_read_string);
    packetReceived = true;
  }
  if (packetReceived) {
    memset(echo_message, 0, sizeof(echo_message));
    bool messageIsValid = checkIfPacketIsValid(current_packet);
    skip_echo = false;
    should_echo = false;
    should_update_no_speed = false; 
    if (messageIsValid) { 
      // go through each message packet
      char* messagePtr = strtok(current_packet, "&");
      while (messagePtr != 0) {
        if (!skip_echo) {
          strcpy(temp_packet, messagePtr); // Copy for parsing a destructive way
          strcpy(echo_message, messagePtr); // save for echoing 
        }
        // Find the next substring delimited by a "&"
        messagePtr = strtok(0, "&");
        
        // convert from a array of chars into an int array
        delimitedStringToIntArray(temp_packet);
        // parse a paceket only if its header is is in the correct range
        if ((int_array_size > 0)
            && (packet_int_array[0] < ePacketHeader_MAX)) {
          // message is valid and the first int can be interpeted as a header
          //  attempt to parse the whole packet
          if (parsePacket(packet_int_array[0])) {
            // if packet parsing is sucessful, echo the packet
            last_message_time = millis();
            if (!skip_echo) {
              strcat(echo_message, message_delimiter);
              should_echo = true;
            }
          }
        }
      }
      if (!skip_echo && should_echo) {
        echoPacket();
      }
    }
  }

  if (update_speed == 0) { 
    if (should_update_no_speed) { 
      changeRoutine(current_routine); 
      routines.applyBrightness();  
      updateLEDs();
    } 
  } else if (!(loop_counter % ((MAX_SPEED_VALUE + 5) - update_speed))) { 
    changeRoutine(current_routine);
    routines.applyBrightness();
    updateLEDs();
  }

  // Timeout the LEDs.
  if ((idle_timeout != 0)
      && (last_message_time + idle_timeout < millis())) {
    routines.turnOff();
  }

  loop_counter++;
  delay(DELAY_VALUE);
}


void updateLEDs()
{
  for (int x = 0; x < LED_COUNT; x++) {
    pixels.setPixelColor(x, pixels.Color(routines.red(x),
                                         routines.green(x),
                                         routines.blue(x)));
  }
  pixels.show();
}


//================================================================================
// Mode Management
//================================================================================

/*!
 * @brief changeRoutine Function that runs every loop iteration
 *        and determines how to light up the LEDs.
 *
 * @param currentMode the current mode of the program
 */
void changeRoutine(ERoutine currentMode)
{
  switch (currentMode)
  {
    case eSingleSolid:
      routines.singleSolid(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue);
      break;

    case eSingleBlink:
      routines.singleBlink(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue);
      break;

    case eSingleWave:
      routines.singleWave(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue);
      break;

    case eSingleGlimmer:
      routines.singleGlimmer(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue, single_glimmer_param);
      break;

    case eSingleFade:
      routines.singleFade(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue, fade_param);
      break;

    case eSingleSawtoothFade:
      routines.singleSawtoothFade(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue, sawtooth_param);
      break;

    case eMultiGlimmer:
      routines.multiGlimmer(current_palette, multi_glimmer_param);
      break;

    case eMultiFade:
      routines.multiFade(current_palette);
      break;

    case eMultiRandomSolid:
      routines.multiRandomSolid(current_palette);
      break;

    case eMultiRandomIndividual:
      routines.multiRandomIndividual(current_palette);
      break;

    case eMultiBars:
      routines.multiBars(current_palette, multi_bars_param);
      break;

    default:
      break;
  }
}

//================================================================================
//  Packet Parsing
//================================================================================

/*!
 * @brief parsePacket This parser looks at the header of a control packet and from that determines
 *        which parameters to use and what settings to change.
 *        
 * @param header the int representation of the packet's first value.
 */
bool parsePacket(int header)
{
  // In each case, theres a final check that the packet was properly
  // formatted by making sure its getting the right number of values.
  boolean success = false;
  switch (header)
  {
    case eOnOffChange:
      if (int_array_size == 3) {
        success = true;
        received_hardware_index = packet_int_array[1];
        if ((received_hardware_index == hardware_index) || (received_hardware_index == 0)) {
          if (packet_int_array[2] == 0) {
            routines.turnOff();
          } else if (packet_int_array[2] == 1) {
            loop_counter = 0;
            routines.turnOn();
          }
        }
      }
      break;
    case eModeChange:
      success = routineParser(success);
      break;
    case eCustomArrayColorChange:
      if (int_array_size == 6) {
        int color_index = packet_int_array[2];
        if (color_index >= 0 && color_index < eRoutine_MAX) {
          success = true;

          // only tell the routines to reset themselves if a custom routine is used.
          if ((current_routine > eSingleSawtoothFade)
              && (current_palette == eCustom)) {
            // Reset LEDS
            loop_counter = 0;
          }
          received_hardware_index = packet_int_array[1];
          if ((received_hardware_index == hardware_index) || (received_hardware_index == 0)) {
            routines.setColor(color_index,
                              packet_int_array[3],
                              packet_int_array[4],
                              packet_int_array[5]);
          }
        }
      }
      break;
    case eBrightnessChange:
      {
        if (int_array_size == 3) {
          success = true;
          int param = constrain(packet_int_array[2], 0, 100);
          received_hardware_index = packet_int_array[1];
          // update brightness level
          if ((received_hardware_index == hardware_index) || (received_hardware_index == 0)) {
            if (param != routines.brightness()) { 
              should_update_no_speed = true; 
              routines.brightness(param); 
            } 
          }
        }
        break;
      }
    case eIdleTimeoutChange:
      if (int_array_size == 3) {
        success = true;
        received_hardware_index = packet_int_array[1];
        if ((received_hardware_index == hardware_index) || (received_hardware_index == 0)) {
          unsigned long new_timeout = (unsigned long)packet_int_array[2];
          idle_timeout = new_timeout * 60 * 1000;
        }
      }
      break;
    case eCustomColorCountChange:
      if (int_array_size == 3) {
        if (packet_int_array[2] > 1) {
          success = true;
          received_hardware_index = packet_int_array[1];
          if ((received_hardware_index == hardware_index) || (received_hardware_index == 0)) {
            routines.setCustomColorCount(packet_int_array[2]);
          }
        }
      }
      break;
    case eStateUpdateRequest:
      if (int_array_size == 1) {
        skip_echo = true;
        // Send back update
        buildStateUpdatePacket();
        Bridge.put(F("state_update"), state_update_packet);
      }
      break;
    case eCustomArrayUpdateRequest:
      if (int_array_size == 1) {
        skip_echo = true;
        // Send back update
        buildCustomArrayUpdatePacket();
        Bridge.put(F("custom_array_update"), state_update_packet);
      }
      break;
    default:
      break;
  }
  return success;
}


/*!
 * @brief routineParser Parses a routine packet, checking that the received packets are the proper
 *        size and that their values fall into the proper ranges. If they do, this function sets
 *        the values in memory.
 *        
 * @param currentSuccess the current success status of the packet parsing. This will only ever
 *        set the success packet to true
 */
bool routineParser(bool currentSuccess)
{
  bool success = currentSuccess;
  // check theres at least enough information to get a routine and hardware index
  // and check if routine is in a valid range
  if (int_array_size > 2) {
      if ((packet_int_array[2] < (int)eRoutine_MAX)
           && (packet_int_array[1] <= (DEFAULT_HW_INDEX + DEVICE_COUNT - 1))) {
      // required values
      received_hardware_index = packet_int_array[1];
      ERoutine routine        = (ERoutine)packet_int_array[2];
      bool isValid            = false;
      reset_counter           = false;
  
      // routine specific values
      EPalette palette        = ePalette_MAX;
      int speedValue          = 0;

      // check that packets are the correct size and fill in parameters
      // check if loop counter should reset
      switch (routine) {
        case eSingleSolid:
        {
          if (int_array_size == 6) {
            isValid = parseColor(packet_int_array[3],
                                 packet_int_array[4],
                                 packet_int_array[5]);
          }
          break;
        }
        case eSingleBlink:
        case eSingleWave:     
        {
          if (int_array_size == 7) {
            speedValue = packet_int_array[6];
            if (speedValue >= 0 && speedValue <= MAX_SPEED_VALUE) {
              isValid = parseColor(packet_int_array[3],
                                   packet_int_array[4],
                                   packet_int_array[5]);           
            }
            // check if reset counter
            if (routine != current_routine) {
              reset_counter = true;            
            }
          }
          break;
        }
        case eSingleFade:
        case eSingleSawtoothFade:        
        case eSingleGlimmer:
        {
          if (int_array_size == 8) {
            speedValue = packet_int_array[6];

            // handle the optional parameters
            if (routine == eSingleFade) {
              if (speedValue >= 0 
                  && speedValue <= MAX_SPEED_VALUE
                  && packet_int_array[7] <= 1) {
                isValid = parseColor(packet_int_array[3],
                                     packet_int_array[4],
                                     packet_int_array[5]);
                if (packet_int_array[7] != fade_param) {
                  fade_param = packet_int_array[7];
                  reset_counter = true;            
                }
              }
            } else if (routine == eSingleSawtoothFade) {
              if (speedValue >= 0 
                  && speedValue <= MAX_SPEED_VALUE
                  && packet_int_array[7] <= 1) {
                isValid = parseColor(packet_int_array[3],
                                     packet_int_array[4],
                                     packet_int_array[5]);
                if (packet_int_array[7] != sawtooth_param) {
                  sawtooth_param = packet_int_array[7];
                  reset_counter = true;            
                }
              }
            } else if (routine == eSingleGlimmer) {
               if (speedValue >= 0 
                  && speedValue <= MAX_SPEED_VALUE
                  && packet_int_array[7] <= 100) {
                isValid = parseColor(packet_int_array[3],
                                     packet_int_array[4],
                                     packet_int_array[5]);
                if (packet_int_array[7] != single_glimmer_param) {
                  single_glimmer_param = packet_int_array[7];
                  reset_counter = true;            
                }
              }
            }

            // check if reset counter
            if (routine != current_routine) {
              reset_counter = true;            
            }
          }
          break;
        } 
        case eMultiGlimmer:
        case eMultiBars:
        {
          if (int_array_size == 6) {
            palette = (EPalette)packet_int_array[3];
            speedValue = packet_int_array[4];
            isValid = true;
  
            if (routine == eMultiGlimmer) {
              if (speedValue >= 0 
                  && speedValue <= MAX_SPEED_VALUE
                  && packet_int_array[5] <= 100) {
                 isValid = true;
                 if (packet_int_array[5] != multi_glimmer_param) {
                    multi_glimmer_param = packet_int_array[5];
                    reset_counter = true;            
                 }
               }
             } else if (routine == eMultiBars) {
               if (speedValue >= 0 
                  && speedValue <= MAX_SPEED_VALUE
                  && packet_int_array[5] <= 10) {
                 isValid = true;
                 if (packet_int_array[5] != multi_bars_param) {
                   multi_bars_param = packet_int_array[5];
                   reset_counter = true;            
                }
              }
            }
            if (speedValue >= 0 && speedValue <= MAX_SPEED_VALUE) {
              isValid = true;
              // check if reset counter
              if (palette != current_palette || routine != current_routine) {
                reset_counter = true;            
              }
            }
          }
          break;
        }
        case eMultiFade:
        case eMultiRandomSolid:
        case eMultiRandomIndividual:
        {
          if (int_array_size == 5) {
            palette = (EPalette)packet_int_array[3];
            speedValue = packet_int_array[4];

            if (speedValue >= 0 && speedValue <= MAX_SPEED_VALUE) {
              isValid = true;
              // check if reset counter
              if (palette != current_palette || routine != current_routine) {
                reset_counter = true;            
              }
            }
          }
          break;
        }
        default:
          break;
      }
      // if the packet was valid, do some final checks
      if (isValid) {
        // update stored values
        if ((received_hardware_index == hardware_index) || (received_hardware_index == 0)) {
          if (routine == eSingleSolid) {
            // check reset counter
            if (routine != current_routine) {
              reset_counter = true;            
            }
            current_routine = routine;
          } else if ((speedValue >= 0 && speedValue <= MAX_SPEED_VALUE)) {
            update_speed = speedValue;
            current_routine = routine;
            if (routine > eSingleSawtoothFade) {
              current_palette = palette;
            }
          }
        }
        
        if (reset_counter) {
          // Reset to 0 to draw to screen right away
          loop_counter = 0;
          if ((received_hardware_index == hardware_index) || (received_hardware_index == 0)) {
            should_update_no_speed = true;            
          }
        }     
        success = true;
      }
    } 
  }
  return success;
}

bool parseColor(int red, int green, int blue) {
  bool success = false;
  // first check if the values are in a valid range
  if (red >= 0 && red <= 255
      || green >= 0 && green <= 255
      || blue >= 0 && blue <= 255) {
      if ((received_hardware_index == hardware_index) || (received_hardware_index == 0)) {
        if (red != routines.mainColor().red
            || green != routines.mainColor().green
            || blue  != routines.mainColor().blue) {
          reset_counter = true;
          routines.setMainColor(red, green, blue);
        }
      }
    success = true;
  }
  return success;
}


//================================================================================
// State Update
//================================================================================

void buildStateUpdatePacket() 
{
  memset(state_update_packet, 0, sizeof(state_update_packet));

  strcat(state_update_packet, itoa((uint8_t)eStateUpdateRequest, num_buf, 10));
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa((uint8_t)hardware_index, num_buf, 10));
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa((uint8_t)routines.isOn(), num_buf, 10));
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa(1, num_buf, 10)); // isReachable
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa(routines.mainColor().red, num_buf, 10));
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa(routines.mainColor().green, num_buf, 10));
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa(routines.mainColor().blue, num_buf, 10));
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa((uint8_t)current_routine, num_buf, 10));
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa((uint8_t)current_palette, num_buf, 10));
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa(routines.brightness(), num_buf, 10));
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa(update_speed, num_buf, 10));
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa(idle_timeout / 60000, num_buf, 10));
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa(calculateMinutesUntilTimeout(last_message_time, idle_timeout), num_buf, 10));
  strcat(state_update_packet, message_delimiter);


  // add the crc
  if (USE_CRC) {
    unsigned long crc = crcCalculator(state_update_packet);
    strcat(state_update_packet, crc_delimiter);
    strcat(state_update_packet, ultoa(crc, num_buf, 10));
    strcat(state_update_packet, message_delimiter);
   }  

}


void buildCustomArrayUpdatePacket() 
{
  memset(state_update_packet, 0, sizeof(state_update_packet));

  strcat(state_update_packet, itoa((uint8_t)eCustomArrayUpdateRequest, num_buf, 10));
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa((uint8_t)hardware_index, num_buf, 10));
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa((uint8_t)routines.customColorCount(), num_buf, 10));
  for (int i = 0; i < routines.customColorCount(); ++i) {
    strcat(state_update_packet, value_delimiter);
    strcat(state_update_packet, itoa(routines.color(i).red, num_buf, 10));
    strcat(state_update_packet, value_delimiter);
    strcat(state_update_packet, itoa(routines.color(i).green, num_buf, 10));
    strcat(state_update_packet, value_delimiter);
    strcat(state_update_packet, itoa(routines.color(i).blue, num_buf, 10));
  }
  strcat(state_update_packet, message_delimiter);

  // add the crc
  if (USE_CRC) {
    unsigned long crc = crcCalculator(state_update_packet);
    strcat(state_update_packet, crc_delimiter);
    strcat(state_update_packet, ultoa(crc, num_buf, 10));
    strcat(state_update_packet, message_delimiter);
  }  

}


void buildDiscoveryPacket()
{
  strcat(discovery_packet, "DISCOVERY_PACKET");
  strcat(discovery_packet, value_delimiter);
  strcat(discovery_packet, itoa((uint8_t)API_LEVEL_MAJOR, num_buf, 10));
  strcat(discovery_packet, value_delimiter);
  strcat(discovery_packet, itoa((uint8_t)API_LEVEL_MINOR, num_buf, 10));
  strcat(discovery_packet, value_delimiter);
  strcat(discovery_packet, itoa((uint8_t)USE_CRC, num_buf, 10));
  strcat(discovery_packet, value_delimiter);
  strcat(discovery_packet, itoa((uint8_t)0, num_buf, 10)); // Hardware Capabilities flag (0 for arduino, 1 for raspberry pi)
  strcat(discovery_packet, value_delimiter);
  strcat(discovery_packet, itoa((uint8_t)max_packet_size, num_buf, 10));
  strcat(discovery_packet, value_delimiter);
  strcat(discovery_packet, itoa((uint8_t)DEVICE_COUNT, num_buf, 10));
  strcat(discovery_packet, names_delimiter);
  strcat(discovery_packet, name_buffer);
  strcat(discovery_packet, value_delimiter);
  strcat(discovery_packet, itoa((uint8_t)light_type, num_buf, 10));
  strcat(discovery_packet, value_delimiter);
  strcat(discovery_packet, itoa((uint8_t)product_type, num_buf, 10));
  strcat(discovery_packet, message_delimiter);

}

void echoPacket()
{  
  // add the crc
  if (USE_CRC) {
    unsigned long crc = crcCalculator(echo_message);
    strcat(echo_message, crc_delimiter);
    strcat(echo_message, ultoa(crc, num_buf, 10));
    strcat(echo_message, message_delimiter);
   }  
  
}

unsigned long calculateMinutesUntilTimeout(unsigned long last_message, unsigned long timeout_max) {
  if (timeout_max == 0) {
    // will never timeout as this is disabled, jsut return 1.
    return 1;
  } else if (last_message + timeout_max < millis()) {
    // already timed out.
    return 0;
  } else {
    // we truncate the value so a +1 is added so that when theres less than a minute its not treated as already timed out.
    return ((timeout_max + last_message - millis()) / 60000) + 1;
  }
}


//================================================================================
// String Parsing
//================================================================================

/*!
 * @brief checkIfPacketIsValid takes a char array as input and determines if  
 *        it contains the proper characters. If CRC is enabled, it also runs the
 *        CRC check. This function does not modify the char array in any way. 
 *
 * @param message the input char array.
 *
 * @return true if the string was parseable and passes CRC, false otherwise.
 */
bool checkIfPacketIsValid(char* message)
{
  bool isValid = true;

  //-----
  // Validity check
  //-----
  // check if it contains only valid characters
  int hashCount = 0;
  int i = 0;
  while(message[i] != 0) {
    if (!(isdigit(message[i])
          || message[i] == ','
          || message[i] == '&'
          || message[i] == '#'
          || message[i] == '-')) {
      isValid = false;
    }
  
    // # is only used in CRCs, and a valid message
    // can only have one. 
    if (message[i] == '#') {
      if (USE_CRC) {
        // count number of #
        hashCount++;
      } else {
        // This should not be valid as its used for CRC only
        isValid = false;
      }
    }  
    ++i;
  }

  //-----
  // Early failures check
  //-----
  // exit early if packet is not valid. 
  if ((USE_CRC && (hashCount != 1)) // CRC packets must have one #
      || (isValid == false)) {
    return false;
  }
  
  //-----
  // CRC check
  //-----
  if (USE_CRC) {
    // get a char array of the payload
    char* payload = strtok(message, "#");
    // get a char array of the CRC
    char* crcASCII = strtok(0, "&");
    // compute the CRC of the full packet
    unsigned long computedCRC = crcCalculator(message);
    // grab the unsigned long value of the crc
    unsigned long givenCRC = strtoul(crcASCII, NULL, 0);
    if (computedCRC == givenCRC) {
      // using CRC, computed CRC matches given
      return true;
    } else {
      // using CRC, computed CRC matches given
      return false;
    }
  } else {
    // valid but not using CRC, return true
    return true;
  }
}

/*!
 *  @brief delimitedStringToIntArray takes a char array and fills the packet_int_array
 *         variable with the integer representation of these packets. By this point in message parsing,
 *         it is already confirmed that the messages contain only digits and "," characters, as the char
 *         array has passed its CRC check and has been converted into a single message and has had its "&"
 *         stripped away. This is a destructive operation and the char array cannot be used after it. 
 *
 * @param message the input string.
 *
 */
void delimitedStringToIntArray(char* message)
{
  int_array_size = 0;
  // Get the frist substring delimited by a ","
  char* valuePtr = strtok(message, ",");
  while (valuePtr != 0) {
    // convert chars to int and story in int array.
    packet_int_array[int_array_size] = atoi(valuePtr);
    int_array_size++;
    // Find the next substring delimited by a ","
    valuePtr = strtok(0, ",");
  } 
}
