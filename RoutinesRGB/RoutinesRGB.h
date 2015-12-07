/*!
 * RoutinesRGB
 * Arduino library that provides a set of lighting routines that can be used
 * with various RGB LED arduino products such as a Rainbowduino by SeedStudio 
 * or Neopixels by Adafruit.  
 * 
 * Version 1.7
 * Date: December 6, 2015
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */
 

#ifndef RoutinesRGB_h
#define RoutinesRGB_h

#include "Arduino.h"

class RoutinesRGB
{
  public:
  
//================================================================================
// Structs and Constructors
//================================================================================

    // used to store a color value
    struct Color 
    {
     uint8_t r;
     uint8_t g;
     uint8_t b;
    };
    
    /*!
     * Required constructor. This library support as many LEDs and colors
     * as your arduino's memory can support. The library should be stored in 
     * global memory and allocated only once at startup.
     * 
     * It will allocate (4 * ledCount) + (3 * colorCount) bytes. 
     *
     * @param ledCount number of individual RGB LEDs. 
     * @param number of colors that can be "saved".
     */
	RoutinesRGB(uint16_t ledCount, uint16_t colorCount);

  
//================================================================================
// Getters and Setters
//================================================================================
  
    /*!
     * Set the color at a given index with the RGB values provided. colorIndex must
     * be less than the colorCount provided to the constructor or else it will not 
     * have any effect. 
     */
    void setColor(uint16_t colorIndex, byte r, byte g, byte b);
    
    /*!
     * Set the brightness between 0 and 100. 0 is off, 100 is full power.
     */
    void setBrightness(uint8_t brightness);
            
    /*!
     * Sets the speed of routines that fade between colors between 1 and 100. 
     * A fade speed of 1 is the slowest possible fade. 
     */    
    void setFadeSpeed(uint8_t fadeSpeed);
    
    /*!
     * sets how many updates to wait before changing the light state in the blink
     * routine and in routines that switch between solid colors. Takes a value between
     * 1 and 255. A value of 1 will make it blink on every frame, which may be too fast
     * when used with other routines.
     */
    void setBlinkSpeed(uint8_t blinkSpeed);
    
    /*!
     * Retrieve the color at the given number.
     */
    Color getColor(uint16_t i);
    
    /*!
     * Retrieve the r value at a given index in the buffer.
     */
    uint8_t getR(uint16_t i);
   
    /*!
     * Retrieve the g value at a given index in the buffer.
     */
    uint8_t getG(uint16_t i);
    
    /*!
     * Retrieve the b value at a given index in the buffer.
     */
    uint8_t getB(uint16_t i);
       
  
//================================================================================
// Single Color Routines
//================================================================================

    /*!
     * Set every LED to the provided color.
     * 
     * @param red strength of red LED, between 0 and 255
     * @param green strength of green LED, between 0 and 255
     * @param blue strength of blue LED, between 0 and 255
     */
    void solid(uint8_t red, uint8_t green, uint8_t blue);
    
    /*!
     * Switches between ON and OFF states using the provided color.
     * 
     * @param red strength of red LED, between 0 and 255
     * @param green strength of green LED, between 0 and 255
     * @param blue strength of blue LED, between 0 and 255
     */
    void blink(uint8_t red, uint8_t green, uint8_t blue);
    
    /*!
     * Fades the LEDs on and off based on the provided color.  Uses the parameter 
     * fadeSpeed to determine how fast to fade. A larger number leads to a slower fade.
     * 
     * @param red strength of red LED, between 0 and 255
     * @param green strength of green LED, between 0 and 255
     * @param blue strength of blue LED, between 0 and 255
     * @param fadeSpeed how many ticks it takes to fade. Higher numbers are slower.           
     */
    void fade(uint8_t red, uint8_t green, uint8_t blue, uint8_t fadeSpeed);
    
    /*!
     * Set every LED to the provided color. A subset of the LEDs based on the percent 
     * parameter will be less bright than the rest of the LEDs. 
     * 
     * @param red strength of red LED, between 0 and 255
     * @param green strength of green LED, between 0 and 255
     * @param blue strength of blue LED, between 0 and 255
     * @param percent determines how many LEDs will be slightly dimmer than the rest
     */
    void glimmer(uint8_t red, uint8_t green, uint8_t blue, long percent);
    
    
//================================================================================
// Multi Color Routines
//================================================================================
    
    /*!
     * A random color is chosen and applied to each LED.
     */
    void randomSolid();
    
    /*!
     * Each LED gets assigned a different random value for its R,G, and B components. 
     * This results in a lot of pastel colored lights and an overall white glow.
     */
    void randomIndividual();
    
    /*!
     * Fade through all the colors in the rainbow!
     */
    void fadeBetweenAllColors();


//================================================================================
// Routines With Saved Colors
//================================================================================

    /*!
     * Takes the color[0] from saved colors and sets that as the standard color. 
     * Then, takes the GLIMMER_PERCENT, and randomly chooses a different
     * saved to glimmer with. Also, glimmers the standard way with 
     * random LEDs being set to dimmer values. 
     *
     * @param colorCount the number of saved colors used for the routine. 
     * @param percent percent of LEDs that will get the glimmer applied
     */
    void savedGlimmer(uint16_t colorCount, long percent);
     
    /*!
     * sets each individual LED as a random color based off of the set of saved colors.
     *
     * @param colorCount the number of saved colors used for the routine. 
     */
    void savedRandomIndividual(uint16_t colorCount);
    
    /*!
     * A random color is chosen from the saved colors and applied to each LED.
	 *
     * @param colorCount the number of saved colors used for the routine.
     */
    void savedRandomSolid(uint16_t colorCount);
    
    /*!
     * Fades between all saved colors.
     *
     * @param colorCount the number of saved colors used for the routine.
     */
    void savedFade(uint16_t colorCount);

    /*! 
     * Sets saved colors in alternating patches with a size of barSize.
     * 
     * @param colorCount number of colors used in bars, starts with lowest saved
     * @param barSize how many LEDs before switching to the other bar.         
     * 
     */
    void savedBarSolid(uint16_t colorCount, byte barSize);
    
    /*!
     * Provides a similar effect as savedBarSolid, but the alternating patches 
     * move up one LED index on each frame update to create a "scrolling" effect.
     *     
	 * @param colorCount number of colors used in bars, starts with lowest saved
     * @param barSize how many LEDs before switching to the other bar.
     */
    void savedBarMoving(uint16_t colorCount, byte barSize);     
    

private:

	// array of saved colors
    Color* colors;
    
    // buffers used for storing the RGB LED values
    uint8_t* r_buffer;
    uint8_t* g_buffer;
    uint8_t* b_buffer;

    // states
    uint16_t m_LED_count;
    uint16_t m_color_count;
    uint16_t m_current_state;
    uint16_t m_bar_count;
    uint16_t m_bar_size;
    uint8_t m_bright_level;
    uint8_t m_fade_speed;
    uint8_t m_blink_speed;
    
    // temp values
    uint8_t* m_temp_buffer;
    uint16_t m_temp_counter;
    uint16_t m_temp_index;
	boolean  m_temp_bool;
	Color 	 m_temp_color;
	
    // colors used by specific routines
    Color m_fade_color;
    Color m_goal_color;

    // used at start of fade to switch colors
    boolean m_start_next_fade;
    // turn off the lights when idle
    boolean m_idle_timeout_on;
    // stored value for fade
    uint8_t m_solid_fade_value;
  
    // fades save their place between modes so they use their own counters
    uint16_t m_fade_counter;
    uint16_t m_fade_saved_counter;  

    /*!
     * Called before every function. Used to update the library state tracking
     * and to reset any necessary variables when a state changes. 
     */
    void preProcess(uint16_t state);
    
    /*!
     * Called after every function that uses brightness and other post-processing.
     */
    void postProcess();
     
    /*!
     * Helper used to apply the brightness level to any value.
     */
    uint8_t applyBrightness(byte value);
    
    /*!
     * Sets two colors alternating in patches the size of barSize.
     * and moves them up in index on each frame.
     *
     * @param colorCount how many color saved colors to use.
     * @param barSize how many LEDs before switching to the other bar.
     */
    void movingBufferSetup(uint16_t colorCount, byte barSize);
    
    /*!
     * Logic of the fade routine. Individually fades each channel of RGB LED
     * up then down. 
     */
    void fadeAllColorsIncrement();
    
    /*!
     * Helper for fading between two predetermined channels
     * of an overall color for saved fade routines.
     */
    uint16_t fadeBetweenValues(uint16_t fadeChannel, uint16_t destinationChannel);
    
    /*!
     * Chooses a random different color from the set of saved colors.
     */
    void chooseRandomSaved(uint16_t maxColorIndex, boolean canRepeat); 
};

#endif
