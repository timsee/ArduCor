
/*!
 * RGB-LED-Routines
 * Supports Adafruit NeoPixels products, the SeeedStudio Rainbowduino,
 * and a single RGB LED.
 * 
 * Provides a set of lighting routines and a serial interface. 
 * 
 * Version 1.0
 * Date: October 14, 2015
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#include <SoftwareSerial.h>
#include <Rainbowduino.h>
#include <Adafruit_NeoPixel.h>

//================================================================================
// Defines
//================================================================================
/*! 
 * INSTRUCTIONS FOR USE: 
 * 
 * To get the code working with your specific hardware, you'll need to set up a few defines.
 * 
 * 1. First, set the flags based on if you have a rainbowduinao, neopixel, or single RGB LED.  
 * 2. Set the LED_COUNT to the the number of LEDs you have.
 * 3. If you have a NeoPixel, set the CONTROL_PIN to the pin being used to control the board.
 * 4. If you have a single LED, set up its pins and whether its a common anode
 */

#define IS_RAINBOWDUINO  0
#define IS_NEOPIXEL      1
#define IS_SINGLE_LED    0

#define LED_COUNT        64 

#define CONTROL_PIN      6      // NeoPixels only, pin to control it

#define R_PIN            6     // SINGLE_LED only
#define G_PIN            5     // SINGLE_LED only
#define B_PIN            4     // SINGLE_LED only
#define IS_COMMON_ANODE  1     // SINGLE_LED only, 0 if common cathode, 1 if common anode

// Default settings
#define DEFAULT_SPEED    20     // default delay for LEDs update, suggested range: 1 (fast) - 1000 (slow)
#define DEFAULT_BRIGHT   50     // default brightness for LEDs, range: 0 - 100

// LED routine parameters
#define SOLID_FADE_SPEED 20     // change rate of solid fade routine, range 1 (slow) - 100 (fast)
#define ALL_FADE_SPEED   10     // change rate of all colors fade routine, range: 1 (slow) - 100 (fast)
#define GLIMMER_PERCENT  20     // percent of "glimmering" LEDs in glimmer routines: range: 0 - 100
#define BLINK_EVERY_X    3      // slow down framerate of blinking routines: range 1 - 100
#define OVERRIDE_LENGTH  15     // The number of frames a temporary color overrides current mode

//================================================================================
// Enumerated Types
//================================================================================

/*!
 * MOAR INSTRUCTIONS FOR USE: 
 * 
 * Routines for lights. If you want to add your own routine you'd go about it like this:
 * 1. add the type to this enum
 * 2. write a function for the routine
 * 3. add a case for the switch in currentLightingRoutine();
 */
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
   * preset color modes
   */
  eGreenGlimmer,
  eTealGlimmer,
  eBlueGlimmer,
  ePurpleGlimmer,
  eRedGlimmer,
  eOrangeGlimmer,
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
  eLightingMode_MAX //total number of modes
  
};


/*!
 * mesage headers for packets coming over serial.
 */
enum EPacketHeader 
{
  /*! 
   * takes one int parameter that gets cast to ELightingMode.
   */
  eModeChange,
  /*! 
   * takes three parameters, a 0-255 representation of Red, Green, and Blue.
   */
  eSolidColorChange,
  /*! 
   * takes one parameter, sets the brightness between 0 and 100.
   */
  eBrightnessChange,
  /*! 
   * takes one parameter, sets the delay value 1 - 23767.
   */
  eSpeedChange,
  /*! 
   * takes 3 parameter, a 0-255 representation of Red, Green, and Blue.
   * Holds that color temporarily for the number of franmes defined 
   * in OVERRIDE_LENGTH
   */
  eOverrideColor,
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

// percent of LED strength used
byte bright_level = DEFAULT_BRIGHT;

// contrary to popular belief, light_speed != MC^2. Instead, it is the delay
// between updates of the LEDs. Calculate it by DEFAULT_SPEED * 10msec
int light_speed = DEFAULT_SPEED;  

//=======================
// Stored Values and States
//=======================

// buffers for the individual LEDs channels.
byte r_buffer[LED_COUNT];
byte g_buffer[LED_COUNT];
byte b_buffer[LED_COUNT];

// used to store a color value
struct Color 
{
  byte r;
  byte g;
  byte b;
};

//Stored colors used by light routines
Color current_color =   {40,  0, 200};
Color fade_color    =   {255, 0, 0};
Color temporary_color = {0,   0, 0};
// variables for tracking state of modes
boolean should_blink = true;
// controls direction of fade 
boolean fade_up = true;
// stored value for fade
byte solid_fade_value = 0;
// checks for override
boolean has_override = false; 


// counters for routines
int loop_counter  = 0;
int fade_counter  = 0;
int blink_counter = 0;
int override_counter = 0;

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

// NeoPixels controller object
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_COUNT, CONTROL_PIN, NEO_GRB + NEO_KHZ800);

// prototype function, needed for compiler to recognize the enum.
void currentLightingRoutine(ELightingMode currentMode);

//================================================================================
// Setup and Loop
//================================================================================

void setup() 
{ 
  if (IS_RAINBOWDUINO) Rb.init(); 
  if (IS_NEOPIXEL)     pixels.begin(); 
  if (IS_SINGLE_LED)
  {
    pinMode(R_PIN, OUTPUT);
    pinMode(G_PIN, OUTPUT);
    pinMode(B_PIN, OUTPUT); 
  }
    
  Serial.begin(19200);
} 

void loop() 
{  
  serialLogic(); 
  
  if (!(loop_counter % light_speed))
  {
    // check for temporary override
    if (!checkForOverride())
    {
      // if no override, display current mode
      currentLightingRoutine(current_mode); 
    }
    updateLEDs();
  }

  loop_counter++;
  delay(10); 
} 

//================================================================================
// Serial Logic and Packet Parsing
//================================================================================

/*!
 * Checks if Serial has incoming messages and takes the most recent one. 
 * In this application, the Serial messages are expected to be delimited strings.
 * This string is converted to an int array and sent through a parser. 
 * 
 * The delimiter for the string is commas, and the endline is a semi-colon.
 */
void serialLogic()
{
  if (Serial.available())
  {
    String currentPacket = Serial.readStringUntil(';');
    parsed_packet = delimitedStringToIntArray(currentPacket);
    // exit if no strings detected
    if (parsed_packet.count == 0) return;
    // pase a paceket only if its header is is in the correct range
    if(parsed_packet.values[0] < ePacketHeader_MAX)
    {
      parsePacket(parsed_packet.values[0]);
    }
  }
}

/*!
 * This parser looks at the header of a control packet and from that determines
 * which parameters to use and what settings to change. 
 */
void parsePacket(int header)
{
  // in each case, theres a final check that the packet was properly
  // formatted by making sure its getting the right number of values.
  boolean success = false;
  switch (header) 
  {
    case eModeChange:
      if (parsed_packet.count == 2 && parsed_packet.values[1] < eLightingMode_MAX)
      {
        success = true;
        // change mode to new mode
        current_mode = (ELightingMode)parsed_packet.values[1];
        resetModeSpecificSettings();
      }
      break;
    case eSolidColorChange:
      if (parsed_packet.count == 4)
      {
        success = true;
        current_color.r = parsed_packet.values[1];
        current_color.g = parsed_packet.values[2];
        current_color.b = parsed_packet.values[3];
        current_mode = eSolidConstant; 
      }
      break;
    case eBrightnessChange:
    {
      if (parsed_packet.count == 2)
      {
        success = true;
        int param = constrain(parsed_packet.values[1], 0, 100);
        // update brightness level
        bright_level = param;
      }
      break;
    }
    case eSpeedChange:
      if (parsed_packet.count == 2)
      {
        success = true;
        // update light speed
        light_speed = parsed_packet.values[1]; 
      }
      break;
    case eOverrideColor:
    {
      if (parsed_packet.count == 4)
      {
        success = true;
        // double check the floor and ceiling before mapping
        temporary_color.r = constrain(parsed_packet.values[1], 0, 255);
        temporary_color.g = constrain(parsed_packet.values[2], 0, 255);
        temporary_color.b = constrain(parsed_packet.values[3], 0, 255);
        has_override = true;
        override_counter = 0;
      }
      break;
    }
    default: 
      return;
  }
  if (success)
  {
    //reset to 0 to draw to screen right away
    loop_counter = 0;  
  }
}

/*!
 * Helper used when changing modes to make sure certain modes
 * do not get out of sync. 
 */
void resetModeSpecificSettings()
{
  if (current_mode == eSolidBlink)
  {
    blink_counter = 0;
    should_blink = true;
  }
  if (current_mode == eRandomSolid)
  {
    blink_counter = 0;
  } 
}

//================================================================================
// Mode Management
//================================================================================

/*!
 * Checks for a recent override packet
 */
boolean checkForOverride()
{
  if (has_override)
  {
    solidColorGlimmer(temporary_color.r, 
                      temporary_color.g, 
                      temporary_color.b, 
                      GLIMMER_PERCENT);
     override_counter++;
     if (override_counter == OVERRIDE_LENGTH)
     {
       has_override = false;
       override_counter = 0;
     }
     return true;
  }
  return false; 
}

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
      solidColor(0,0,0);
      break;

    case eSolidConstant:
      solidColor(current_color.r, current_color.g, current_color.b);
      break;

    case eSolidBlink:
      solidBlink(current_color.r, current_color.g, current_color.b);
      break;
      
    case eSolidFade:
      solidFade(current_color.r, current_color.g, current_color.b, SOLID_FADE_SPEED);
      break;
      
    case eSolidGlimmer:
      solidColorGlimmer(current_color.r, current_color.g, current_color.b, GLIMMER_PERCENT);
      break;
      
    case eGreenGlimmer:
      solidColorGlimmer(0, 255, 0, GLIMMER_PERCENT); 
      break;
      
    case eTealGlimmer:
      solidColorGlimmer(0, 255, 255, GLIMMER_PERCENT); 
      break;
      
    case eBlueGlimmer:
      solidColorGlimmer(0, 0, 255, GLIMMER_PERCENT); 
      break;
      
     case ePurpleGlimmer:
      solidColorGlimmer(255, 0, 255, GLIMMER_PERCENT); 
      break;
      
    case eRedGlimmer:
      solidColorGlimmer(255, 0, 0, GLIMMER_PERCENT); 
      break;
      
    case eOrangeGlimmer:
      solidColorGlimmer(255, 127, 0, GLIMMER_PERCENT); 
      break;
      
    case eWhiteGlimmer:
      solidColorGlimmer(255, 255, 255, GLIMMER_PERCENT); 
      break;
      
    case eRandomIndividual:
      randomColorsIndividual();
      break;

    case eRandomSolid:
      randomColorSolid();
      break;
     
    case eFadeSolidColors:
      colorCycle();
      break; 
      
    default:
      solidColor(0,0,0);
      break;
  }
}


//================================================================================
// Lighting Routines
//================================================================================


/*!
 * Set every LED to the provided color.
 * 
 * @param red strength of red LED, between 0 and 255
 * @param green strength of green LED, between 0 and 255
 * @param blue strength of blue LED, between 0 and 255
 */
void solidColor(byte red, byte green, byte blue)
{
  for(int x = 0; x < LED_COUNT; x++)
  {
    r_buffer[x] = red;
    g_buffer[x] = green;
    b_buffer[x] = blue;
  }
}

/*!
 * Switches between OFF state and using the provided color on every frame.
 * 
 * @param red strength of red LED, between 0 and 255
 * @param green strength of green LED, between 0 and 255
 * @param blue strength of blue LED, between 0 and 255
 */
void solidBlink(byte red, byte green, byte blue)
{
  if (!(blink_counter % BLINK_EVERY_X))
  {
    if (should_blink)
    {
      for(int x = 0; x < LED_COUNT; x++)
      {
        r_buffer[x] = red;
        g_buffer[x] = green;
        b_buffer[x] = blue;
      }
    }
    else
    {
      for(int x = 0; x < LED_COUNT; x++)
      {
        r_buffer[x] = 0;
        g_buffer[x] = 0;
        b_buffer[x] = 0;
      }
    }
    should_blink = !should_blink;
  }
  blink_counter++;
}


/*!
 * Fades the LEDs on and off based on the provided color. 
 * Uses the parameter fadeSpeed to determine how fast to fade. A larger
 * number leads to a slower fade.
 * 
 * @param red strength of red LED, between 0 and 255
 * @param green strength of green LED, between 0 and 255
 * @param blue strength of blue LED, between 0 and 255
 * @param fadeSpeed how many ticks it takes to fade. Higher numbers are slower.           
 */
void solidFade(byte red, byte green, byte blue, byte fadeSpeed)
{
  // catch edge case
  if (fadeSpeed == 0) fadeSpeed = 1;
  
  // apply the fade
  if (fade_up)  solid_fade_value++;
  else          solid_fade_value--;
  
  // constrain the fade
  if (solid_fade_value == fadeSpeed) fade_up = false;
  else if (solid_fade_value == 0)    fade_up = true;
  
  // set up the buffers
  for(int x = 0; x < LED_COUNT; x++)
  {
    r_buffer[x] = (byte)(red   * (solid_fade_value / (float)fadeSpeed));
    g_buffer[x] = (byte)(green * (solid_fade_value / (float)fadeSpeed));
    b_buffer[x] = (byte)(blue  * (solid_fade_value / (float)fadeSpeed));
  }
}

/*!
 * Set every LED to the provided color. A subset of the LEDs
 * based on the percent parameter will be less bright than the 
 * rest of the LEDs. 
 * 
 * @param red strength of red LED, between 0 and 255
 * @param green strength of green LED, between 0 and 255
 * @param blue strength of blue LED, between 0 and 255
 * @param percent determines how many LEDs will be slightly dimmer than the rest
 */
void solidColorGlimmer(byte red, byte green, byte blue, long percent)
{
  for(int x = 0; x < LED_COUNT; x++)
  {
    // a random number is generated, if its less than the percent,
    // treat this as an LED that gets a glimmer effect
    if (random(1,100) < percent && percent != 0)
    {
      // chooses how much to divide the input by 
      byte scaleFactor = (byte)random(2,5);
      
      byte rScaled = red / scaleFactor;
      byte gScaled = green / scaleFactor;
      byte bScaled = blue / scaleFactor;
      
      r_buffer[x] = rScaled;
      g_buffer[x] = gScaled;
      b_buffer[x] = bScaled;
    }
    else
    {
      r_buffer[x] = red;
      g_buffer[x] = green;
      b_buffer[x] = blue;
    }  
  }
}

/*!
 * Each LED gets assigned a different random value for 
 * its R,G, and B components. This results in a lot of
 * pastel colored lights and an overall white glow.
 */
void randomColorsIndividual()
{
  for(int x = 0; x < LED_COUNT; x++)
  {
     r_buffer[x] = (byte)random(0, 255);
     g_buffer[x] = (byte)random(0, 255);
     b_buffer[x] = (byte)random(0, 255);
   }
}

/*!
 * A random color is chosen and applied to each LED.
 */
void randomColorSolid()
{
  if (!(blink_counter % BLINK_EVERY_X))
  {
    byte r = (byte)random(0, 255);
    byte g = (byte)random(0, 255);
    byte b = (byte)random(0, 255);
    for(int x = 0; x < LED_COUNT; x++)
    {
      r_buffer[x] = r;
      g_buffer[x] = g;
      b_buffer[x] = b;
    }
  }
  blink_counter++;
}

/*!
 * cycle through the colors using stored variables.
 */
void colorCycle()
{
  fadeColors();
  for(int x = 0; x < LED_COUNT; x++)
  {
    r_buffer[x] = applyBrightLevel(fade_color.r);
    g_buffer[x] = applyBrightLevel(fade_color.g);
    b_buffer[x] = applyBrightLevel(fade_color.b);
  }
}
   
/*!
 * logic of fade routine. Individually fades each channel of RGB LED
 * up then down. 
 */
void fadeColors()
{
  if (fade_counter < 255 / ALL_FADE_SPEED)
  {
    fade_color.g = fade_color.g + ALL_FADE_SPEED;
  }
  else if (fade_counter < 255 / ALL_FADE_SPEED * 2)
  {
    fade_color.r = fade_color.r - ALL_FADE_SPEED;
  }
  else if (fade_counter < 255 / ALL_FADE_SPEED * 3)
  {
    fade_color.b = fade_color.b + ALL_FADE_SPEED;
  }
  else if (fade_counter < 255 / ALL_FADE_SPEED * 4)
  {
    fade_color.g = fade_color.g - ALL_FADE_SPEED;
  }
  else if (fade_counter < 255 / ALL_FADE_SPEED * 5)
  {
    fade_color.r = fade_color.r + ALL_FADE_SPEED;
  }
  else if (fade_counter < 255 / ALL_FADE_SPEED * 6)
  {
    fade_color.b = fade_color.b - ALL_FADE_SPEED;
  }
  else
  {
    fade_counter = -1;
  }
  fade_counter++;
}

/*!
 * helper used to apply the brightness level to any value. 
 */
byte applyBrightLevel(byte value) { return (byte)(value * (bright_level / 100.0f)); }

//================================================================================
// Hardware Updates
//================================================================================

/*!
 * Takes the arrays of LED values and updates the LEDs in the way that the hardware
 * expects.
 */
void updateLEDs()
{
  if (IS_RAINBOWDUINO)
  {
    int index = 0;
    //Rainbowduino driver only has a setXY or setXYZ command
    for(int x = 0; x < 8; x++)
    {
      for(int y = 0; y < 8; y++) 
      {
        Rb.setPixelXY(x,y,
                      applyBrightLevel(r_buffer[index]), 
                      applyBrightLevel(g_buffer[index]), 
                      applyBrightLevel(b_buffer[index]));
        index++;
      }
    }
  }
  
  if (IS_NEOPIXEL)
  {
    for(int x = 0; x < LED_COUNT; x++) 
    {
      pixels.setPixelColor(x, pixels.Color(applyBrightLevel(r_buffer[x]),
                                           applyBrightLevel(g_buffer[x]),
                                           applyBrightLevel(b_buffer[x])));
    }
    // Neopixels use the show function to update the pixels
    pixels.show();
  }

  if (IS_SINGLE_LED)
  {
    if (IS_COMMON_ANODE)
    {
      analogWrite(R_PIN, 255 - applyBrightLevel(r_buffer[0]));
      analogWrite(G_PIN, 255 - applyBrightLevel(g_buffer[0]));
      analogWrite(B_PIN, 255 - applyBrightLevel(b_buffer[0]));
    }
    else
    {
      analogWrite(R_PIN, applyBrightLevel(r_buffer[0]));
      analogWrite(G_PIN, applyBrightLevel(g_buffer[0]));
      analogWrite(B_PIN, applyBrightLevel(b_buffer[0]));
    }
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
  if (checkIfValidString(message))
  {
    int packetSize = countDelimitedValues(message);
    int valueStartIndex = 0;
    int valueCounter = 0;
    int valueArray[packetSize];
    for (int i = 0; i < message.length(); i++)
    {
      if (message.charAt(i) == delimiter)
      {
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
  }
  else
  {
    int junkArray[0];
    ParsedIntPacket packet = {junkArray, 0};
    return packet;
  }
}

/*!
 * does a naive check on if the string is parseable. For now, it just
 * makes sure that every character is either a digit, a negative sign, 
 * or a delimiter.
 * 
 * @param message the current delimited string
 * 
 * @return true if it passes validity test, false otherwise
 */
boolean checkIfValidString(String message)
{
  for (int i = 0; i < message.length(); i++)
  {
    if (!(isDigit(message.charAt(i)) 
          || message.charAt(i) == delimiter 
          || message.charAt(i) == '-'))
    {
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
  for (int i = 0; i < message.length(); i++)
  {
    if (message.charAt(i) == delimiter) delimiters_count++;
  }

  if (delimiters_count == 0)
  {
    return 1;
  }
  else
  {
    return delimiters_count + 1; // first delimiter has two values
  } 
}



