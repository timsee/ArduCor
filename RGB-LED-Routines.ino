
/*!
 * RGB-LED-Routines
 * Sample Sketch
 * 
 * Supports Adafruit NeoPixels products, the SeeedStudio Rainbowduino,
 * and a single RGB LED.
 * 
 * Provides a serial interface to a set of lighting routines generated
 * by the RoutinesRGB library. 
 * 
 * Version 1.7
 * Date: December 6, 2015
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include <Rainbowduino.h>
#include <RoutinesRGB.h>

//================================================================================
// Settings
//================================================================================

const int IS_RAINBOWDUINO   = 0;
const int IS_NEOPIXEL       = 1;
const int IS_SINGLE_LED     = 0;

const int CONTROL_PIN       = 6;     // NeoPixels only, pin used by NeoPixels library

const int R_PIN             = 5;     // SINGLE_LED only
const int G_PIN             = 4;     // SINGLE_LED only
const int B_PIN             = 3;     // SINGLE_LED only
const int IS_COMMON_ANODE   = 1;     // SINGLE_LED only, 0 if common cathode, 1 if common anode

const int LED_COUNT         = 64; 
const int COLOR_COUNT       = 5;      // Number of saved colors

const int DEFAULT_SPEED     = 40;     // default delay for LEDs update, suggested range: 1 (fast) - 1000 (slow)
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
   * Does color[0] as main color and uses other saved colors 
   * as glimmer.
   */
  eSavedGlimmer,
  /*!
   * Uses the saved colors and randomly lights up to them.
   */
  eSavedRandomIndividual,
  /*!
   * Uses the saved colors and randomly lights up to them.
   */
  eSavedRandomSolid,
  /*!
   * Uses the saved colors and fades between them.
   */
  eSavedFade,
  /*!
   * Colors alternating in groups of equal size
   */
  eSavedBarSolid,
  /*!
   * Colors alternating in groups of equal size and moving up in indices
   */
  eSavedBarMoving,
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
   * Takes four parameters, three parameters, the LED, a 0-255 representation of Red, Green, and Blue.
   */
  eColorChange,
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
ELightingMode current_mode = eGreenGlimmer; 

// contrary to popular belief, light_speed != MC^2. Instead, it is the delay
// between updates of the LEDs. Calculate it by DEFAULT_SPEED * 10msec
int light_speed = DEFAULT_SPEED;  
int bar_size    = DEFAULT_BAR_SIZE;

// sets for saved color routines
int saved_bar_count               = DEFAULT_BAR_COUNT;
int saved_glimmer_count           = COLOR_COUNT;
int saved_random_solid_count      = COLOR_COUNT;
int saved_random_individual_count = COLOR_COUNT;
int saved_fade_count              = COLOR_COUNT;

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

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_COUNT, CONTROL_PIN, NEO_GRB + NEO_KHZ800);

//================================================================================
// Setup and Loop
//================================================================================

void setup() 
{
  if (IS_RAINBOWDUINO) Rb.init(); 
  if (IS_NEOPIXEL)     pixels.begin();
   
  if (IS_SINGLE_LED) {
    pinMode(R_PIN, OUTPUT);
    pinMode(G_PIN, OUTPUT);
    pinMode(B_PIN, OUTPUT); 
  }

  // setup the timeout. 
  idle_timeout = DEFAULT_TIMEOUT * 60 * 1000; 
  last_message_time = 0;

  routines.setColor(0, 0,   255, 0);   // green
  routines.setColor(1, 125, 0,   255); // teal
  routines.setColor(2, 0,   0,   255); // blue
  routines.setColor(3, 40,  127, 40);  // light green
  routines.setColor(4, 60,  0,   160); // purple 

  // put your setup code here, to run once:
  Serial.begin(19200);
}

void loop() 
{
  if (Serial.available()) {
    String currentPacket = Serial.readStringUntil(';');
    parsed_packet = delimitedStringToIntArray(currentPacket);
    // parse a paceket only if its header is is in the correct range
    if(parsed_packet.count > 0 && parsed_packet.values[0] < ePacketHeader_MAX) {
      parsePacket(parsed_packet.values[0]);
    }
  }

  if (!(loop_counter % light_speed)) {
    currentLightingRoutine(current_mode);
    updateLEDs();
  }
  
  // Timeout the LEDs.
  if (idle_timeout_on && last_message_time + idle_timeout < millis()) {
    routines.solid(0,0,0);
    current_mode = eLightsOff;
  }
  
  loop_counter++;
  delay(10);
}


void updateLEDs()
{
  if (IS_RAINBOWDUINO) {
    int index = 0;
    for(int x = 0; x < 8; x++) {
      for(int y = 0; y < 8; y++)  {
        Rb.setPixelXY(x,y,
                      routines.getR(index), 
                      routines.getG(index), 
                      routines.getB(index));
        index++;
      }
    }
  }
  
  if (IS_NEOPIXEL)
  {
    for(int x = 0; x < LED_COUNT; x++) {
     pixels.setPixelColor(x, pixels.Color(routines.getR(x),
                                          routines.getG(x),
                                          routines.getB(x)));
    }
    pixels.show();
  }

  if (IS_SINGLE_LED) {
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
void currentLightingRoutine(ELightingMode currentMode)
{
  switch(currentMode)
  {
    case eLightsOff:
      routines.solid(0,0,0);
      break;

    case eSolidConstant:
      routines.solid(routines.getColor(0).r, routines.getColor(0).g, routines.getColor(0).b);
      break;

    case eSolidBlink:
      routines.blink(routines.getColor(0).r, routines.getColor(0).g, routines.getColor(0).b);
      break;
      
    case eSolidFade:
      routines.fade(routines.getColor(0).r, routines.getColor(0).g, routines.getColor(0).b, FADE_SPEED);
      break;
      
    case eSolidGlimmer:
      routines.glimmer(routines.getColor(0).r, routines.getColor(0).g, routines.getColor(0).b, GLIMMER_PERCENT);
      break;
      
    case eRedGlimmer:
      routines.glimmer(255, 0, 0, GLIMMER_PERCENT); 
      break;
      
    case eOrangeGlimmer:
      routines.glimmer(255, 60, 0, GLIMMER_PERCENT); 
      break;

    case eYellowGlimmer:
      routines.glimmer(255, 255, 0, GLIMMER_PERCENT); 
      break;
      
    case eGreenGlimmer:
      routines.glimmer(0, 255, 0, GLIMMER_PERCENT); 
      break;
      
    case eTealGlimmer:
      routines.glimmer(0, 255, 80, GLIMMER_PERCENT); 
      break;
      
    case eBlueGlimmer:
      routines.glimmer(0, 0, 255, GLIMMER_PERCENT); 
      break;
      
    case ePurpleGlimmer:
      routines.glimmer(60, 0, 160, GLIMMER_PERCENT); 
      break;

    case eLightBlueGlimmer:
      routines.glimmer(80, 80, 255, GLIMMER_PERCENT); 
      break; 

    case ePinkGlimmer:
      routines.glimmer(255, 80, 80, GLIMMER_PERCENT); 
      break;
      
    case eWhiteGlimmer:
      routines.glimmer(255, 255, 255, GLIMMER_PERCENT); 
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

    case eSavedGlimmer:
      routines.savedGlimmer(saved_glimmer_count, GLIMMER_PERCENT);
      break; 
      
    case eSavedRandomSolid:
      routines.savedRandomSolid(saved_random_solid_count);
      break;
      
    case eSavedRandomIndividual:
      routines.savedRandomIndividual(saved_random_individual_count);
      break; 
    
    case eSavedFade:
      routines.savedFade(saved_fade_count);
      break;
      
    case eSavedBarSolid:
      routines.savedBarSolid(saved_bar_count, bar_size);
      break; 

    case eSavedBarMoving:
      routines.savedBarMoving(saved_bar_count, bar_size);
      break;
      
    default:
      routines.glimmer(0, 255, 0, GLIMMER_PERCENT); 
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
        success = true;
        // change mode to new mode
        current_mode = (ELightingMode)parsed_packet.values[1];
      }
      // pick up cases where the modes can take extra optional arguments
      if (parsed_packet.count == 3) {
        if (parsed_packet.values[1] == eSavedGlimmer) {
          success = true;
          current_mode = (ELightingMode)parsed_packet.values[1];
          saved_glimmer_count = parsed_packet.values[2];          
        } else if (parsed_packet.values[1] == eSavedRandomIndividual) {
          success = true;
          current_mode = (ELightingMode)parsed_packet.values[1];
          saved_random_individual_count = parsed_packet.values[2]; 
        } else if (parsed_packet.values[1] == eSavedRandomSolid) {
          success = true;
          current_mode = (ELightingMode)parsed_packet.values[1];
          saved_random_solid_count = parsed_packet.values[2]; 
        } else if (parsed_packet.values[1] == eSavedFade) {
          success = true;
          current_mode = (ELightingMode)parsed_packet.values[1];
          saved_fade_count = parsed_packet.values[2]; 
        } else if (parsed_packet.values[1] == eSavedBarSolid) {
          success = true;
          current_mode = (ELightingMode)parsed_packet.values[1];
          // set the number of bars used by the routine
          saved_bar_count = parsed_packet.values[2]; 
        } else if (parsed_packet.values[1] == eSavedBarMoving) {
          success = true;
          current_mode = (ELightingMode)parsed_packet.values[1];
          // set the number of bars used by the routine
          saved_bar_count = parsed_packet.values[2]; 
        }
      }
      break;
    case eColorChange:
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
        success = true;
        int param = constrain(parsed_packet.values[1], 0, 100);
        // update brightness level
        routines.setBrightness(param);
      }
      break;
    }
    case eSpeedChange:
      if (parsed_packet.count == 2) {
        success = true;
        // update light speed
        light_speed = parsed_packet.values[1]; 
      }
      break;
    case eIdleTimeoutChange:
      if (parsed_packet.count == 2) {
        success = true;
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
    // Reset to 0 to draw to screen right away.
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
    ParsedIntPacket packet = {valueArray, packetSize};
    return packet;
  } else {
    int junkArray[0];
    ParsedIntPacket packet = {junkArray, 0};
    return packet;
  }
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



