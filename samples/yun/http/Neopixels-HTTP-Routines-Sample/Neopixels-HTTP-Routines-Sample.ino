/*!
 * RGB-LED-Routines
 * Sample Sketch
 *
 * Supports Adafruit NeoPixels products.
 * 
 * Provides a HTTP interface to a set of lighting routines.
 *
 * Version 2.1.1
 * Date: January 22, 2016
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */
#include <RoutinesRGB.h>
#include <Adafruit_NeoPixel.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

//================================================================================
// Settings
//================================================================================

const byte CONTROL_PIN       = 6;      // pin used by NeoPixels library
const int  LED_COUNT         = 64;

const byte BAR_SIZE          = 4;      // default length of a bar for bar routines
const byte GLIMMER_PERCENT   = 10;     // percent of "glimmering" LEDs in glimmer routines: range: 0 - 100

const byte DELAY_VALUE       = 50;     // amount of sleep time between loops

const int  DEFAULT_SPEED     = 300;    // default delay for LEDs update, suggested range: 10 (fast) - 1000 (slow).
const int  DEFAULT_TIMEOUT   = 120;    // number of minutes without packets until the arduino times out.

const int  DEFAULT_HW_INDEX  = 1;      // index for this particular microcontroller
const int  MAX_HW_INDEX      = 1;      // number of LED devices connected, 1 for every sample except the multi sample

//=======================
// Stored Values and States
//=======================

ELightingRoutine current_routine = eSingleGlimmer;
EColorGroup current_group = eCustom;

bool isOn = true;

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
String currentPacket;

BridgeClient client;

//=======================
// String Parsing
//=======================

// flag ued by parsing system. if TRUE, continue parsing, if FALSE,
// packet is either illegal or empty and parsing can be skipped.
bool packetReceived = false;

const int max_number_of_messages = 5;
const int max_message_size = 20;

// buffers for receiving messages
String multi_packet_strings[max_number_of_messages];
char multi_packet_char_array[max_message_size * max_number_of_messages];

// buffers for converting ASCII to an int array 
char char_array[20];
int packet_int_array[10];
char num_buf[4];

// used to manipulate the buffers for receiving messages and
// converting them to int arrays.
int multi_packet_size = 0;
int current_multi_packet = 0;
int int_array_size = 0;

//=======================
// RoutinesRGB Setup
//=======================
// Library used to generate the RGB LED routines.
RoutinesRGB routines = RoutinesRGB(LED_COUNT);

//=======================
// Hardware Setup
//=======================

//NOTE: you may need to change the NEO_GRB or NEO_KHZ2800 for this sample to work with your lights.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_COUNT, CONTROL_PIN, NEO_GRB + NEO_KHZ800);
BridgeServer server;

//================================================================================
// Setup and Loop
//================================================================================

void setup()
{
  pixels.begin();

  Bridge.begin();
  server.listenOnLocalhost();
  server.begin();
  // choose the default color for the single
  // color routines. This can be changed at any time.
  // and its set it to green in sample routines.
  // If its not set, it defaults to a faint orange.
  routines.setMainColor(0, 255, 0);
}

void loop()
{
  packetReceived = false;
  client = server.accept();
  if (client) {
    currentPacket = client.readStringUntil('/');
    if (currentPacket.substring(0, 16).equals("DISCOVERY_PACKET")) {
      String discovery = "";
      discovery += "DISCOVERY_PACKET,";
      discovery += (uint8_t)MAX_HW_INDEX;
      discovery += ",";
      discovery += buildStateUpdatePacket();
      client.print(discovery);
    } else {
      client.print(currentPacket); // echo packet back
      packetReceived = true;
    }
  }
  if (packetReceived) {
    // remove any extraneous whitepsace or newline characters
    currentPacket.trim();
    bool multiMessageIsValid = parseMultiMessageString(currentPacket);
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
    updateLEDs();
  }

  // Timeout the LEDs.
  if ((idle_timeout != 0)
      && (last_message_time + idle_timeout < millis())) {
    isOn = false;
  }

  loop_counter++;
  delay(DELAY_VALUE);
  client.stop();
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
        // Send back update
        client.print(buildStateUpdatePacket());
      }
      break;
    case eCustomArrayUpdateRequest:
      if (int_array_size == 1) {
        // Send back update
        client.print(buildCustomArrayUpdatePacket());
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
 * @brief delimitedStringToIntArray takes an Arduino string that contains a series of
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

    // if the string is parseable, parse it.
    if (isValid) {
      // Get the frist substring delimited by a "&"
      char* valuePtr = strtok(multi_packet_char_array, "&");
      while (valuePtr != 0) {
        // convert chars to int and story in int array.
        multi_packet_strings[multi_packet_size] = String(valuePtr);
        multi_packet_size++;
        // Find the next substring delimited by a "&"
        valuePtr = strtok(0, "&");
      }
      return isValid;
    } else {
      return isValid;
    }
  } else {
    return false;
  }
}

