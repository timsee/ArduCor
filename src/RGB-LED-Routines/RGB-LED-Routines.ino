#define IS_RAINBOWDUINO 0 
#define IS_NEOPIXELS 1 
#define IS_SINGLE_LED 0 
#define IS_CUSTOM 0 
#define IS_SERIAL 1 
#define IS_HTTP 0 
#define IS_UDP 0 
/*!
 * RGB-LED-Routines
 * Sample Sketch
 *
 * DESCRIPTION_PLACEHOLDER
 *
 * COM_PLACEHOLDER
 * by the RoutinesRGB library.
 *
 * Version 1.9.8
 * Date: July 3, 2016
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
#if IS_HTTP
#include <BridgeServer.h>
#include <BridgeClient.h>
#endif
#if IS_UDP
#include <Bridge.h>
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
const int LED_COUNT          = 64;
#endif
#if IS_RAINBOWDUINO
const int LED_COUNT          = 64;
#endif
#if IS_SINGLE_LED
const byte R_PIN             = 5;
const byte G_PIN             = 4;
const byte B_PIN             = 3;
const byte IS_COMMON_ANODE   = 1;      // 0 if common cathode, 1 if common anode
#endif
#if IS_CUSTOM
const byte CONTROL_PIN       = 5;
const byte CUBE_IN           = 4;
const byte CUBE_OUT          = 3;
const int LED_COUNT          = 120;
#endif

const byte BAR_SIZE          = 4;      // default length of a bar for bar routines
const byte GLIMMER_PERCENT   = 10;     // percent of "glimmering" LEDs in glimmer routines: range: 0 - 100

#if IS_SERIAL
const byte DELAY_VALUE       = 3;      // amount of sleep time between loops 
#endif
#if IS_UDP
const byte DELAY_VALUE       = 3;      // amount of sleep time between loops 
#endif
#if IS_HTTP
const byte DELAY_VALUE       = 50;     // amount of sleep time between loops 
#endif

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

#if IS_UDP
// used for communication over the Bridge Library
String lastString = "";
char buffer[25];
#endif

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

#if IS_NEOPIXELS
//=======================
// Hardware Setup
//=======================

//NOTE: you may need to change the NEO_GRB or NEO_KHZ2800 for this sample to work with your lights. 
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_COUNT, CONTROL_PIN, NEO_GRB + NEO_KHZ800);
#endif
#if IS_CUSTOM
//=======================
// Hardware Setup
//=======================

// NeoPixels controller object
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(120, CONTROL_PIN, NEO_GRB + NEO_KHZ800);

SoftwareSerial cubeSerial(CUBE_IN, CUBE_OUT); // RX, TX
#endif
#if IS_HTTP

BridgeServer server;
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
  pixels.begin();
  cubeSerial.begin(19200); 
  while (!cubeSerial);
#endif

#if IS_HTTP
  Bridge.begin();
  server.listenOnLocalhost();
  server.begin();
#endif
#if IS_UDP
  Bridge.begin();
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
 packetReceived = false;
#if IS_SERIAL
  if (Serial.available()) {
    currentPacket = Serial.readStringUntil(';');
    // remove any extraneous whitepsace or newline characters
    packetReceived = true;
  }
#endif
#if IS_HTTP
  BridgeClient client = server.accept();
  if (client) {
    currentPacket = client.readStringUntil('/');
    client.stop();
    packetReceived = true;
  }
#endif
#if IS_UDP
  Bridge.get("udp", buffer, 25); 
  String tempString(buffer);
  if (tempString != lastString) {
    currentPacket = tempString; 
    lastString = currentPacket;
    packetReceived = true;
  }
#endif
  if (packetReceived) {
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
    currentLightingRoutine(current_routine);
    routines.applyBrightness(); // Optional. uses brightness setting
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
void updateLEDs()
{
  for(int x = 0; x < LED_COUNT; x++) 
    {
     pixels.setPixelColor(x, pixels.Color(routines.red(x),
                                          routines.green(x),
                                          routines.blue(x)));
    }
    // Neopixels use the show function to update the pixels
    pixels.show();
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
          success = true;         
          
          // only tell the routines to reset themselves if a custom routine is used. 
          if ((current_routine > eSingleSawtoothFadeOut)
              && (current_group == eCustom)) {
            // Reset LEDS
            loop_counter = 0;
          }
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
        // reset requires a message with exactly 2 parameters:
        // 42 and 71. This drops the probability of buffer 
        // issues causing causing false positives.
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



