/*!
 * RGB-LED-Routines
 * Sample Sketch
 *
 * Supports a single RGB LED but can be easily hacked to support more.
 *
 * Provides a Serial interface to a set of lighting routines.
 * 
 * Version 2.8.0
 * Date: February 3, 2018
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */
#include <RoutinesRGB.h>


//================================================================================
// Settings
//================================================================================

const byte R_PIN             = 6;
const byte G_PIN             = 5;
const byte B_PIN             = 4;
const int  LED_COUNT         = 1;
const byte IS_COMMON_ANODE   = 1;      // 0 if common cathode, 1 if common anode

const byte BAR_SIZE          = 4;      // default length of a bar for bar routines
const byte GLIMMER_PERCENT   = 10;     // percent of "glimmering" LEDs in glimmer routines: range: 0 - 100

const byte DELAY_VALUE       = 3;      // amount of sleep time between loops

const int  DEFAULT_SPEED     = 300;    // default delay for LEDs update, suggested range: 10 (fast) - 1000 (slow).
const int  DEFAULT_TIMEOUT   = 120;    // number of minutes without packets until the arduino times out.

const int  DEFAULT_HW_INDEX  = 1;      // index for this particular microcontroller
const int  MAX_HW_INDEX      = 1;      // number of LED devices connected, 1 for every sample except the multi sample


const bool USE_CRC           = true;   // true uses CRC, false ignores it.
const bool USE_NEWLINE       = false;  // true adds newline to serial packets, false skips it.

//=======================
// API level
//=======================
// The API level defines the messaging protocol. Major levels are incremented 
// when the API breaks and significant features are added that cannot be 
// supported in the old API. Minor levels are incremented when the API has 
// new functions added that do not significantly break the existing
// messaging protocol.
const uint8_t API_LEVEL_MAJOR = 2;
const uint8_t API_LEVEL_MINOR = 0;

//=======================
// Stored Values and States
//=======================

ELightingRoutine current_routine = eSingleGlimmer;
EColorGroup current_group = eCustom;

bool isOn = true;
// set this to turn off echoing all together
bool skip_echo = false;
// the sample sets this when it receives a valid packet
bool should_echo = false;

// used in sketches with multiple hardware connected to one arduino.
uint8_t received_hardware_index;
uint8_t hardware_index = DEFAULT_HW_INDEX;

// value determines how quickly the LEDs udpate. Lower values lead to faster updates
int update_speed = (int)((1000.0 / DELAY_VALUE) / (DEFAULT_SPEED / 100.0));
// value received from packets
int raw_speed = update_speed;

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
const int max_number_of_ints = 10;
const int max_message_size = 20;
const int max_number_of_messages = 8;
const int max_packet_size = max_message_size * max_number_of_messages;

// buffers for receiving messages
char current_packet[max_message_size * max_number_of_messages];
char echo_message[max_message_size * max_number_of_messages];

// buffers for converting ASCII to an int array
char char_array[max_message_size];
int packet_int_array[max_number_of_ints];
char temp_packet[max_packet_size];
char echo_packet[max_packet_size];

// used to manipulate the buffers for receiving messages and
// converting them to int arrays.
int multi_packet_size = 0;
int current_multi_packet = 0;
int int_array_size = 0;

// buffers for char arrays
char state_update_packet[100];
char discovery_packet[35];

// used for string manipulations
char num_buf[16];
const char value_delimiter[] = ",";
const char message_delimiter[] = "&";
const char crc_delimiter[] = "#";
const char packet_delimiter[] = ";";
const char new_line[] = "\n";


//=======================
// RoutinesRGB Setup
//=======================
// Library used to generate the RGB LED routines.
RoutinesRGB routines = RoutinesRGB(LED_COUNT);


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
  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);

  // choose the default color for the single
  // color routines. This can be changed at any time.
  // and its set it to green in sample routines.
  // If its not set, it defaults to a faint orange.
  routines.setMainColor(0, 255, 0);
  // put your setup code here, to run once:
  Serial.begin(9600);
  buildDiscoveryPacket();
}

void loop()
{
  packetReceived = false;
  memset(current_packet, 0, sizeof(current_packet));
  if (Serial.available()) {
    Serial.readBytesUntil(';',current_packet, sizeof(current_packet));
 }
 if (current_packet[0] == 'D') {
    char* pch = strstr (current_packet,"DISCOVERY_PACKET");
    if (strcmp(pch, "DISCOVERY_PACKET") == 0) {
      Serial.write(discovery_packet);
    }
  } else if (current_packet[0] != 0) {
    packetReceived = true;
  }
  
  if (packetReceived) {
    memset(echo_message, 0, sizeof(echo_message));
    bool messageIsValid = checkIfPacketIsValid(current_packet);
    skip_echo = false;
    should_echo = false;
    if (messageIsValid) { 
      // go through each message packet
      char* messagePtr = strtok(current_packet, "&");
      while (messagePtr != 0) {
        if (!skip_echo) {
          strcpy(temp_packet, messagePtr); // Copy for parsing a destructive way
          strcpy(echo_packet, messagePtr); // save for echoing 
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
              strcat(echo_message, echo_packet);
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

  if (!(loop_counter % update_speed)) {
    if (isOn) {
      changeLightingRoutine(current_routine);
      routines.applyBrightness(); // Optional. Dims the LEDs based on the routines.brightness() setting
    } else {
      routines.turnOff();
    }
    updateLEDs();
  }

  // Timeout the LEDs.
  if ((idle_timeout != 0)
      && (last_message_time + idle_timeout < millis())) {
    isOn = false;
  }

  loop_counter++;
  delay(DELAY_VALUE);
}


void updateLEDs()
{
  if (IS_COMMON_ANODE) {
    analogWrite(R_PIN, 255 - routines.red(0));
    analogWrite(G_PIN, 255 - routines.green(0));
    analogWrite(B_PIN, 255 - routines.blue(0));
  }
  else {
    analogWrite(R_PIN, routines.red(0));
    analogWrite(G_PIN, routines.green(0));
    analogWrite(B_PIN, routines.blue(0));
  }
}


//================================================================================
// Mode Management
//================================================================================

/*!
 * @brief changeLightingRoutine Function that runs every loop iteration
 *        and determines how to light up the LEDs.
 *
 * @param currentMode the current mode of the program
 */
void changeLightingRoutine(ELightingRoutine currentMode)
{
  switch (currentMode)
  {
    case eOff:
      routines.turnOff();
      break;

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
      routines.singleGlimmer(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue, GLIMMER_PERCENT);
      break;

    case eSingleLinearFade:
      routines.singleFade(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue, false);
      break;

    case eSingleSineFade:
      routines.singleFade(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue, true);
      break;

    case eSingleSawtoothFadeIn:
      routines.singleSawtoothFade(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue, true);
      break;

    case eSingleSawtoothFadeOut:
      routines.singleSawtoothFade(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue, false);
      break;

    case eMultiGlimmer:
      routines.multiGlimmer(current_group, GLIMMER_PERCENT);
      break;

    case eMultiFade:
      routines.multiFade(current_group);
      break;

    case eMultiRandomSolid:
      routines.multiRandomSolid(current_group);
      break;

    case eMultiRandomIndividual:
      routines.multiRandomIndividual(current_group);
      break;

    case eMultiBarsSolid:
      routines.multiBarsSolid(current_group, BAR_SIZE);
      break;

    case eMultiBarsMoving:
      routines.multiBarsMoving(current_group, BAR_SIZE);
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
    case eModeChange:
      if (int_array_size == 3 && packet_int_array[2] < eLightingRoutine_MAX) {
        if (packet_int_array[2] != current_routine) {
          // Reset to 0 to draw to screen right away
          loop_counter = 0;
        }
        success = true;
        received_hardware_index = packet_int_array[1];
        if ((received_hardware_index == hardware_index) || (received_hardware_index == 0)) {
          if ((ELightingRoutine)packet_int_array[2] == eOff) {
            isOn = false;
          } else {
            isOn = true;
            // change mode to new mode
            current_routine = (ELightingRoutine)packet_int_array[2];
          }
        }
      }
      // pick up cases where the modes can take extra optional arguments
      if (int_array_size == 4) {
        if (packet_int_array[2] >= (int)eMultiGlimmer) {
          if (packet_int_array[2] != current_routine) {
            // Reset to 0 to draw to screen right away
            loop_counter = 0;
          }
          success = true;
          received_hardware_index = packet_int_array[1];
          if ((received_hardware_index == hardware_index) || (received_hardware_index == 0)) {
            current_routine = (ELightingRoutine)packet_int_array[2];
            current_group = (EColorGroup)packet_int_array[3];
            isOn = true;
          }
        }
      }
      break;
    case eMainColorChange:
      if (int_array_size == 5) {
        if (packet_int_array[2] != routines.mainColor().red
            || packet_int_array[3] != routines.mainColor().green
            || packet_int_array[4] != routines.mainColor().blue) {
          // Reset to 0 to draw to screen right away
          loop_counter = 0;
        }
        success = true;
        received_hardware_index = packet_int_array[1];
        if ((received_hardware_index == hardware_index) || (received_hardware_index == 0)) {
          routines.setMainColor(packet_int_array[2],
                                packet_int_array[3],
                                packet_int_array[4]);
          isOn = true;
        }
      }
      break;
    case eCustomArrayColorChange:
      if (int_array_size == 6) {
        int color_index = packet_int_array[2];
        if (color_index >= 0 && color_index < eLightingRoutine_MAX) {
          success = true;

          // only tell the routines to reset themselves if a custom routine is used.
          if ((current_routine > eSingleSawtoothFadeOut)
              && (current_group == eCustom)) {
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
            routines.brightness(param);
            isOn = true;
          }
        }
        break;
      }
    case eSpeedChange:
      if (int_array_size == 3) {
        success = true;
        received_hardware_index = packet_int_array[1];
        if ((received_hardware_index == hardware_index) || (received_hardware_index == 0)) {
          raw_speed = packet_int_array[2];
          update_speed = (int)((1000.0 / DELAY_VALUE) / (raw_speed / 100.0));
        }
      }
      break;
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
        Serial.write(state_update_packet);
      }
      break;
    case eCustomArrayUpdateRequest:
      if (int_array_size == 1) {
        skip_echo = true;
        // Send back update
        buildCustomArrayUpdatePacket();
        Serial.write(state_update_packet);
      }
      break;
    case eResetSettingsToDefaults:
      if (int_array_size == 3) {
        // reset requires a message with exactly 2 parameters:
        // 42 and 71. This drops the probability of buffer
        // issues causing causing false positives.
        if ((packet_int_array[1] == 42)
            && (packet_int_array[2] == 71)) {
          success = true;
          routines.resetToDefaults();
        }
      }
      break;
    default:
      break;
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
  strcat(state_update_packet, itoa((uint8_t)isOn, num_buf, 10));
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
  strcat(state_update_packet, itoa((uint8_t)current_group, num_buf, 10));
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa(routines.brightness(), num_buf, 10));
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa(raw_speed, num_buf, 10));
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
  strcat(state_update_packet, packet_delimiter);
  // add the newline
  if (USE_NEWLINE) {
    strcat(state_update_packet, new_line);
  }

}


void buildCustomArrayUpdatePacket() 
{
  memset(state_update_packet, 0, sizeof(state_update_packet));

  strcat(state_update_packet, itoa((uint8_t)eCustomArrayUpdateRequest, num_buf, 10));
  strcat(state_update_packet, value_delimiter);
  strcat(state_update_packet, itoa((uint8_t)hardware_index, num_buf, 10));
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

  strcat(state_update_packet, packet_delimiter);
  // add the newline
  if (USE_NEWLINE) {
      strcat(state_update_packet, new_line);
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
  strcat(discovery_packet, itoa((uint8_t)MAX_HW_INDEX, num_buf, 10));
  strcat(discovery_packet, value_delimiter);
  strcat(discovery_packet, itoa((uint8_t)max_packet_size, num_buf, 10));
  strcat(discovery_packet, message_delimiter);
  
  strcat(discovery_packet, packet_delimiter);
  // add the newline
  if (USE_NEWLINE) {
    strcat(discovery_packet, new_line);
  }
}

void echoPacket()
{  
  // add the crc
  if (USE_CRC) {
    unsigned long crc = crcCalculator(state_update_packet);
    strcat(echo_message, crc_delimiter);
    strcat(echo_message, ultoa(crc, num_buf, 10));
    strcat(echo_message, message_delimiter);
   }  
  
  strcat(echo_message, packet_delimiter);
  // add the newline
  if (USE_NEWLINE) {
    strcat(echo_message, new_line);
  }
  Serial.write(echo_message);
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
