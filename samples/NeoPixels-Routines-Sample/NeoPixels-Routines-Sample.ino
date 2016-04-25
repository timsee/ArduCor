/*!
 * RGB-LED-Routines
 * Sample Sketch
 *
 * Supports Adafruit NeoPixels products.
 *
 * Provides a serial interface to a set of lighting routines generated
 * by the RoutinesRGB library.
 *
 * Version 1.9.0
 * Date: April 24, 2016
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#include <SoftwareSerial.h>
#include <RoutinesRGB.h>
#include <Adafruit_NeoPixel.h>

//================================================================================
// Settings
//================================================================================

const int CONTROL_PIN       = 6;      // pin used by NeoPixels library
const int LED_COUNT         = 64;
const int COLOR_COUNT       = 25;     // Number of array colors

const int DEFAULT_SPEED     = 300;    // default delay for LEDs update, suggested range: 10 (super fast) - 1000 (slow). 
const int DELAY_VALUE       = 3;    // amount of sleep time between loops 
const int DEFAULT_BAR_SIZE  = 4;      // default length of a bar for bar routines
const int DEFAULT_BAR_COUNT = 2;      // the number of colors used in bar routines

const int FADE_SPEED        = 20;     // change rate of solid fade routine, range 1 (slow) - 100 (fast)
const int GLIMMER_PERCENT   = 8;      // percent of "glimmering" LEDs in glimmer routines: range: 0 - 100

const unsigned long DEFAULT_TIMEOUT = 120;  // number of minutes without packets until the arduino times out.

//================================================================================
// Enums
//================================================================================
/*!
 * Mesage headers for packets coming over serial.
 */
enum EPacketHeader
{
  /*!
   * Takes one int parameter that gets cast to ELightingMode.
   */
  eModeChange,
  /*!
   * Takes 3 parameters, a 0-255 representation of Red, Green, and Blue.
   */
  eMainColorChange,
  /*!
   * Takes four parameters, three parameters, the LED, a 0-255 representation of Red, Green, and Blue.
   */
  eCustomArrayColorChange,
  /*!
   * Takes one parameter, sets the brightness between 0 and 100.
   */
  eBrightnessChange,
  /*!
   * Takes one parameter, sets the delay value 1 - 23767.
   */
  eSpeedChange,
  /*!
   * Change the number of colors used in a custom array routine.
   */
  eCustomColorCountChange,
  /*!
   * Set to 0 to turn off, set to any other number minutes until
   * idle timeout happens
   */
  eIdleTimeoutChange,
  /*!
   * Resets all values inside of RoutinesRGB back to their 
   * default values. Useful for soft reseting the LED hardware. 
   */
  eResetSettingsToDefaults,
  ePacketHeader_MAX //total number of Packet Headers
};

//================================================================================
// Variables
//================================================================================

//=======================
// Settings
//=======================

// checked on each frame to see how the LEDs should update
ELightingMode current_mode = eSingleGlimmer;
EColorPreset  current_preset = eCustom;

// contrary to popular belief, light_speed != MC^2. Instead, it is the delay
// between updates of the LEDs. Calculate it by DEFAULT_SPEED * 10msec
int light_speed = (1000 / DELAY_VALUE) / (DEFAULT_SPEED / 100);
int bar_size    = DEFAULT_BAR_SIZE;

// sets for array color routines
int custom_array_count = COLOR_COUNT;

unsigned long idle_timeout = DEFAULT_TIMEOUT * 60 * 1000; // convert to milliseconds

//=======================
// Stored Values and States
//=======================

// turn off the lights when idle
boolean idle_timeout_on = true;

// timeout variable
unsigned long last_message_time;

// counts each loop and uses it to determine
// when to update the LEDs.
unsigned long loop_counter = 0;

//=======================
// String Parsing
//=======================

// delimiter used to break up values sent as strings over serial.
char delimiter = ',';

// struct that stores parsed values
struct ParsedIntPacket
{
  int* values;  // array of values
  byte count;   // count of values
  byte isValid;
};
// stored version of last parsed packet
ParsedIntPacket parsed_packet;

// function prototype to compile with struct
ParsedIntPacket delimitedStringToIntArray(String message);

//=======================
// Miscellaneous
//=======================

// Library used to generate the RGB LED routines.
RoutinesRGB routines = RoutinesRGB(LED_COUNT, COLOR_COUNT);

// prototype function, needed for compiler to recognize the enum.
void currentLightingRoutine(ELightingMode currentMode);

//NOTE: you may need to change the NEO_GRB or NEO_KHZ2800 for this sample to work with your lights. 
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_COUNT, CONTROL_PIN, NEO_GRB + NEO_KHZ800);

//================================================================================
// Setup and Loop
//================================================================================

void setup()
{
  pixels.begin();

  // setup the timeout.
  idle_timeout = DEFAULT_TIMEOUT * 60 * 1000;
  last_message_time = 0;

  routines.setMainColor(0, 255, 0);
  
  // put your setup code here, to run once:
  Serial.begin(19200);
}

void loop()
{
  if (Serial.available()) {
    String currentPacket = Serial.readStringUntil(';');
    // remove any extraneous whitepsace or newline characters
    currentPacket.trim();
    parsed_packet = delimitedStringToIntArray(currentPacket);
    // parse a paceket only if its header is is in the correct range
    if (parsed_packet.isValid
        && parsed_packet.count > 0
        && parsed_packet.values[0] < ePacketHeader_MAX) {
      parsePacket(parsed_packet.values[0]);
    }
  }

  if (!(loop_counter % light_speed)) {
    currentLightingRoutine(current_mode);
    updateLEDs();
  }

  // Timeout the LEDs.
  if (idle_timeout_on && last_message_time + idle_timeout < millis()) {
    routines.solid(0, 0, 0);
    current_mode = eOff;
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
 * Function that runs every loop iteration and determines how to light up the LEDs.
 *
 * @param currentMode the current mode of the program
 */
void currentLightingRoutine(ELightingMode currentMode)
{
  switch (currentMode)
  {
    case eOff:
      routines.solid(0, 0, 0);
      break;

    case eSingleSolid:
      routines.solid(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue);
      break;

    case eSingleBlink:
      routines.blink(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue);
      break;

    case eSingleFade:
      // allow for the color to change independently of the fade animation while setting a color
      if (last_message_time + light_speed > millis()) {
        routines.fade(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue, FADE_SPEED, false);
      } else {
        routines.fade(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue, FADE_SPEED, true);
      }

      break;

    case eSingleGlimmer:
      if (last_message_time + light_speed > millis()) {
        routines.glimmer(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue, GLIMMER_PERCENT, false);
      } else {
        routines.glimmer(routines.mainColor().red, routines.mainColor().green, routines.mainColor().blue, GLIMMER_PERCENT, true);
      }
      break;

    case eMultiGlimmer:
      routines.arrayGlimmer(current_preset, GLIMMER_PERCENT);
      break;

    case eMultiFade:
      routines.arrayFade(current_preset);
      break;
      
    case eMultiRandomSolid:
      routines.arrayRandomSolid(current_preset);
      break;

    case eMultiRandomIndividual:
      routines.arrayRandomIndividual(current_preset);
      break;

    case eMultiBarsSolid:
      routines.arrayBarsSolid(current_preset, bar_size);
      break;

    case eMultiBarsMoving:
      routines.arrayBarsMoving(current_preset, bar_size);
      break;
      
    default:
      //routines.glimmer(0, 255, 0, GLIMMER_PERCENT);
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
RoutinesRGB::Color mainColor;
void parsePacket(int header)
{
  // In each case, theres a final check that the packet was properly
  // formatted by making sure its getting the right number of values.
  boolean success = false;
  boolean requiresRedraw = false;
  switch (header)
  {
    case eModeChange:
      if (parsed_packet.count == 2 && parsed_packet.values[1] < eLightingMode_MAX) {
        if (parsed_packet.values[1] != current_mode) {
          success = true;
        }
        // change mode to new mode
        current_mode = (ELightingMode)parsed_packet.values[1];
      }
      // pick up cases where the modes can take extra optional arguments
      if (parsed_packet.count == 3) {
        if (parsed_packet.values[1] == eMultiGlimmer
            || parsed_packet.values[1] == eMultiRandomIndividual
            || parsed_packet.values[1] == eMultiRandomSolid
            || parsed_packet.values[1] == eMultiFade
            || parsed_packet.values[1] == eMultiBarsSolid
            || parsed_packet.values[1] == eMultiBarsMoving ) {
            if (parsed_packet.values[1] != current_mode) {
              success = true;
            }
            current_mode = (ELightingMode)parsed_packet.values[1];
            current_preset = (EColorPreset)parsed_packet.values[2];
        } 
      }
      break;
    case eMainColorChange:
      if (parsed_packet.count == 4) {
        if (parsed_packet.values[1] != mainColor.red 
        || parsed_packet.values[2] != mainColor.green 
        || parsed_packet.values[3] != mainColor.blue) {
          success = true;
        }
        mainColor.red = parsed_packet.values[1];
        mainColor.green = parsed_packet.values[2];
        mainColor.blue = parsed_packet.values[3];
        routines.setMainColor(mainColor.red, mainColor.green, mainColor.blue);
      }
      break;
    case eCustomArrayColorChange:
      if (parsed_packet.count == 5) {
        int color_index = parsed_packet.values[1];
        if (color_index >= 0 && color_index < eLightingMode_MAX) {
          success = true;
          routines.setColor(color_index, parsed_packet.values[2], parsed_packet.values[3], parsed_packet.values[4]);
        }
      }
      break;
    case eBrightnessChange:
      {
        if (parsed_packet.count == 2) {
          //success = true;
          int param = constrain(parsed_packet.values[1], 0, 100);
          // update brightness level
          routines.brightness(param);
        }
        break;
      }
    case eSpeedChange:
      if (parsed_packet.count == 2) {
        //success = true;
        // convert from 100 * FPS to real data 
        light_speed = (1000 / DELAY_VALUE) / (parsed_packet.values[1] / 100);
      }
      break;
    case eIdleTimeoutChange:
      if (parsed_packet.count == 2) {
        //success = true;
        if (parsed_packet.values[1] == 0) {
          idle_timeout_on = false;
        } else {
          idle_timeout_on = true;
          unsigned long new_timeout = (unsigned long)parsed_packet.values[1];
          idle_timeout = new_timeout * 60 * 1000;
        }
      }
      break;
   case eCustomColorCountChange:
      if (parsed_packet.count == 2) {
        if (parsed_packet.values[1] > 0) {
          custom_array_count = parsed_packet.values[1];
          routines.setColorCount(custom_array_count);
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
            routines.resetToDefaults();
        }
      }
      break;
    default:
      return;
  }
  if (success) {
    last_message_time = millis();
    // Reset to 0 to draw to screen right away only if its been enough time
    loop_counter = 0;
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
    int packetSize = countDelimitedValues(message);
    if (packetSize > 0) {
      int valueStartIndex = 0;
      int valueCounter = 0;
      int valueArray[packetSize];
      for (int i = 0; i < message.length(); i++) {
        if (message.charAt(i) == delimiter) {
          String valueString = message.substring(valueStartIndex, i);
          valueArray[valueCounter] = valueString.toInt();
          valueCounter++;
          valueStartIndex = i + 1;
        }
      }
      // grab the last value of the string
      String valueString = message.substring(valueStartIndex, message.length());
      valueArray[valueCounter] = valueString.toInt();
      ParsedIntPacket packet = {valueArray, packetSize, 1};
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
      Serial.println("String is not properly formatted!");
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
  int delimiters_count = 0;
  for (int i = 0; i < message.length(); i++) {
    if (message.charAt(i) == delimiter) delimiters_count++;
  }

  if (delimiters_count == 0) {
    return 1;
  } else {
    return delimiters_count + 1; // first delimiter has two values
  }
}



