/*!
 * RGB-LED-Routines
 * Sample Sketch
 *
 * Example sketch with multiple routinesRGB instances used
 *
 * Provides a UDP interface to a set of lighting routines.
 * 
 * Version 2.2.0
 * Date: May 7, 2017
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */
#include <RoutinesRGB.h>
#include <Bridge.h>
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

//================================================================================
// Settings
//================================================================================

const byte CONTROL_PIN       = 5;
const int  LED_COUNT         = 120;

const byte BAR_SIZE          = 4;      // default length of a bar for bar routines
const byte GLIMMER_PERCENT   = 10;     // percent of "glimmering" LEDs in glimmer routines: range: 0 - 100

const byte DELAY_VALUE       = 3;      // amount of sleep time between loops

const int  DEFAULT_SPEED     = 300;    // default delay for LEDs update, suggested range: 10 (fast) - 1000 (slow).
const int  DEFAULT_TIMEOUT   = 120;    // number of minutes without packets until the arduino times out.

const int  DEFAULT_HW_INDEX  = 1;      // index for this particular microcontroller
const int  MAX_HW_INDEX      = 2;      // multi sample gives access to 2 different LED devices



const bool USE_CRC           = true;   // true uses CRC, false ignores it.

//=======================
// Stored Values and States
//=======================

ELightingRoutine current_routine = eSingleGlimmer;
ELightingRoutine current_routine_2 = eSingleGlimmer;
EColorGroup current_group = eCustom;
EColorGroup current_group_2    = eCustom;
EColorGroup current_group_cube = eCustom;

bool isOn = true;
bool isOn_2 = true;
bool skip_echo = false;

// used in sketches with multiple hardware connected to one arduino.
uint8_t received_hardware_index;
uint8_t hardware_index = DEFAULT_HW_INDEX;

// value determines how quickly the LEDs udpate. Lower values lead to faster updates
int update_speed = (int)((1000.0 / DELAY_VALUE) / (DEFAULT_SPEED / 100.0));
// value received from packets
int raw_speed = update_speed;
int update_speed_2 = (int)((1000.0 / DELAY_VALUE) / (DEFAULT_SPEED / 100.0));
int raw_speed_2 = update_speed_2;

// timeout variables
unsigned long idle_timeout = (unsigned long)DEFAULT_TIMEOUT * 60 * 1000; // convert to milliseconds
unsigned long idle_timeout_2 = (unsigned long)DEFAULT_TIMEOUT * 60 * 1000; // convert to milliseconds
unsigned long last_message_time = 0;

// counts each loop and uses it to determine
// when to update the LEDs.
unsigned long loop_counter = 0;
String currentPacket;

//=======================
// String Parsing
//=======================

// flag ued by parsing system. if TRUE, continue parsing, if FALSE,
// packet is either illegal or empty and parsing can be skipped.
bool packetReceived = false;

// ints used for determining how much memory to use
const int max_number_of_ints = 10;
const int max_message_size = 20;
const int max_number_of_messages = 5;
const int max_packet_size = max_message_size * max_number_of_messages;

// buffers for receiving messages
String multi_packet_strings[max_number_of_messages];
char multi_packet_char_array[max_message_size * max_number_of_messages];

// buffers for converting ASCII to an int array
char char_array[max_message_size];
int packet_int_array[max_number_of_ints];
char num_buf[4];

// used to manipulate the buffers for receiving messages and
// converting them to int arrays.
int multi_packet_size = 0;
int current_multi_packet = 0;
int int_array_size = 0;

//=======================
// Yun Setup
//=======================

// used for communication over the Bridge Library
const String packetReadString = "packet_read";
char buffer[max_message_size * max_number_of_messages];


//=======================
// Hardware Setup
//=======================
/*
 * This demo sketch shows routines that use these indices for their light groups:
 *    1 (routines)   :  first half of a 2 meter neopixel strip
 *     2 (routines_2) :  second half of 2 meter neopixel strip
 */
// NeoPixels controller object
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_COUNT, CONTROL_PIN, NEO_GRB + NEO_KHZ800);

uint8_t routines_2_index  = DEFAULT_HW_INDEX + 1;

RoutinesRGB routines = RoutinesRGB(LED_COUNT / 2);
RoutinesRGB routines_2 = RoutinesRGB(LED_COUNT / 2);



//=======================
// CRC-32
//=======================
// Based on this guide http://excamera.com/sphinx/article-crc.html
// 8-bit CRC is too little, 32-bit is a bit overkill but this method
// is really elegant and uses very little PROGMEM

const PROGMEM uint32_t crcTable[16] = {
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

unsigned long crcCalculator(String s)
{
  unsigned long crc = ~0L;
  for (uint16_t i = 0; i < s.length(); ++i) {
    char c = s[i];
    crc = crcUpdate(crc, c);
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
  Bridge.put(F("hardware_count"), itoa(MAX_HW_INDEX, num_buf, 10));
  Bridge.put(F("using_crc"), itoa(USE_CRC, num_buf, 10));
  Bridge.put(F("max_packet_size"), itoa(max_packet_size, num_buf, 10));
  Bridge.put(F("state_update"), buildStateUpdatePacket());
  Bridge.put(F("custom_array_update"), buildCustomArrayUpdatePacket()); 
  // choose the default color for the single
  // color routines. This can be changed at any time.
  // and its set it to green in sample routines.
  // If its not set, it defaults to a faint orange.
  routines.setMainColor(0, 255, 0);
  routines_2.setMainColor(0, 255, 0);
}

void loop()
{
  packetReceived = false;
  Bridge.get("udp", buffer, sizeof(buffer));
  currentPacket = String(buffer);
  if (currentPacket != packetReadString) {
    Bridge.put(F("udp"), packetReadString);
    packetReceived = true;
  }
  if (packetReceived) {
    // remove any extraneous whitepsace or newline characters
    currentPacket.trim();
    bool multiMessageIsValid = parseMultiMessageString(currentPacket);
    skip_echo = false;
    if (multiMessageIsValid) {
      for (current_multi_packet = 0; current_multi_packet < multi_packet_size; ++current_multi_packet) {
        bool isValid = delimitedStringToIntArray(multi_packet_strings[current_multi_packet]);
        // parse a paceket only if its header is is in the correct range
        if (isValid
            && (int_array_size > 0)
            && (packet_int_array[0] < ePacketHeader_MAX)) {
          if (parsePacket(packet_int_array[0])) {
            last_message_time = millis();
          }
        }
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
  }
  if (!(loop_counter % update_speed_2)) {
    if (isOn_2) {
      changeLightingRoutine_2(current_routine_2);
      routines_2.applyBrightness(); // Optional. Dims the LEDs based on the routines_2.brightness() setting
    } else {
      routines_2.turnOff();
    }
  }

  // update happens here for edge case handling
  if (!(loop_counter % update_speed_2) || !(loop_counter % update_speed)) {
    updateLEDs();
  }

  // Timeout the LEDs.
  if ((idle_timeout != 0)
      && (last_message_time + idle_timeout < millis())) {
    isOn = false;
  }
  if ((idle_timeout_2 != 0)
      && (last_message_time + idle_timeout_2 < millis())) {
    isOn_2 = false;
  }

  loop_counter++;
  delay(DELAY_VALUE);
}


void updateLEDs()
{
  for (int x = 0; x < LED_COUNT / 2; x++) {
    pixels.setPixelColor(x, pixels.Color(routines.red(x),
                                         routines.green(x),
                                         routines.blue(x)));
  }

  int y = 0;
  for (int x = LED_COUNT / 2; x < LED_COUNT; x++) {
    pixels.setPixelColor(x, pixels.Color(routines_2.red(y),
                                         routines_2.green(y),
                                         routines_2.blue(y)));
    y++;
  }
  // Neopixels use the show function to update the pixels
  pixels.show();
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

void changeLightingRoutine_2(ELightingRoutine currentMode)
{
  switch (currentMode)
  {
    case eOff:
      routines_2.turnOff();
      break;

    case eSingleSolid:
      routines_2.singleSolid(routines_2.mainColor().red, routines_2.mainColor().green, routines_2.mainColor().blue);
      break;

    case eSingleBlink:
      routines_2.singleBlink(routines_2.mainColor().red, routines_2.mainColor().green, routines_2.mainColor().blue);
      break;

    case eSingleWave:
      routines_2.singleWave(routines_2.mainColor().red, routines_2.mainColor().green, routines_2.mainColor().blue);
      break;

    case eSingleGlimmer:
      routines_2.singleGlimmer(routines_2.mainColor().red, routines_2.mainColor().green, routines_2.mainColor().blue, GLIMMER_PERCENT);
      break;

    case eSingleLinearFade:
      routines_2.singleFade(routines_2.mainColor().red, routines_2.mainColor().green, routines_2.mainColor().blue, false);
      break;

    case eSingleSineFade:
      routines_2.singleFade(routines_2.mainColor().red, routines_2.mainColor().green, routines_2.mainColor().blue, true);
      break;

    case eSingleSawtoothFadeIn:
      routines_2.singleSawtoothFade(routines_2.mainColor().red, routines_2.mainColor().green, routines_2.mainColor().blue, true);
      break;

    case eSingleSawtoothFadeOut:
      routines_2.singleSawtoothFade(routines_2.mainColor().red, routines_2.mainColor().green, routines_2.mainColor().blue, false);
      break;

    case eMultiGlimmer:
      routines_2.multiGlimmer(current_group_2, GLIMMER_PERCENT);
      break;

    case eMultiFade:
      routines_2.multiFade(current_group_2);
      break;

    case eMultiRandomSolid:
      routines_2.multiRandomSolid(current_group_2);
      break;

    case eMultiRandomIndividual:
      routines_2.multiRandomIndividual(current_group_2);
      break;

    case eMultiBarsSolid:
      routines_2.multiBarsSolid(current_group_2, BAR_SIZE);
      break;

    case eMultiBarsMoving:
      routines_2.multiBarsMoving(current_group_2, BAR_SIZE);
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
          success = true;
        }
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
        if ((received_hardware_index == routines_2_index) || (received_hardware_index == 0)) {
          if ((ELightingRoutine)packet_int_array[2] == eOff) {
            isOn_2 = false;
          } else {
            isOn_2 = true;
            // change mode to new mode
            current_routine_2 = (ELightingRoutine)packet_int_array[2];
          }
        }
      }
      // pick up cases where the modes can take extra optional arguments
      if (int_array_size == 4) {
        if (packet_int_array[2] >= (int)eMultiGlimmer) {
          if (packet_int_array[2] != current_routine) {
            // Reset to 0 to draw to screen right away
            loop_counter = 0;
            success = true;
          }
          received_hardware_index = packet_int_array[1];
          if ((received_hardware_index == hardware_index) || (received_hardware_index == 0)) {
            current_routine = (ELightingRoutine)packet_int_array[2];
            current_group = (EColorGroup)packet_int_array[3];
            isOn = true;
          }
          if ((received_hardware_index == routines_2_index) || (received_hardware_index == 0)) {
            current_routine_2 = (ELightingRoutine)packet_int_array[2];
            current_group_2   = (EColorGroup)packet_int_array[3];
            isOn_2 = true;
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
          success = true;
        }
        received_hardware_index = packet_int_array[1];
        if ((received_hardware_index == hardware_index) || (received_hardware_index == 0)) {
          routines.setMainColor(packet_int_array[2],
                                packet_int_array[3],
                                packet_int_array[4]);
          isOn = true;
        }
        if ((received_hardware_index == routines_2_index) || (received_hardware_index == 0)) {
          routines_2.setMainColor(packet_int_array[2],
                                  packet_int_array[3],
                                  packet_int_array[4]);
          isOn_2 = true;
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
          if ((received_hardware_index == routines_2_index) || (received_hardware_index == 0)) {
            routines_2.setColor(color_index,
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
          if ((received_hardware_index == routines_2_index) || (received_hardware_index == 0)) {
            routines_2.brightness(param);
            isOn_2 = true;
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
        if ((received_hardware_index == routines_2_index) || (received_hardware_index == 0)) {
          raw_speed_2 = packet_int_array[2];
          update_speed_2 = (int)((1000.0 / DELAY_VALUE) / (raw_speed_2 / 100.0));
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
        if ((received_hardware_index == routines_2_index) || (received_hardware_index == 0)) {
          unsigned long new_timeout = (unsigned long)packet_int_array[2];
          idle_timeout_2 = new_timeout * 60 * 1000;
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
          if ((received_hardware_index == routines_2_index) || (received_hardware_index == 0)) {
            routines_2.setCustomColorCount(packet_int_array[2]);
          }
        }
      }
      break;
    case eStateUpdateRequest:
      if (int_array_size == 1) {
        skip_echo = true;
        // Send back update
        Bridge.put(F("state_update"), buildStateUpdatePacket());
      }
      break;
    case eCustomArrayUpdateRequest:
      if (int_array_size == 1) {
        skip_echo = true;
        // Send back update
        Bridge.put(F("custom_array_update"), buildCustomArrayUpdatePacket());
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

String buildStateUpdatePacket()
{
  String updatePacket = "";
  updatePacket += (uint8_t)eStateUpdateRequest;
  updatePacket += ",";
  updatePacket += (uint8_t)hardware_index;
  updatePacket += ",";
  updatePacket += (uint8_t)isOn;
  updatePacket += ",";
  updatePacket += (uint8_t)1; // isReachable
  updatePacket += ",";
  updatePacket += (uint8_t)routines.mainColor().red;
  updatePacket += ",";
  updatePacket += (uint8_t)routines.mainColor().green;
  updatePacket += ",";
  updatePacket += (uint8_t)routines.mainColor().blue;
  updatePacket += ",";
  updatePacket += (uint8_t)current_routine;
  updatePacket += ",";
  updatePacket += (uint8_t)current_group;
  updatePacket += ",";
  updatePacket += (uint8_t)routines.brightness();
  updatePacket += ",";
  updatePacket += (uint16_t)raw_speed;
  updatePacket += ",";
  updatePacket += (uint16_t)(idle_timeout / 60000);
  updatePacket += ",";
  updatePacket += calculateMinutesUntilTimeout(last_message_time, idle_timeout);
  updatePacket += "&";

  updatePacket += (uint8_t)eStateUpdateRequest;
  updatePacket += ",";
  updatePacket += (uint8_t)routines_2_index;
  updatePacket += ",";
  updatePacket += (uint8_t)isOn_2;
  updatePacket += ",";
  updatePacket += (uint8_t)1; // isReachable
  updatePacket += ",";
  updatePacket += (uint8_t)routines_2.mainColor().red;
  updatePacket += ",";
  updatePacket += (uint8_t)routines_2.mainColor().green;
  updatePacket += ",";
  updatePacket += (uint8_t)routines_2.mainColor().blue;
  updatePacket += ",";
  updatePacket += (uint8_t)current_routine_2;
  updatePacket += ",";
  updatePacket += (uint8_t)current_group_2;
  updatePacket += ",";
  updatePacket += (uint8_t)routines_2.brightness();
  updatePacket += ",";
  updatePacket += (uint16_t)raw_speed_2;
  updatePacket += ",";
  updatePacket += (uint16_t)(idle_timeout_2 / 60000);
  updatePacket += ",";
  updatePacket += calculateMinutesUntilTimeout(last_message_time, idle_timeout_2);
  updatePacket += "&";

  if (USE_CRC) {
    // add the crc
    updatePacket += crcCalculator(updatePacket);
    updatePacket += "&";
  }
  return updatePacket;
}

String buildCustomArrayUpdatePacket() {
  String updatePacket = "";
  updatePacket += (uint8_t)eCustomArrayUpdateRequest;
  updatePacket += ",";
  updatePacket += (uint8_t)hardware_index;
  for (int i = 0; i < routines.customColorCount(); ++i) {
    updatePacket += ",";
    updatePacket += routines.color(i).red;
    updatePacket += ",";
    updatePacket += routines.color(i).green;
    updatePacket += ",";
    updatePacket += routines.color(i).blue;
  }
  updatePacket += "&";

  updatePacket += (uint8_t)eCustomArrayUpdateRequest;
  updatePacket += ",";
  updatePacket += (uint8_t)hardware_index;
  for (int i = 0; i < routines.customColorCount(); ++i) {
    updatePacket += ",";
    updatePacket += routines_2.color(i).red;
    updatePacket += ",";
    updatePacket += routines_2.color(i).green;
    updatePacket += ",";
    updatePacket += routines_2.color(i).blue;
  }
  updatePacket += "&";

  if (USE_CRC) {
    // add the crc
    updatePacket += crcCalculator(updatePacket);
    updatePacket += "&";
  }
  return updatePacket;
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
 *  @brief delimitedStringToIntArray takes an Arduino string that contains a series of
 *        numbers delimited by commas, converts it to a char array, then converts
 *        that char array into a series of integers. C functions are used here to decrease
 *        PROGMEM size and to decrease the amount of dynamic memory allocation that
 *        is requird for String manipulation.
 *
 * @param message the input string.
 *
 * @return true if the string was parseable, false otherwise.
 */
bool delimitedStringToIntArray(String message)
{
  bool isValid = true;
  int_array_size = 0;
  // ignore messages that are too long
  if (message.length() + 1 < sizeof(char_array)) {
    // append a final comma to simplify parsing
    message += ",";
    // convert to char array
    message.toCharArray(char_array, message.length());

    // check if it contains valid characters.
    for (int i = 0; i < message.length(); i++) {
      if (!(isdigit(message[i])
            || message[i] == ','
            || message[i] == '-')) {
        isValid = false;
      }
    }

    // if the string is parseable, parse it.
    if (isValid) {
      // Get the frist substring delimited by a ","
      char* valuePtr = strtok(char_array, ",");
      while (valuePtr != 0) {
        // convert chars to int and story in int array.
        packet_int_array[int_array_size] = atoi(valuePtr);
        int_array_size++;
        // Find the next substring delimited by a ","
        valuePtr = strtok(0, ",");
      }
      return isValid;
    } else {
      return isValid;
    }
  } else {
    return false;
  }
}

/*!
 * @brief parseMultiMessageString takes an Arduino string that contains a series of
 *        messages delimited by ampersands(&), and converts it to an array of strings.
 *        C functions are used here to decrease PROGMEM size.
 *
 * @param message the input string.
 *
 * @return true if the string was parseable, false otherwise.
 */
bool parseMultiMessageString(String message)
{
  bool isValid = false;
  multi_packet_size = 0;
  // ignore messages that are too long
  if (message.length() < sizeof(multi_packet_char_array)) {
    // convert to char array
    message.toCharArray(multi_packet_char_array, message.length());

    // check if it contains valid characters.
    for (int i = 0; i < message.length(); i++) {
      if (message[i] == '&') {
        isValid = true;
      }
    }

    uint32_t lastPos = 0;
    // if the string is parseable, parse it.
    if (isValid) {
      // Get the frist substring delimited by a "&"
      char* valuePtr = strtok(multi_packet_char_array, "&");
      while (valuePtr != 0) {
        // convert chars to int and story in int array.
        multi_packet_strings[multi_packet_size] = String(valuePtr);
        // Find the next substring delimited by a "&"
        valuePtr = strtok(0, "&");
        if (valuePtr != 0) {
          lastPos += multi_packet_strings[multi_packet_size].length() + 1;
        }
        multi_packet_size++;
      }
    }

    if (isValid) {
      if (USE_CRC) {
        if (multi_packet_size < 2) { 
          // using CRC, but packet is too small 
          return false; 
        } 
        // create a substring of the packet without the CRC
        unsigned long computedCRC = crcCalculator(message.substring(0, lastPos));
        // grab the value of the crc
        unsigned long givenCRC = multi_packet_strings[multi_packet_size - 1].toInt();
        multi_packet_size--; // last packet is a checksum, not a valid packet.
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
    } else {
      // not valid, exit early
      return false;
    }
  } else {
    return false;
  }
}


