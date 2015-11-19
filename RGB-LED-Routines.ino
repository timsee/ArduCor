
/*!
 * RGB-LED-Routines
 * Supports Adafruit NeoPixels products, the SeeedStudio Rainbowduino,
 * and a single RGB LED.
 * 
 * Provides a set of lighting routines and a serial interface. 
 * 
 * Version 1.5
 * Date: November 18, 2015
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

const int IS_RAINBOWDUINO   = 0;
const int IS_NEOPIXEL       = 1;
const int IS_SINGLE_LED     = 0;

const int LED_COUNT         = 64; 

const int CONTROL_PIN       = 6;     // NeoPixels only, pin to control it

const int R_PIN             = 5;     // SINGLE_LED only
const int G_PIN             = 4;     // SINGLE_LED only
const int B_PIN             = 3;     // SINGLE_LED only
const int IS_COMMON_ANODE   = 1;     // SINGLE_LED only, 0 if common cathode, 1 if common anode

const int NUM_OF_COLORS     = 5;      // Number of saved colors

// Default settings
const int DEFAULT_SPEED     = 40;     // default delay for LEDs update, suggested range: 1 (fast) - 1000 (slow)
const int DEFAULT_BRIGHT    = 50;     // default brightness for LEDs, range: 0 - 100

// LED routine parameters
const int SOLID_FADE_SPEED  = 20;     // change rate of solid fade routine, range 1 (slow) - 100 (fast)
const int ALL_FADE_SPEED    = 10;     // change rate of all colors fade routine, range: 1 (slow) - 100 (fast)
const int SAVED_FADE_SPEED  = 10;     // change rate of saved colors fade routine, range: 1 (slow) - 100 (fast)
const int GLIMMER_PERCENT   = 8;      // percent of "glimmering" LEDs in glimmer routines: range: 0 - 100
const int BLINK_EVERY_X     = 3;      // slow down framerate of blinking routines: range 1 - 100
const int OVERRIDE_LENGTH   = 15;     // The number of frames a temporary color overrides current mode
const int BAR_SIZE          = 4;      // The number of LEDs in groups in bar-based lighting schemes such as twoSolidBars
const int BARS_DEFAULT      = 2;      // Number of bars, should be less than NUM_OF_COLORS

const unsigned long TIMEOUT_DEFAULT = 120;  // number of minutes without packets until the arduino times out.

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
   * saved color modes
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
  /*! 
   * Takes 3 parameter, a 0-255 representation of Red, Green, and Blue.
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

// used to store a color value
struct Color 
{
  byte r;
  byte g;
  byte b;
};

// stored saved colors for routines
Color colors[NUM_OF_COLORS];

// buffers for the individual LEDs channels.
byte r_buffer[LED_COUNT];
byte g_buffer[LED_COUNT];
byte b_buffer[LED_COUNT];

// stores location for moving buffers
byte moving_buffer[LED_COUNT];

// colors used by specific routines
Color fade_color       = {255, 0,0};
Color temporary_color  = {0,   0,0};
Color prest_fade_color = {0,   0,0};
Color random_saved     = {0,   0,0};
Color fade_saved       = {0,   0,0};
Color goal_color       = {0,   0,0};

// settings
int num_of_bars = BARS_DEFAULT;
unsigned long idle_timeout = TIMEOUT_DEFAULT * 60 * 1000; // convert to milliseconds

// variables for tracking state of modes
boolean should_blink = true;
// controls direction of fade 
boolean fade_up = true;
// checks for override
boolean has_override = false; 
// used in two bar color schemes
boolean use_first_bar = false;
// used in moving two bar color scheme
boolean start_with_first_bar = true;
// looks for any values that are still fading
boolean fade_met = true;
// used at start of fade to switch colors
boolean start_next_fade = true;
// turn off the lights when idle
boolean idle_timeout_on = true;

// stored value for fade
byte solid_fade_value = 0;

// counters for routines
int loop_counter  = 0;
int fade_counter  = 0;
int blink_counter = 0;
int override_counter = 0;
int bar_counter = 0;
int moving_bar_counter = 0;
int saved_blink_counter = 0;
int fade_saved_counter = 0;

// indices
int bar_index = 0;
int moving_bar_index = 0;
int random_saved_index = 1;
int moving_start_index = 0;

// timeout variable
unsigned long last_message_time;

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
   
  if (IS_SINGLE_LED) {
    pinMode(R_PIN, OUTPUT);
    pinMode(G_PIN, OUTPUT);
    pinMode(B_PIN, OUTPUT); 
  }

  // setup the saved colors
  colors[0] =   {0,   255, 0};   // green
  colors[1] =   {125, 0,   255}; // teal
  colors[2] =   {0,   0,   255}; // blue
  colors[3] =   {40,  127, 40};  // light green
  colors[4] =   {60,  0,   160}; // purple
  
  // setup the timeout. 
  idle_timeout = TIMEOUT_DEFAULT * 60 * 1000; 
  last_message_time = 0;
  
  // setup the moving buffer
  movingBufferSetup(num_of_bars, BAR_SIZE);
  Serial.begin(19200);
} 

void loop() 
{  
  serialLogic(); 
  
  if (!(loop_counter % light_speed)) {
    // check for temporary override
    if (!checkForOverride()) {
      // if no override, display current mode
      currentLightingRoutine(current_mode); 
    }
    updateLEDs();
  }
  
  // Timeout the LEDs.
  if (idle_timeout_on && last_message_time + idle_timeout < millis()) {
    solidColor(0,0,0);
    current_mode = eLightsOff;
  }
  
  loop_counter++;
  delay(10); 
} 

//================================================================================
// Serial Logic and Packet Parsing
//================================================================================

/*!
 * Checks if Serial has incoming messages and takes the most recent one. 
 * In this sketch, the Serial messages are expected to be delimited strings.
 * This string is converted to an int array and sent through a parser. 
 * 
 * The delimiter for the string is commas, and the endline is a semi-colon.
 */
void serialLogic()
{
  if (Serial.available()) {
    String currentPacket = Serial.readStringUntil(';');
    parsed_packet = delimitedStringToIntArray(currentPacket);
    // exit if no strings detected
    if (parsed_packet.count == 0) return;
    // pase a paceket only if its header is is in the correct range
    if(parsed_packet.values[0] < ePacketHeader_MAX) {
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
        resetModeSpecificSettings();
      }
      // pick up cases where the modes can take extra optional arguments
      if (parsed_packet.count == 3 
          && (parsed_packet.values[1] == eSavedBarSolid
          || parsed_packet.values[1] == eSavedBarMoving)) {
              
        if (parsed_packet.values[1] == eSavedBarSolid) {
          success = true;
          current_mode = (ELightingMode)parsed_packet.values[1];
          // set the number of bars used by the routine
          num_of_bars = parsed_packet.values[2]; 
          movingBufferSetup(num_of_bars, BAR_SIZE);
        } else if (parsed_packet.values[1] == eSavedBarMoving) {
          success = true;
          current_mode = (ELightingMode)parsed_packet.values[1];
          // set the number of bars used by the routine
          num_of_bars = parsed_packet.values[2]; 
          savedBarMoving(num_of_bars, BAR_SIZE);
          movingBufferSetup(num_of_bars, BAR_SIZE);
        }
      }
      break;
    case eColorChange:
      if (parsed_packet.count == 5) {
        int color_index = parsed_packet.values[1] - 1;
        if (color_index >= 0 && color_index < eLightingMode_MAX) {
          success = true;
          colors[color_index].r = parsed_packet.values[2];
          colors[color_index].g = parsed_packet.values[3];
          colors[color_index].b = parsed_packet.values[4];
        }
      }
      break;
    case eBrightnessChange:
    {
      if (parsed_packet.count == 2) {
        success = true;
        int param = constrain(parsed_packet.values[1], 0, 100);
        // update brightness level
        bright_level = param;
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
    case eOverrideColor:
    {
      if (parsed_packet.count == 4) {
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
  if (success) {
    last_message_time = millis();
    // Reset to 0 to draw to screen right away.
    loop_counter = 0;  
  }
}

/*!
 * Helper used when changing modes to make sure certain modes
 * do not get out of sync. 
 */
void resetModeSpecificSettings()
{
  if (current_mode == eSolidBlink) {
    blink_counter = 0;
    should_blink = true;
  }
  if (current_mode == eRandomSolid) {
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
  if (has_override) {
    solidColorGlimmer(temporary_color.r, 
                      temporary_color.g, 
                      temporary_color.b, 
                      GLIMMER_PERCENT);
     override_counter++;
     if (override_counter == OVERRIDE_LENGTH) {
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
      solidColor(colors[0].r, colors[0].g, colors[0].b);
      break;

    case eSolidBlink:
      solidBlink(colors[0].r, colors[0].g, colors[0].b);
      break;
      
    case eSolidFade:
      solidFade(colors[0].r, colors[0].g, colors[0].b, SOLID_FADE_SPEED);
      break;
      
    case eSolidGlimmer:
      solidColorGlimmer(colors[0].r, colors[0].g, colors[0].b, GLIMMER_PERCENT);
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
      fadeAllColors();
      break; 

    case eSavedGlimmer:
      savedGlimmer(GLIMMER_PERCENT);
      break; 
      
    case eSavedRandomSolid:
      savedRandomSolid();
      break;
      
    case eSavedRandomIndividual:
      savedRandomIndividual();
      break; 
    
    case eSavedFade:
      savedFade();
      break;
      
    case eSavedBarSolid:
      savedBarSolid(num_of_bars, BAR_SIZE);
      break; 

    case eSavedBarMoving:
      savedBarMoving(num_of_bars, BAR_SIZE);
      break;
      
    default:
      solidColorGlimmer(0, 255, 0, GLIMMER_PERCENT); 
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
  for(int x = 0; x < LED_COUNT; x++) {
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
  if (!(blink_counter % BLINK_EVERY_X)) {
    if (should_blink) {
      for(int x = 0; x < LED_COUNT; x++) {
        r_buffer[x] = red;
        g_buffer[x] = green;
        b_buffer[x] = blue;
      }
    } else {
      for(int x = 0; x < LED_COUNT; x++) {
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
  for(int x = 0; x < LED_COUNT; x++) {
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
  for(int x = 0; x < LED_COUNT; x++) {
    // a random number is generated. If its less than the percent,
    // treat this as an LED that gets a glimmer effect
    if (random(1,100) < percent && percent != 0) {
      // set a random level for the LED to be dimmed by.
      byte scaleFactor = (byte)random(2,5);
      
      byte rScaled = red / scaleFactor;
      byte gScaled = green / scaleFactor;
      byte bScaled = blue / scaleFactor;
      
      r_buffer[x] = rScaled;
      g_buffer[x] = gScaled;
      b_buffer[x] = bScaled;
    } else {
      r_buffer[x] = red;
      g_buffer[x] = green;
      b_buffer[x] = blue;
    }  
  }
}


/*!
 * Takes the color[0] from saved colors and sets that as the standard color. 
 * Then, takes the GLIMMER_PERCENT, and randomly chooses a different
 * saved to glimmer with. Also, glimmers the standard way with 
 * random LEDs being set to dimmer values. 
 */
void savedGlimmer(long percent)
{
  for(int x = 0; x < LED_COUNT; x++) {
    random_saved = colors[0];
    if (random(1,100) < percent && percent != 0) {
      setSavedRandom(); 
    }
   
    // a random number is generated, if its less than the percent,
    // treat this as an LED that gets a glimmer effect
    if (random(1,100) < percent && percent != 0) {
      // chooses how much to divide the input by 
      byte scaleFactor = (byte)random(2,5);
      
      byte rScaled = random_saved.r / scaleFactor;
      byte gScaled = random_saved.g / scaleFactor;
      byte bScaled = random_saved.b / scaleFactor;
      
      r_buffer[x] = rScaled;
      g_buffer[x] = gScaled;
      b_buffer[x] = bScaled;
    } else {
      r_buffer[x] = random_saved.r;
      g_buffer[x] = random_saved.g;
      b_buffer[x] = random_saved.b;
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
  for(int x = 0; x < LED_COUNT; x++) {
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
  if (!(blink_counter % BLINK_EVERY_X)) {
    byte r = (byte)random(0, 255);
    byte g = (byte)random(0, 255);
    byte b = (byte)random(0, 255);
    for(int x = 0; x < LED_COUNT; x++) {
      r_buffer[x] = r;
      g_buffer[x] = g;
      b_buffer[x] = b;
    }
  }
  blink_counter++;
}

/*!
 * sets each individual LED as a random color
 * based off of the set of saved colors.
 */
void savedRandomIndividual()
{
  for(int x = 0; x < LED_COUNT; x++) {
    setSavedRandom();
    r_buffer[x] = random_saved.r;
    g_buffer[x] = random_saved.g;
    b_buffer[x] = random_saved.b;
  }
}

/*!
 * A random color is chosen from the saved colors
 * and applied to each LED.
 */
void savedRandomSolid()
{
  if (!(blink_counter % BLINK_EVERY_X)) {
    setSavedRandom();
    for(int x = 0; x < LED_COUNT; x++) {
      r_buffer[x] = random_saved.r;
      g_buffer[x] = random_saved.g;
      b_buffer[x] = random_saved.b;
    }
  }
  blink_counter++;
}

/*!
 * Fade through all the colors in the rainbow!
 */
void fadeAllColors()
{
  fadeAllColorsIncrement();
  for(int x = 0; x < LED_COUNT; x++) {
    r_buffer[x] = fade_color.r;
    g_buffer[x] = fade_color.g;
    b_buffer[x] = fade_color.b;
  }
}


/*! 
 * Sets two colors alternating in patches the size of barSize.
 * 
 * @param colorCount number of colors used in bars, starts with lowest saved
 * @param barSize how many LEDs before switching to the other bar.         
 * 
 */
void savedBarSolid(int colorCount, byte barSize)
{
  bar_counter = 0;
  bar_index = 0;
  // set up the buffers
  for(int x = 0; x < LED_COUNT; x++) {
    r_buffer[x] = colors[bar_index].r;
    g_buffer[x] = colors[bar_index].g;
    b_buffer[x] = colors[bar_index].b; 
    bar_counter++;
    if (bar_counter == barSize) {
      bar_counter = 0;
      bar_index = (bar_index + 1) % colorCount;
    }
  }
}


/*! 
 * Sets two colors alternating in patches the size of barSize.
 * and moves them up in index on each frame.
 * 
 * @param colorCount how many color saved colors to use.
 * @param barSize how many LEDs before switching to the other bar.         
 */
void movingBufferSetup(int colorCount, byte barSize)
{
  int index = 0;
  int counter = 0;
  for (int x = 0; x < LED_COUNT; x++) {
     moving_buffer[x] = index;
     counter++;
     if (counter == barSize) {
      counter = 0;
      index++;
      if (index == colorCount) {
        index = 0;
      }
     }
  }
}


/*!
 * Moves groups of LEDs set to the same color (bars) from the
 * set of saved colors.
 */
void savedBarMoving(int colorCount, byte barSize)
{
  for(int x = 0; x < LED_COUNT - moving_start_index; x++) {
    r_buffer[x] = colors[moving_buffer[x + moving_start_index]].r;
    g_buffer[x] = colors[moving_buffer[x + moving_start_index]].g;
    b_buffer[x] = colors[moving_buffer[x + moving_start_index]].b; 
  }
  int z = 0;
  for (int x = LED_COUNT - moving_start_index; x < LED_COUNT; x++) {
    r_buffer[x] = colors[moving_buffer[z]].r;
    g_buffer[x] = colors[moving_buffer[z]].g;
    b_buffer[x] = colors[moving_buffer[z]].b;
    z++;
  }
  moving_start_index = (moving_start_index + 1) % LED_COUNT;
}


/*!
 * Chooses a random different color from the set of saved colors.
 */
void setSavedRandom()
{ 
    int possible_saved = random(0, NUM_OF_COLORS);
    while (possible_saved == random_saved_index) {
      possible_saved = random(0, NUM_OF_COLORS); 
    }
    random_saved_index = possible_saved;
    random_saved = colors[random_saved_index];
}

/*!
 * Logic of the fade routine. Individually fades each channel of RGB LED
 * up then down. 
 */
void fadeAllColorsIncrement()
{
  if (fade_counter < 255 / ALL_FADE_SPEED) {
    fade_color.g = fade_color.g + ALL_FADE_SPEED;
  } else if (fade_counter < 255 / ALL_FADE_SPEED * 2) {
    fade_color.r = fade_color.r - ALL_FADE_SPEED;
  } else if (fade_counter < 255 / ALL_FADE_SPEED * 3) {
    fade_color.b = fade_color.b + ALL_FADE_SPEED;
  } else if (fade_counter < 255 / ALL_FADE_SPEED * 4) {
    fade_color.g = fade_color.g - ALL_FADE_SPEED;
  } else if (fade_counter < 255 / ALL_FADE_SPEED * 5) {
    fade_color.r = fade_color.r + ALL_FADE_SPEED;
  } else if (fade_counter < 255 / ALL_FADE_SPEED * 6) {
    fade_color.b = fade_color.b - ALL_FADE_SPEED;
  } else {
    fade_counter = -1;
  }
  fade_counter++;
}


/*!
 * Fades between all saved colors.
 */
void savedFade()
{
  if (start_next_fade) {
    start_next_fade = false;
    fade_saved_counter = (fade_saved_counter + 1) % NUM_OF_COLORS;
    fade_saved = colors[fade_saved_counter];
    goal_color = colors[(fade_saved_counter + 1) % NUM_OF_COLORS];
  }

  fade_met = true;
  fade_saved.r = fadeBetweenValues(fade_saved.r, goal_color.r);
  fade_saved.g = fadeBetweenValues(fade_saved.g, goal_color.g);
  fade_saved.b = fadeBetweenValues(fade_saved.b, goal_color.b);
  start_next_fade = fade_met;

  for(int x = 0; x < LED_COUNT; x++) {
    r_buffer[x] = fade_saved.r;
    g_buffer[x] = fade_saved.g;
    b_buffer[x] = fade_saved.b;
  }
}

/*!
 * Helper for fading between two predetermined channels
 * of an overall color for saved fade routines.
 */
int fadeBetweenValues(int fadeChannel, int destinationChannel)
{
  if (fadeChannel < destinationChannel) {
    int difference = destinationChannel - fadeChannel;
    if (difference < SAVED_FADE_SPEED) {
      fadeChannel = destinationChannel;
    } else {
      fadeChannel = fadeChannel + SAVED_FADE_SPEED;
      fade_met = false;
    }
  }
  else if (fadeChannel > destinationChannel) {
    int difference = fadeChannel - destinationChannel;
    if (difference < SAVED_FADE_SPEED) {
      fadeChannel = destinationChannel;
    } else {
      fadeChannel = fadeChannel - SAVED_FADE_SPEED;
      fade_met = false;
    }
  }
  return fadeChannel;
}


/*!
 * Helper used to apply the brightness level to any value. 
 */
byte applyBrightLevel(byte value) { return (byte)round((value * (bright_level / 100.0f))); }

//================================================================================
// Hardware Updates
//================================================================================

/*!
 * Takes the arrays of LED values and updates the LEDs in the way that the hardware
 * expects.
 */
void updateLEDs()
{
  if (IS_RAINBOWDUINO) {
    int index = 0;
    //Rainbowduino driver only has a setXY or setXYZ command
    for(int x = 0; x < 8; x++) {
      for(int y = 0; y < 8; y++) {
        Rb.setPixelXY(x,y,
                      applyBrightLevel(r_buffer[index]), 
                      applyBrightLevel(g_buffer[index]), 
                      applyBrightLevel(b_buffer[index]));
        index++;
      }
    }
  }
  
  if (IS_NEOPIXEL) {
    for(int x = 0; x < LED_COUNT; x++) {
      pixels.setPixelColor(x, pixels.Color(applyBrightLevel(r_buffer[x]),
                                           applyBrightLevel(g_buffer[x]),
                                           applyBrightLevel(b_buffer[x])));
    }
    // Neopixels use the show function to update the pixels
    pixels.show();
  }

  if (IS_SINGLE_LED) {
    if (IS_COMMON_ANODE) {
      analogWrite(R_PIN, 255 - applyBrightLevel(r_buffer[0]));
      analogWrite(G_PIN, 255 - applyBrightLevel(g_buffer[0]));
      analogWrite(B_PIN, 255 - applyBrightLevel(b_buffer[0]));
    } else {
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





