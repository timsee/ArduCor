/*!
 * RGB-LED-Routines
 * Sample Sketch
 *
 * Supports Adafruit NeoPixels products.
 * 
 * Provides a HTTP interface to a set of lighting routines.
 *
 * Version 2.0.0
 * Date: July 31, 2016
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

// used in sketches with multiple hardware connected to one arduino.
uint8_t received_hardware_index;
uint8_t hardware_index = DEFAULT_HW_INDEX;

// value determines how quickly the LEDs udpate. Lower values lead to faster updates
int update_speed = (int)((1000.0 / DELAY_VALUE) / (DEFAULT_SPEED / 100.0));

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
// packet is either illegal, a repeat, or empty and parsing can be skipped.
bool packetReceived = false;

char char_array[30];
int packet_int_array[10];
int int_array_size = 0;

char num_buf[4];

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
      client.print("DISCOVERY_PACKET,");
      client.print(MAX_HW_INDEX);
      client.print(",");
      client.println(buildStateUpdatePacket());
    } else {
      client.println(currentPacket); // echo packet back
      packetReceived = true;
    }
  }
  if (packetReceived) {
    // remove any extraneous whitepsace or newline characters
    currentPacket.trim();
    bool isValid = delimitedStringToIntArray(currentPacket);
    // parse a paceket only if its header is is in the correct range
    if (isValid
        && (int_array_size > 0)
        && (packet_int_array[0] < ePacketHeader_MAX)) {
     if(parsePacket(packet_int_array[0])) {
      last_message_time = millis();
    }
    }
  }

  if (!(loop_counter % update_speed)) {
    changeLightingRoutine(current_routine);
    routines.applyBrightness(); // Optional. Dims the LEDs based on the routines.brightness() setting
    updateLEDs();
  }

  // Timeout the LEDs.
  if ((idle_timeout != 0)
      && (last_message_time + idle_timeout < millis())) {
    routines.singleSolid(0, 0, 0);
    current_routine = eOff;
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
          // change mode to new mode
          current_routine = (ELightingRoutine)packet_int_array[2];
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
          }
        }
        break;
      }
    case eSpeedChange:
      if (int_array_size == 3) {
        success = true;
        received_hardware_index = packet_int_array[1];
        if ((received_hardware_index == hardware_index) || (received_hardware_index == 0)) {
          update_speed = (int)((1000.0 / DELAY_VALUE) / (packet_int_array[2] / 100.0));
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
        client.println(buildStateUpdatePacket());
        // count this is as a valid message, despite
        // it not setting success to true. 
        last_message_time = millis();
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
  updatePacket += (uint8_t)routines.isOn(); 
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
  
  return updatePacket;
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
      while (valuePtr != 0)
      {
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
