#define IS_RAINBOWDUINO 0
#define IS_NEOPIXEL 1
#define IS_SINGLE_LED 0
#define IS_YUN 0
/*!
 * RGB-LED-Routines
 * Sample Sketch
 *
 * DESCRIPTION_PLACEHOLDER
 *
 * COM_PLACEHOLDER
 * by the RoutinesRGB library.
 *
 * Version 1.8
 * Date: February 28, 2015
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#include <SoftwareSerial.h>
#include <RoutinesRGB.h>
#if IS_NEOPIXEL
#include <Adafruit_NeoPixel.h>
#endif
#if IS_RAINBOWDUINO
#include <Rainbowduino.h>
#endif
#if IS_YUN
#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#endif

//================================================================================
// Settings
//================================================================================

#if IS_NEOPIXEL
const int CONTROL_PIN       = 6;      // pin used by NeoPixels library
#endif
#if IS_SINGLE_LED
const int R_PIN             = 5;      // SINGLE_LED only
const int G_PIN             = 4;      // SINGLE_LED only
const int B_PIN             = 3;      // SINGLE_LED only
const int IS_COMMON_ANODE   = 1;      // SINGLE_LED only, 0 if common cathode, 1 if common anode
#endif
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

// Lighting modes supported by this sketch This is technically more modes than the library
// supports, but thats because the sketch treats different colors as different modes.
enum ELightingMode
{
  eLightsOff,
  /*!
   * Single color modes
   */
  eSolidConstant,
  eSolidBlink,
  eSolidFade,
  eSolidGlimmer,
  /*!
   * saved color modes
   */
  eRedGlimmer,
  eOrangeGlimmer,
  eYellowGlimmer,
  eGreenGlimmer,
  eTealGlimmer,
  eBlueGlimmer,
  ePurpleGlimmer,
  eLightBlueGlimmer,
  ePinkGlimmer,
  eWhiteGlimmer,
  /*!
   * Every individual LED gets its own random color.
   */
  eRandomIndividual,
  /*!
   * One random color is assigned to every LED.
   */
  eRandomSolid,
  /*!
   * Fades in between all the colors.
   */
  eFadeSolidColors,
  /*!
   * Does color[0] as main color and uses other colors in
   * the array as glimmer.
   */
  eArrayGlimmer,
  /*!
  * Chooses a random color from the array for each
  * individual LED.
  */
  eArrayRandomIndividual,
  /*!
   * Chooses a random color from the array and lights all
   * all LEDs to match that color.
   */
  eArrayRandomSolid,
  /*!
   * Uses the colors in the array and fades between them.
   */
  eArrayFade,
  /*!
   * Colors alternating in groups of equal size
   */
  eArrayBarSolid,
  /*!
   * Colors alternating in groups of equal size and moving up in indices
   */
  eArrayBarMoving,
  eLightingMode_MAX //total number of modes
};


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
  eColorArrayChange,
  /*!
   * Takes one parameter, sets the brightness between 0 and 100.
   */
  eBrightnessChange,
  /*!
   * Takes one parameter, sets the delay value 1 - 23767.
   */
  eSpeedChange,
  /*!
   * Set to 0 to turn off, set to any other number minutes until
   * idle timeout happens
   */
  eIdleTimeoutChange,
  ePacketHeader_MAX //total number of Packet Headers
};

//================================================================================
// Variables
//================================================================================

//=======================
// Settings
//=======================

// checked on each frame to see how the LEDs should update
ELightingMode current_mode = eSolidGlimmer;

// contrary to popular belief, light_speed != MC^2. Instead, it is the delay
// between updates of the LEDs. Calculate it by DEFAULT_SPEED * 10msec
int light_speed = (1000 / DELAY_VALUE) / (DEFAULT_SPEED / 100);
int bar_size    = DEFAULT_BAR_SIZE;

// sets for array color routines
int array_bar_count               = DEFAULT_BAR_COUNT;
int array_glimmer_count           = COLOR_COUNT;
int array_random_solid_count      = COLOR_COUNT;
int array_random_individual_count = COLOR_COUNT;
int array_fade_count              = COLOR_COUNT;

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

#if IS_NEOPIXEL
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_COUNT, CONTROL_PIN, NEO_GRB + NEO_KHZ800);
#endif
#if IS_YUN
YunServer server;
#endif

//================================================================================
// Setup and Loop
//================================================================================

void setup()
{
#if IS_RAINBOWDUINO
  Rb.init();
#endif
#if IS_NEOPIXEL
  pixels.begin();
#endif
#if IS_SINGLE_LED
  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
#endif
#if IS_YUN
  Bridge.begin();
  server.listenOnLocalhost();
  server.begin();
#endif

  // setup the timeout.
  idle_timeout = DEFAULT_TIMEOUT * 60 * 1000;
  last_message_time = 0;

  routines.setMainColor(0, 255, 0);

  for (int i = 0; i < COLOR_COUNT; i = i + 5) {
    routines.setColor(i, 0,   255, 0);   // green
    routines.setColor(i + 1, 125, 0,   255); // teal
    routines.setColor(i + 2, 0,   0,   255); // blue
    routines.setColor(i + 3, 40,  127, 40);  // light green
    routines.setColor(i + 4, 60,  0,   160); // purple
  }
  // put your setup code here, to run once:
  Serial.begin(19200);
}

void loop()
{
#if IS_YUN
  YunClient client = server.accept();
  if (client) {
    String command = client.readStringUntil('/');
    client.stop();
  }
#endif
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
    current_mode = eLightsOff;
  }

  loop_counter++;
  delay(DELAY_VALUE);
}


void updateLEDs()
{
#if IS_RAINBOWDUINO
  int index = 0;
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++)  {
      Rb.setPixelXY(x, y,
                    routines.getR(index),
                    routines.getG(index),
                    routines.getB(index));
      index++;
    }
  }
#endif
#if IS_NEOPIXEL
  for (int x = 0; x < LED_COUNT; x++) {
    pixels.setPixelColor(x, pixels.Color(routines.getR(x),
                                         routines.getG(x),
                                         routines.getB(x)));
  }
  pixels.show();
#endif
#if IS_SINGLE_LED
  if (IS_COMMON_ANODE) {
    analogWrite(R_PIN, 255 - routines.getR(0));
    analogWrite(G_PIN, 255 - routines.getG(0));
    analogWrite(B_PIN, 255 - routines.getB(0));
  }
  else {
    analogWrite(R_PIN, routines.getR(0));
    analogWrite(G_PIN, routines.getG(0));
    analogWrite(B_PIN, routines.getB(0));
  }
#endif
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
    case eLightsOff:
      routines.solid(0, 0, 0);
      break;

    case eSolidConstant:
      routines.solid(routines.getMainColor().r, routines.getMainColor().g, routines.getMainColor().b);
      break;

    case eSolidBlink:
      routines.blink(routines.getMainColor().r, routines.getMainColor().g, routines.getMainColor().b);
      break;

    case eSolidFade:
      // allow for the color to change independently of the fade animation while setting a color
      if (last_message_time + light_speed > millis()) {
        routines.fade(routines.getMainColor().r, routines.getMainColor().g, routines.getMainColor().b, FADE_SPEED, false);
      } else {
        routines.fade(routines.getMainColor().r, routines.getMainColor().g, routines.getMainColor().b, FADE_SPEED, true);
      }

      break;

    case eSolidGlimmer:
      if (last_message_time + light_speed > millis()) {
        routines.glimmer(routines.getMainColor().r, routines.getMainColor().g, routines.getMainColor().b, GLIMMER_PERCENT, false);
      } else {
        routines.glimmer(routines.getMainColor().r, routines.getMainColor().g, routines.getMainColor().b, GLIMMER_PERCENT, true);
      }
      break;

    case eRedGlimmer:
      routines.glimmer(255, 0, 0, GLIMMER_PERCENT, true);
      break;

    case eOrangeGlimmer:
      routines.glimmer(255, 60, 0, GLIMMER_PERCENT, true);
      break;

    case eYellowGlimmer:
      routines.glimmer(255, 255, 0, GLIMMER_PERCENT, true);
      break;

    case eGreenGlimmer:
      routines.glimmer(0, 255, 0, GLIMMER_PERCENT, true);
      break;

    case eTealGlimmer:
      routines.glimmer(0, 255, 80, GLIMMER_PERCENT, true);
      break;

    case eBlueGlimmer:
      routines.glimmer(0, 0, 255, GLIMMER_PERCENT, true);
      break;

    case ePurpleGlimmer:
      routines.glimmer(60, 0, 160, GLIMMER_PERCENT, true);
      break;

    case eLightBlueGlimmer:
      routines.glimmer(80, 80, 255, GLIMMER_PERCENT, true);
      break;

    case ePinkGlimmer:
      routines.glimmer(255, 80, 80, GLIMMER_PERCENT, true);
      break;

    case eWhiteGlimmer:
      routines.glimmer(255, 255, 255, GLIMMER_PERCENT, true);
      break;

    case eRandomIndividual:
      routines.randomIndividual();
      break;

    case eRandomSolid:
      routines.randomSolid();
      break;

    case eFadeSolidColors:
      routines.fadeBetweenAllColors();
      break;

    case eArrayGlimmer:
      routines.arrayGlimmer(array_glimmer_count, GLIMMER_PERCENT);
      break;

    case eArrayRandomSolid:
      routines.arrayRandomSolid(array_random_solid_count);
      break;

    case eArrayRandomIndividual:
      routines.arrayRandomIndividual(array_random_individual_count);
      break;

    case eArrayFade:
      routines.arrayFade(array_fade_count);
      break;

    case eArrayBarSolid:
      routines.arrayBarSolid(array_bar_count, bar_size);
      break;

    case eArrayBarMoving:
      routines.arrayBarMoving(array_bar_count, bar_size);
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
        if (parsed_packet.values[1] == eArrayGlimmer) {
          if (parsed_packet.values[1] != current_mode) {
            success = true;
          }
          current_mode = (ELightingMode)parsed_packet.values[1];
          array_glimmer_count = parsed_packet.values[2];
        } else if (parsed_packet.values[1] == eArrayRandomIndividual) {
          if (parsed_packet.values[1] != current_mode) {
            success = true;
          }
          current_mode = (ELightingMode)parsed_packet.values[1];
          array_random_individual_count = parsed_packet.values[2];
        } else if (parsed_packet.values[1] == eArrayRandomSolid) {
          if (parsed_packet.values[1] != current_mode) {
            success = true;
          }
          current_mode = (ELightingMode)parsed_packet.values[1];
          array_random_solid_count = parsed_packet.values[2];
        } else if (parsed_packet.values[1] == eArrayFade) {
          if (parsed_packet.values[1] != current_mode) {
            success = true;
          }
          current_mode = (ELightingMode)parsed_packet.values[1];
          array_fade_count = parsed_packet.values[2];
        } else if (parsed_packet.values[1] == eArrayBarSolid) {
          if (parsed_packet.values[1] != current_mode) {
            success = true;
          }
          current_mode = (ELightingMode)parsed_packet.values[1];
          // set the number of bars used by the routine
          array_bar_count = parsed_packet.values[2];
        } else if (parsed_packet.values[1] == eArrayBarMoving) {
          if (parsed_packet.values[1] != current_mode) {
            success = true;
          }
          current_mode = (ELightingMode)parsed_packet.values[1];
          // set the number of bars used by the routine
          array_bar_count = parsed_packet.values[2];
        }
      }
      break;
    case eMainColorChange:
      if (parsed_packet.count == 4) {
        if (parsed_packet.values[1] != mainColor.r || parsed_packet.values[2] != mainColor.g || parsed_packet.values[3] != mainColor.b) {
          success = true;
        }
        mainColor.r = parsed_packet.values[1];
        mainColor.g = parsed_packet.values[2];
        mainColor.b = parsed_packet.values[3];
        routines.setMainColor(mainColor.r, mainColor.g, mainColor.b);
      }
      break;
    case eColorArrayChange:
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
          routines.setBrightness(param);
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



