#define IS_RAINBOWDUINO 0 
#define IS_NEOPIXELS 1 
#define IS_SINGLE_LED 0 
#define IS_CUSTOM 0 
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
 * Version 1.9.1
 * Date: May 1, 2016
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */
#include <RoutinesRGB.h>
#if IS_NEOPIXELS
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
#if IS_CUSTOM
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#endif

//================================================================================
// Settings
//================================================================================

#if IS_NEOPIXELS
const byte CONTROL_PIN       = 6;      // pin used by NeoPixels library
#endif
#if IS_SINGLE_LED
const byte R_PIN             = 5;      // SINGLE_LED only
const byte G_PIN             = 4;      // SINGLE_LED only
const byte B_PIN             = 3;      // SINGLE_LED only
const byte IS_COMMON_ANODE   = 1;      // SINGLE_LED only, 0 if common cathode, 1 if common anode
#endif
#if IS_CUSTOM
const byte CONTROL_PIN       = 6;     
const byte CONTROL_PIN_2     = 5;     

const byte CUBE_IN           = 4;    
const byte CUBE_OUT          = 3; 
#endif
const int LED_COUNT          = 64;

const byte BAR_SIZE          = 4;      // default length of a bar for bar routines
const byte FADE_SPEED        = 20;     // change rate of solid fade routine, range 1 (slow) - 100 (fast)
const byte GLIMMER_PERCENT   = 10;     // percent of "glimmering" LEDs in glimmer routines: range: 0 - 100
const byte DELAY_VALUE       = 3;      // amount of sleep time between loops 

const int DEFAULT_SPEED      = 300;    // default delay for LEDs update, suggested range: 10 (super fast) - 1000 (slow). 
const int DEFAULT_TIMEOUT    = 120;    // number of minutes without packets until the arduino times out.

//=======================
// Stored Values and States
//=======================

// checked on each frame to see how the LEDs should update
ELightingMode current_mode = eSingleGlimmer;
EColorPreset  current_preset = eCustom;

// contrary to popular belief, light_speed != MC^2. Instead, it is the delay
// between updates of the LEDs. Calculate it by DEFAULT_SPEED * 10msec
int light_speed = (1000 / DELAY_VALUE) / (DEFAULT_SPEED / 100);

// timeout variables
unsigned long idle_timeout = (unsigned long)DEFAULT_TIMEOUT * 60 * 1000; // convert to milliseconds
unsigned long last_message_time = 0;

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

//=======================
// Hardware Setup
//=======================

#if IS_NEOPIXELS
//NOTE: you may need to change the NEO_GRB or NEO_KHZ2800 for this sample to work with your lights. 
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_COUNT, CONTROL_PIN, NEO_GRB + NEO_KHZ800);
#endif
#if IS_CUSTOM
// NeoPixels controller object
Adafruit_NeoPixel pixels_desk = Adafruit_NeoPixel(60, CONTROL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels_dresser = Adafruit_NeoPixel(60, CONTROL_PIN_2, NEO_GRB + NEO_KHZ800);

SoftwareSerial cubeSerial(CUBE_IN, CUBE_OUT); // RX, TX
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
#if IS_NEOPIXELS
  pixels.begin();
#endif
#if IS_SINGLE_LED
  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);
#endif
#if IS_CUSTOM
  pixels_desk.begin();
  pixels_dresser.begin(); 
  cubeSerial.begin(19200); 
#endif
#if IS_YUN
  Bridge.begin();
  server.listenOnLocalhost();
  server.begin();
#endif

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
#if IS_CUSTOM
    String repeatString = currentPacket + ";";
    cubeSerial.println(repeatString);
#endif
    parsed_packet = delimitedStringToIntArray(currentPacket);
    // parse a paceket only if its header is is in the correct range
    if (parsed_packet.isValid
        && (parsed_packet.count > 0)
        && (parsed_packet.values[0] < ePacketHeader_MAX)) {
      parsePacket(parsed_packet.values[0]);
    }
  }

  if (!(loop_counter % light_speed)) {
    currentLightingRoutine(current_mode);
    updateLEDs();
  }

  // Timeout the LEDs.
  if ((idle_timeout != 0) && 
      (last_message_time + idle_timeout < millis())) {
    routines.solid(0, 0, 0);
    current_mode = eOff;
  }

  loop_counter++;
  delay(DELAY_VALUE);
}


#if IS_RAINBOWDUINO
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
#endif
#if IS_NEOPIXELS
void updateLEDs()
{
  for (int x = 0; x < LED_COUNT; x++) {
    pixels.setPixelColor(x, pixels.Color(routines.red(x),
                                         routines.green(x),
                                         routines.blue(x)));
  }
  pixels.show();
}
#endif
#if IS_SINGLE_LED
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
#endif
#if IS_CUSTOM
int reverse_counter;
void updateLEDs()
{
  for(int x = 0; x < LED_COUNT; x++) 
    {
     pixels_desk.setPixelColor(x, pixels_desk.Color(routines.red(x),
                                                    routines.green(x),
                                                    routines.blue(x)));
    }

    reverse_counter = LED_COUNT;
    for(int x = 0; x < LED_COUNT; x++) 
    {
      pixels_dresser.setPixelColor(reverse_counter, pixels_dresser.Color(routines.red(x),
                                                                         routines.green(x),
                                                                         routines.blue(x)));
      reverse_counter--;
    }
    // Neopixels use the show function to update the pixels
    pixels_desk.show();
    pixels_dresser.show();
}
#endif


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
      routines.arrayBarsSolid(current_preset, BAR_SIZE);
      break;

    case eMultiBarsMoving:
      routines.arrayBarsMoving(current_preset, BAR_SIZE);
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
      if (parsed_packet.count == 2 && parsed_packet.values[1] < eLightingMode_MAX) {
        if (parsed_packet.values[1] != current_mode) {
            // Reset to 0 to draw to screen right away
            loop_counter = 0;
            success = true;
        }
        // change mode to new mode
        current_mode = (ELightingMode)parsed_packet.values[1];
      }
      // pick up cases where the modes can take extra optional arguments
      if (parsed_packet.count == 3) {
        if (parsed_packet.values[1] >= (int)eMultiGlimmer) {
            if (parsed_packet.values[1] != current_mode) {
              // Reset to 0 to draw to screen right away
              loop_counter = 0;
              success = true;
            }
            current_mode = (ELightingMode)parsed_packet.values[1];
            current_preset = (EColorPreset)parsed_packet.values[2];
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
        if (color_index >= 0 && color_index < eLightingMode_MAX) {
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
        light_speed = (1000 / DELAY_VALUE) / (parsed_packet.values[1] / 100);
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
          routines.setColorCount(parsed_packet.values[1]);
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



