
#ifndef RoutinesRGB_h
#define RoutinesRGB_h

#include "Arduino.h"

/*!
 * @version v1.8.3
 * @date March 27, 2016
 * @author Tim Seemann
 * @copyright <a href="https://github.com/timsee/RGB-LED-Routines/blob/master/LICENSE">
 *            MIT License
 *            </a>
 *
 *
 * @brief An Arduino library that provides a set of RGB lighting routines for compatible LED array hardware.
 *
 * @details This library has been tested with SeeedStudio Rainbowduinos, quite a few of the
 *  Adafruit Neopixels products, and a standard RGB LED. Sample code is provided in the git repo for all
 * tested hardware in the samples folder of the git repository.  
 *
 * If you are starting a project from scratch, first you'll need to make a global object in the
 * arduino sketch:
 *
 * ~~~~~~~~~~~~~~~~~~~~~
 * RoutinesRGB routines = RoutinesRGB(LED_COUNT, COLOR_COUNT);
 * ~~~~~~~~~~~~~~~~~~~~~
 *
 * where `LED_COUNT` is the number of LEDs in your array, and `COLOR_COUNT` is the maximum number of
 * colors you want to use in the array color routines.
 *
 * After setting up the global object, it will be showing a solid green color with a glimmer by default. To
 * update the colors, first call the proper functions to change it to the mode you want. For instance,
 * to update to a red blinking light, call this function:
 *  
 * ~~~~~~~~~~~~~~~~~~~~~
 * routines.blink(255, 0, 0);
 * ~~~~~~~~~~~~~~~~~~~~~
 *
 * Then, update the LED array with the values from the library's RGB buffer. The way to do this will
 * vary from hardware to hardware, but for a NeoPixels sample, it would look something like this:
 *
 * ~~~~~~~~~~~~~~~~~~~~~
 * for (int x = 0; x < LED_COUNT; x++) {
 *    pixels.setPixelColor(x, pixels.Color(routines.getR(x),
 *                                         routines.getG(x),
 *                                         routines.getB(x)));
 * }
 * pixels.show();
 * ~~~~~~~~~~~~~~~~~~~~~
 *
 * Some routines, such as the random solid and all of the fades, change their values overtime.
 * For these, put the routine's API call and the hardware update in your `loop()` and use your loop's
 * update speed to determime how fast the LEDs change.
 *
 */
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
     * Required constructor. This library can support as many LEDs and colors
     * as your arduino's memory can support. The library should be stored in 
     * global memory and allocated only once at startup.
     * 
     * It will allocate `(4 * ledCount) + (3 * colorCount)` bytes.
     *
     * @param ledCount number of individual RGB LEDs. 
     * @param number of colors that are allocated for the array.
     */
    RoutinesRGB(uint16_t ledCount, uint16_t colorCount);

  
//================================================================================
// Getters and Setters
//================================================================================
    /*! @defgroup gettersetters Getters and Setters
     *  These are the getters and setters for RoutinesRGB that are used to control
     *  the settings and the colors.
     *  @{
     */
    
    
    /*!
     * Sets the color used for single color routines.
     */
     void setMainColor(byte r, byte g, byte b);
     
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
     * Sets how many updates to wait before changing the light state in the blink
     * routine and in routines that switch between solid colors.
     *
     * @param blinkSpeed a value between 1 and 255 representing how fast to blink.
     *        A value of 1 will make it blink on every frame, which may be too fast
     *        when used with other routines.
     */
    void setBlinkSpeed(uint8_t blinkSpeed);
    
    /*!
     * Retrieve the main color, which is used for single color routines.
     */
    Color getMainColor();
    
    /*!
     * Retrieve the color at the given index.
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
       
    /*! @} */
//================================================================================
// Single Color Routines
//================================================================================
    /*! @defgroup singleRoutines Single Color Routines
     *  These routines each take a R, G, and B value as parameters to generate
     *  a color. This color is the only color used by the routine. 
     *
     *  Blink, fade, and glimmer, should be called repeatedly on a loop for their full effect.
     *  The speed of the loop determines how fast the LEDs update.
     *  @{
     */

    
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
     * Fades the LEDs on and off based on the provided color. 
     * Uses the parameter fadeSpeed to determine how fast to fade. A larger
     * number leads to a slower fade.
     * 
     * @param red strength of red LED, between 0 and 255
     * @param green strength of green LED, between 0 and 255
     * @param blue strength of blue LED, between 0 and 255
     * @param fadeSpeed how many ticks it takes to fade. Higher numbers are slower.           
     */
    void fade(uint8_t red, uint8_t green, uint8_t blue, uint8_t fadeSpeed, boolean shouldUpdate);
    
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
    void glimmer(uint8_t red, uint8_t green, uint8_t blue, long percent, boolean shouldUpdate);
    
    /*! @} */
//================================================================================
// Multi Color Routines
//================================================================================
    /*! @defgroup multiRoutines Multi Color Routines
     *  These routines use their own set of predefined colors and require no additional parameters.
     *
     *  Blink, fade, and glimmer, should be called repeatedly on a loop for their full effect.
     *  The speed of the loop determines how fast the LEDs update.
     *  @{
     */
    
    
    /*!
     * A random color is chosen and applied to each LED.
     */
    void randomSolid();
    
    /*!
     * Each LED gets assigned a different random value for 
     * its R,G, and B components. This results in a lot of
     * pastel colored lights and an overall white glow.
     */
    void randomIndividual();
    
    /*!
     * Fade through all the colors in the rainbow!
     */
    void fadeBetweenAllColors();

    /*! @} */
//================================================================================
// Routines With The Color Array
//================================================================================
    /*! @defgroup arrayRoutines Array Colors Routines
     *  These routines use the `colors` array as a basis for their colors.
     *  They all take the parameter of `colorCount` which is used to determine how many
     *  colors to use.
     *
     *  Blink, fade, and glimmer, should be called repeatedly on a loop for their full effect.
     *  The speed of the loop determines how fast the LEDs update.
     *  @{
     */
    
    
    /*!
     * This method uses its percent parameter to dim LEDs randomly, similar to the 
     * standard glimmer mode. It also uses the percent to randomly change the color
     * of select LEDs to a color in the `colors` array. The base color is `color[0]` 
     * from the colors array.
     *
     * @param colorCount the number of colors in the array used for the routine. 
     * @param percent percent of LEDs that will get the glimmer applied
     */
    void arrayGlimmer(uint16_t colorCount, long percent);
     
    /*!
     * sets each individual LED as a random color from the color array.
     *
     * @param colorCount the number of colors in the array used for the routine. 
     */
    void arrayRandomIndividual(uint16_t colorCount);
    
    /*!
     * A random color is chosen from the color array and applied to each LED.
     *
     * @param colorCount the number of colors in the array used for the routine. 
     */
    void arrayRandomSolid(uint16_t colorCount);
    
    /*!
     * Fades between the number of colors in the array.
     *
     * @param colorCount the number of colors in the array used for the routine. 
     */
    void arrayFade(uint16_t colorCount);

    /*! 
     * Uses the color array to set the LEDs in alternating patches with a size of barSize.
     * 
     * @param colorCount the number of colors in the array used for the routine. 
     * @param barSize how many LEDs before switching to the other bar.         
     * 
     */
    void arrayBarSolid(uint16_t colorCount, byte barSize);
    
    /*!
     * Provides a similar effect as arrayBarSolid, but the alternating patches 
     * move up one LED index on each frame update to create a "scrolling" effect.
     *     
     * @param colorCount the number of colors in the array used for the routine. 
     * @param barSize how many LEDs before switching to the other bar.
     */
    void arrayBarMoving(uint16_t colorCount, byte barSize);     
    
    /*! @} */ 
private:

    // array of colors
    Color* colors;
    // used for single color routines
    Color m_main_color;
    
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
    Color    m_temp_color;
    
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
    uint16_t m_fade_array_counter;  

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
     * @param colorCount the number of colors in the array used for the routine. 
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
     * of an overall color for array fade routines.
     */
    uint16_t fadeBetweenValues(uint16_t fadeChannel, uint16_t destinationChannel);
    
    /*!
     * Chooses a random different color from the array of colors.
     */
    void chooseRandomFromArray(uint16_t maxColorIndex, boolean canRepeat); 
};

#endif