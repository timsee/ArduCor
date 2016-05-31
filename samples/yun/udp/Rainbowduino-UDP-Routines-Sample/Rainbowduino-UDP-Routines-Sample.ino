/*!
 * RGB-LED-Routines
 * Sample Sketch
 *
 * Supports SeeedStudio Rainbowduino projects.
 *
 * Provides a UDP interface to a set of lighting routines.
 * by the RoutinesRGB library.
 *
 * Version 1.9.6
 * Date: May 30, 2016
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */
#include <RoutinesRGB.h>
#include <Rainbowduino.h>
#include <Bridge.h>

//================================================================================
// Settings
//================================================================================

const int LED_COUNT          = 64;

const byte BAR_SIZE          = 4;      // default length of a bar for bar routines
const byte FADE_SPEED        = 50;     // change rate of solid fade routine, range 1 (slow) - 100 (fast)
const byte GLIMMER_PERCENT   = 10;     // percent of "glimmering" LEDs in glimmer routines: range: 0 - 100

const byte DELAY_VALUE       = 3;      // amount of sleep time between loops 

const int DEFAULT_SPEED      = 300;    // default delay for LEDs update, suggested range: 10 (super fast) - 1000 (slow). 
const int DEFAULT_TIMEOUT    = 120;    // number of minutes without packets until the arduino times out.

//=======================
// Stored Values and States
//=======================

// checked on each frame to see how the LEDs should update
ELightingRoutine current_routine = eSingleGlimmer;
EColorGroup  current_group = eCustom;

// contrary to popular belief, light_speed != MC^2. Instead, it is the delay
// between updates of the LEDs. Calculate it by DEFAULT_SPEED * 10msec
int light_speed = (int)((1000.0f / DELAY_VALUE) / (DEFAULT_SPEED / 100.0f));

// timeout variables
unsigned long idle_timeout = (unsigned long)DEFAULT_TIMEOUT * 60 * 1000; // convert to milliseconds
unsigned long last_message_time = 0;

// counts each loop and uses it to determine
// when to update the LEDs.
unsigned long loop_counter = 0;

// used for communication over the Bridge Library
String lastString = "";
char buffer[25];

//=======================
// String Parsing
//=======================

// the current packet being parsed by the loop() function
String currentPacket;
// flag ued by parsing system. if TRUE, continue parsing, if FALSE,
// packet is either illegal, a repeat, or empty and parsing can be skipped.
bool packetReceived = false;

// delimiter used to break up values sent as strings over serial.
char delimiter = ',';

// struct that stores parsed values
struct ParsedIntPacket
{
  int* values;  // array of values
  byte count;   // count of values
  byte isValid;
};

// temporary values used by the string parsing system
ParsedIntPacket parsed_packet;
uint8_t delimited_packet_size = 0;
uint8_t delimited_counter = 0;
uint8_t delimited_index = 0;
String delimited_string;

// function prototype to compile with struct
ParsedIntPacket delimitedStringToIntArray(String message);

//=======================
// RoutinesRGB Setup
//=======================

// Library used to generate the RGB LED routines.
RoutinesRGB routines = RoutinesRGB(LED_COUNT);


//================================================================================
// Setup and Loop
//================================================================================

void setup()
{
  Rb.init();

  Bridge.begin();
  // choose the default color for the single
  // color routines. This can be changed at any time.
  // and its set it to green in sample routines. 
  // If its not set, it defaults to a faint orange. 
  routines.setMainColor(0, 255, 0);

  // put your setup code here, to run once:
  Serial.begin(19200);
}

void loop()
{
 packetReceived = false;
  Bridge.get("udp", buffer, 25); 
  String tempString(buffer);
  if (tempString != lastString) {
    currentPacket = tempString; 
    lastString = currentPacket;
    packetReceived = true;
  }
  if (packetReceived) {
    currentPacket.trim();
    parsed_packet = delimitedStringToIntArray(currentPacket);
    // parse a paceket only if its header is is in the correct range
    if (parsed_packet.isValid
        && (parsed_packet.count > 0)
        && (parsed_packet.values[0] < ePacketHeader_MAX)) {
      parsePacket(parsed_packet.values[0]);
    }
  }

  if (!(loop_counter % light_speed)) {
    currentLightingRoutine(current_routine);
    updateLEDs();
  }

  // Timeout the LEDs.
  if ((idle_timeout != 0) && 
      (last_message_time + idle_timeout < millis())) {
    routines.singleSolid(0, 0, 0);
    current_routine = eOff;
  }

  loop_counter++;
  delay(DELAY_VALUE);
}


void updateLEDs()
{
  int index = 0;
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++)  {
      Rb.setPixelXY(x, y,
                    routines.red(index),
                    routines.green(index),
                    routines.blue(index));
      index++;
    }
  }
}


//================================================================================
// Mode Management
//================================================================================

/*!
 * Function that runs every loop iteration and determines how to light up the LEDs.
 *
 * @param currentMode the current mode of the program
 */
void currentLightingRoutine(ELightingRoutine currentMode)
{
  switch (currentMode)
  {
    case eOff:
      routines.singleSolid(0, 0, 0);
      break;

    case eSingleSolid:
      routines.singleSolid(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue);
      break;

    case eSingleBlink:
      routines.singleBlink(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue);
      break;

    case eSingleFade:
      // allow for the color to change independently of the fade animation while setting a color
      if (last_message_time + light_speed > millis()) {
        routines.singleFade(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue, FADE_SPEED, false);
      } else {
        routines.singleFade(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue, FADE_SPEED, true);
      }

      break;

    case eSingleGlimmer:
      if (last_message_time + light_speed > millis()) {
        routines.singleGlimmer(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue, GLIMMER_PERCENT, false);
      } else {
        routines.singleGlimmer(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue, GLIMMER_PERCENT, true);
      }
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
 * This parser looks at the header of a control packet and from that determines
 * which parameters to use and what settings to change.
 */
void parsePacket(int header)
{
  // In each case, theres a final check that the packet was properly
  // formatted by making sure its getting the right number of values.
  boolean success = false;
  switch (header)
  {
    case eModeChange:
      if (parsed_packet.count == 2 && parsed_packet.values[1] < eLightingRoutine_MAX) {
        if (parsed_packet.values[1] != current_routine) {
            // Reset to 0 to draw to screen right away
            loop_counter = 0;
            success = true;
        }
        // change mode to new mode
        current_routine = (ELightingRoutine)parsed_packet.values[1];
      }
      // pick up cases where the modes can take extra optional arguments
      if (parsed_packet.count == 3) {
        if (parsed_packet.values[1] >= (int)eMultiGlimmer) {
            if (parsed_packet.values[1] != current_routine) {
              // Reset to 0 to draw to screen right away
              loop_counter = 0;
              success = true;
            }
            current_routine = (ELightingRoutine)parsed_packet.values[1];
            current_group = (EColorGroup)parsed_packet.values[2];
        } 
      }
      break;
    case eMainColorChange:
      if (parsed_packet.count == 4) {
        if (parsed_packet.values[1] != routines.mainColor().red 
        || parsed_packet.values[2] != routines.mainColor().green 
        || parsed_packet.values[3] != routines.mainColor().blue) {
          // Reset to 0 to draw to screen right away
          loop_counter = 0;
          success = true;
        }
        routines.setMainColor(parsed_packet.values[1], 
                              parsed_packet.values[2], 
                              parsed_packet.values[3]);
      }
      break;
    case eCustomArrayColorChange:
      if (parsed_packet.count == 5) {
        int color_index = parsed_packet.values[1];
        if (color_index >= 0 && color_index < eLightingRoutine_MAX) {
          // Reset to 0 to draw to screen right away
          loop_counter = 0;
          success = true;
          routines.setColor(color_index, 
                            parsed_packet.values[2], 
                            parsed_packet.values[3], 
                            parsed_packet.values[4]);
        }
      }
      break;
    case eBrightnessChange:
      {
        if (parsed_packet.count == 2) {
          success = true;
          int param = constrain(parsed_packet.values[1], 0, 100);
          // update brightness level
          routines.brightness(param);
        }
        break;
      }
    case eSpeedChange:
      if (parsed_packet.count == 2) {
        success = true;
        // convert from 100 * FPS to real data 
        light_speed = (int)((1000.0f / DELAY_VALUE) / (parsed_packet.values[1] / 100.0f));
      }
      break;
    case eIdleTimeoutChange:
      if (parsed_packet.count == 2) {
        success = true;
        unsigned long new_timeout = (unsigned long)parsed_packet.values[1];
        idle_timeout = new_timeout * 60 * 1000;
      }
      break;
   case eCustomColorCountChange:
      if (parsed_packet.count == 2) {
        if (parsed_packet.values[1] > 0) {
          success = true;
          routines.setCustomColorCount(parsed_packet.values[1]);
        }
      }
      break;
   case eResetSettingsToDefaults:
      if (parsed_packet.count == 3) {
        // reset is based off of a sequence of 3 bytes
        // and the message must be exactly this size.
        // this prevents buffer issues from causing
        // false positives on just one byte message 
        // sizes. 
        if ((parsed_packet.values[1] == 42) 
            && (parsed_packet.values[2] == 71)) {
            success = true;
            routines.resetToDefaults();
        }
      }
      break;
    default:
      return;
  }
  if (success) {
    last_message_time = millis();
  }
}

//================================================================================
// String Parsing
//================================================================================

/*!
 * Takes a string that is delimited by command and converts it to an int array.
 *
 * @param message the incoming delimited string
 *
 * @return an int array that contains the values in the delimited string
 */
ParsedIntPacket delimitedStringToIntArray(String message)
{
  if (checkIfValidString(message)) {
    delimited_packet_size = countDelimitedValues(message);
    if (delimited_packet_size > 0) {
      delimited_index = 0;
      delimited_counter = 0;
      int valueArray[delimited_packet_size];
      for (int i = 0; i < message.length(); i++) {
        if (message.charAt(i) == delimiter) {
          delimited_string = message.substring(delimited_index, i);
          valueArray[delimited_counter] = delimited_string.toInt();
          delimited_counter++;
          delimited_index = i + 1;
        }
      }
      // grab the last value of the string
      valueArray[delimited_counter] = message.substring(delimited_index, message.length()).toInt();
      ParsedIntPacket packet = {valueArray, delimited_packet_size, 1};
      return packet;
    }
  }
  // somethign went wrong, treat it as junk
  int junkArray[0];
  ParsedIntPacket packet = {junkArray, 0, 0};
  return packet;
}

/*!
 * Does a naive check on if the string is parseable. For now, it just
 * makes sure that every character is either a digit, a negative sign,
 * or a delimiter.
 *
 * @param message the current delimited string
 *
 * @return true if it passes validity test, false otherwise
 */
boolean checkIfValidString(String message)
{
  for (int i = 0; i < message.length(); i++) {
    if (!(isDigit(message.charAt(i))
          || message.charAt(i) == delimiter
          || message.charAt(i) == '-')) {
      Serial.println("Invalid String");
      return false;
    }
  }
  return true;
}

/*!
 * Helper function that scans the incoming string and determines how
 * many values are in it.
 *
 * @param message the current delimited string
 *
 * @return number of values in delimited string
 */
int countDelimitedValues(String message)
{
  delimited_counter = 0;
  for (int i = 0; i < message.length(); i++) {
    if (message.charAt(i) == delimiter) delimited_counter++;
  }

  if (delimited_counter == 0) {
    return 1;
  } else {
    return delimited_counter + 1; // first delimiter has two values
  }
}



