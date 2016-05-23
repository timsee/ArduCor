/*!
 * \version v1.9.5
 * \date May 22, 2016
 * \author Tim Seemann
 * \copyright <a href="https://github.com/timsee/RGB-LED-Routines/blob/master/LICENSE">
 *            MIT License
 *            </a>
 *
 *
 * \brief An Arduino library that provides a set of RGB lighting routines for compatible
 * LED array hardware.
 *
 */
 
#include "RoutinesRGB.h"
#include "ColorPresets.h"

// Default brightness of LEDS, must be a value between 50 and 100.
const uint8_t  DEFAULT_BRIGHTNESS  = 50;
// used for determining how fast the fade routines fade. LEDs will change
// this value per fade frame, so a smaller value slows down fades.
const uint8_t  DEFAULT_FADE_SPEED  = 10;
// used for determining how fast blink routines are. This is how many
// frames it waits until switching the LED states from on or off. 
// a lower number speeds up the blink.
const uint8_t  DEFAULT_BLINK_SPEED = 3;
// default value that determines how many colors a custom color routine should use.
// This value must be less than the size of the custom color array. 
const uint8_t  DEFAULT_CUSTOM_COUNT = 2;

//================================================================================
// Constructors
//================================================================================

RoutinesRGB::RoutinesRGB(uint16_t ledCount)
{
    m_LED_count = ledCount;
    // catch an illegal argument
    if (m_LED_count == 0) {
        m_LED_count = 1;
    }
    
    // allocate the arrays not known at runtime. 
    if((r_buffer = (uint8_t*)malloc(ledCount))) {
        memset(r_buffer, 0, ledCount);
    }

    if((g_buffer = (uint8_t*)malloc(ledCount))) {
        memset(g_buffer, 0, ledCount);
    } 

    if((b_buffer = (uint8_t*)malloc(ledCount))) {
        memset(b_buffer, 0, ledCount);
    }
    
    if((m_temp_buffer = (uint8_t*)malloc(ledCount))) {
        memset(m_temp_buffer, 0, ledCount);
    }
    
    // all colors gets set before use since it changes each times
    resetToDefaults(); 
}

void RoutinesRGB::resetToDefaults()
{
    // By default, this is set to orange. However,
    // most sample sketches will override this value
    // during their setup. 
    m_main_color = {100, 25, 0};
    
    // set user configurable settings
    m_current_group = eCustom;
    m_current_routine = eSingleGlimmer;
    
    m_bright_level = DEFAULT_BRIGHTNESS;
    m_fade_speed   = DEFAULT_FADE_SPEED;
    m_blink_speed  = DEFAULT_BLINK_SPEED;
    m_custom_count = DEFAULT_CUSTOM_COUNT;
    m_bar_size = 0;
    
    // set temp values
    m_temp_index = 0;
    m_temp_counter = 0;
    m_temp_bool = true;
    m_temp_color = {0, 0, 0};
    
    // set routine specific variables
    m_goal_color = {0, 0, 0};
    m_start_next_fade = true;

    
    // set custom colors to default colors
    for (int i = 0; i < 10; i = i + 5) {
        m_custom_colors[i]     = {0,   255, 0};     // green
        m_custom_colors[i + 1] = {125, 0,   255};   // teal
        m_custom_colors[i + 2] = {0,   0,   255};   // blue
        m_custom_colors[i + 3] = {40,  127, 40};    // light green
        m_custom_colors[i + 4] = {60,  0,   160};   // purple
    }  
}

//================================================================================
// Getters and Setters
//================================================================================

void
RoutinesRGB::setMainColor(uint8_t r, uint8_t g, uint8_t b)
{
    m_main_color = {r, g, b};
}

void
RoutinesRGB::setColor(uint16_t colorIndex, uint8_t r, uint8_t g, uint8_t b)
{
    if (colorIndex < (sizeof(m_custom_colors) / sizeof(Color))) {
        m_custom_colors[colorIndex] = {r, g, b};
    }
}


void 
RoutinesRGB::setCustomColorCount(uint8_t count)
{
    if (count != 0) {
        m_custom_count = count;
        // catch edge case that preprocess isn't well suited to catch. 
        if (m_current_group == eCustom) {
            m_preprocess_flag = true;
        }
    }
}

uint8_t 
RoutinesRGB::customColorCount()
{
    return m_custom_count;
}

void 
RoutinesRGB::brightness(uint8_t brightness)
{
    if (brightness <= 100) {
        m_bright_level = brightness;
    }
}


void 
RoutinesRGB::fadeSpeed(uint8_t fadeSpeed)
{
    if (fadeSpeed != 0) {
        m_fade_speed = fadeSpeed;
    }
}


void 
RoutinesRGB::blinkSpeed(uint8_t blinkSpeed)
{
    if (blinkSpeed != 0) {
        m_blink_speed = blinkSpeed;
    }
}


RoutinesRGB::Color
RoutinesRGB::mainColor()
{  
    return m_main_color;
}


RoutinesRGB::Color
RoutinesRGB::color(uint16_t i)
{
    if (i < (sizeof(m_custom_colors) / sizeof(Color))) {
        return m_custom_colors[i];
    } else {
        return (Color){0,0,0};
    }
}


uint8_t
RoutinesRGB::red(uint16_t i)
{
    if (i < m_LED_count) {
        return r_buffer[i];
    } else {
        return 0;
    }
}

uint8_t
RoutinesRGB::green(uint16_t i)
{
    if (i < m_LED_count) {
        return g_buffer[i];
    } else {
        return 0;
    }
}

uint8_t
RoutinesRGB::blue(uint16_t i)
{
    if (i < m_LED_count) {
        return b_buffer[i];
    } else {
        return 0;
    }
}


  
//================================================================================
// Pre Processing
//================================================================================
 
void
RoutinesRGB::preProcess(ELightingRoutine newRoutine, EColorGroup newGroup)
{    
    // prevent illegal values
    if (newGroup >= eColorGroup_MAX) {
        newGroup = (EColorGroup)((int)eColorGroup_MAX - 1);
    }
    if (newGroup < 0) {
        newGroup = (EColorGroup)0;
    }
    
    //---------
    // State Has Changed
    //---------
    if ((m_current_routine != newRoutine)
        || m_preprocess_flag) {
        // reset the temps
        m_temp_index = 0;
        m_temp_counter = 0;
        m_temp_bool = true;
        m_temp_color = {0,0,0};
        
        // always reset blink
        if (m_current_routine == eSingleBlink) {
            m_temp_counter = 0;
            m_temp_bool = true;
        }
        m_preprocess_flag = true;
        m_current_routine = newRoutine;
    }
    
    //---------
    // Preset Has Changed
    //---------
    if ((m_current_group != newGroup) 
        || m_preprocess_flag) {
        
        // reset flag
        m_preprocess_flag = false;
        // reset the temps
        m_temp_index = 0;
        m_temp_counter = 0;
        m_temp_bool = true;
        m_temp_color = {0,0,0};      
        
        // reset fades, even when only the colorGroup changes
        if (m_current_routine == eMultiFade) {
            m_start_next_fade = true;
            m_temp_counter = 0;
        }
        
        setupColorGroup(newGroup);
        
        // setup the buffer to do a moving array.
        if (newRoutine == eMultiBarsMoving
            || newRoutine == eMultiBarsSolid) {
            m_temp_index = 0;
            movingBufferSetup(m_temp_size, m_bar_size);
        }
        
        m_current_group = newGroup;
    }
}


void 
RoutinesRGB::setupColorGroup(EColorGroup colorGroup)
{
    // Set up the m_temp_array used for the multi color routines
    // This is done by copying the relevant colors into the 
    // m_temp_array and storing the number of colors in m_temp_size.
    if (colorGroup == eCustom) {
        m_temp_size = m_custom_count;
        memcpy(m_temp_array, m_custom_colors, sizeof(m_custom_colors));
    } else if (colorGroup == eAll) {
        // create a random color for every color in the temp array.
        m_temp_size = (sizeof(m_temp_array) / sizeof(Color));
        for (int i = 0 ; i < (sizeof(m_temp_array) / sizeof(Color)); i++) {
            m_temp_array[i] = { (uint8_t)random(0, 256),
                                (uint8_t)random(0, 256),
                                (uint8_t)random(0, 256)};
        }
    } else {
        // For our PROGMEM we aimed to have as small of footprint as possible.
        // We currently store a 2D array of color colorGroups, and another array of
        // the sizes of those colorGroups groups. First we grab the size, then we copy
        // the buffer directly from the 2D array.
        m_temp_size = pgm_read_word_near(presetSizes + colorGroup - 1);
        memcpy_P(m_temp_array, 
                (void*)pgm_read_word_near(colorPresets + colorGroup - 1), 
                (m_temp_size * 3));
    } 
}

//================================================================================
// Post Processing
//================================================================================

void
RoutinesRGB::postProcess()
{
    // loop again to apply global effects
    for(uint16_t x = 0; x < m_LED_count; x++) {
        // apply brightness
        r_buffer[x] = applyBrightness(r_buffer[x]);
        g_buffer[x] = applyBrightness(g_buffer[x]);
        b_buffer[x] = applyBrightness(b_buffer[x]);
    }
}


uint8_t
RoutinesRGB::applyBrightness(uint8_t value)
{
    return (uint8_t)round((value * (m_bright_level / 100.0f)));
}


//================================================================================
// Single Color Routines
//================================================================================


void
RoutinesRGB::singleSolid(uint8_t red, uint8_t green, uint8_t blue)
{
    preProcess(eSingleSolid, m_current_group);
    for (uint16_t x = 0; x < m_LED_count; x++) {
        r_buffer[x] = red;
        g_buffer[x] = green;
        b_buffer[x] = blue;
    }
    postProcess();
}


void
RoutinesRGB::singleBlink(uint8_t red, uint8_t green, uint8_t blue)
{
    preProcess(eSingleBlink, m_current_group);
    // switches states between on/off based off of m_blink_speed
    if (!(m_temp_counter % m_blink_speed)) {
        if (m_temp_bool) {
            for (uint16_t x = 0; x < m_LED_count; x++) {
                r_buffer[x] = red;
                g_buffer[x] = green;
                b_buffer[x] = blue;
            }
        } else {
            for (uint16_t x = 0; x < m_LED_count; x++) {
                r_buffer[x] = 0;
                g_buffer[x] = 0;
                b_buffer[x] = 0;
            }
        }
        m_temp_bool = !m_temp_bool;
        postProcess();
    }
    m_temp_counter++;
}



void
RoutinesRGB::singleFade(uint8_t red, uint8_t green, uint8_t blue, uint8_t fadeSpeed, boolean shouldUpdate)
{
    preProcess(eSingleFade, m_current_group);
    if (shouldUpdate) {
        // catch illegal argument and fix it
        if (fadeSpeed == 0) fadeSpeed = 1;
    
        // apply the fade
        if (m_temp_bool)  m_temp_counter++;
        else              m_temp_counter--;
    
        // constrain the fade
        if (m_temp_counter == fadeSpeed) m_temp_bool = false;
        else if (m_temp_counter == 0)    m_temp_bool = true;
    }
    
    // set up the buffers
    for (int x = 0; x < m_LED_count; x++) {
        r_buffer[x] = (uint8_t)(red   * (m_temp_counter / (float)fadeSpeed));
        g_buffer[x] = (uint8_t)(green * (m_temp_counter / (float)fadeSpeed));
        b_buffer[x] = (uint8_t)(blue  * (m_temp_counter / (float)fadeSpeed));
    }
    postProcess();
}



void
RoutinesRGB::singleGlimmer(uint8_t red, uint8_t green, uint8_t blue, long percent, boolean shouldUpdate)
{
    preProcess(eSingleGlimmer, m_current_group);
    for (uint16_t x = 0; x < m_LED_count; x++) {
        // a random number is generated. If its less than the percent,
        // treat this as an LED that gets a glimmer effect
        if (random(1,101) < percent && percent != 0 && shouldUpdate) {
            // set a random level for the LED to be dimmed by.
            byte scaleFactor = (byte)random(2,6);
            
            r_buffer[x] = m_temp_color.red / scaleFactor;
            g_buffer[x] = m_temp_color.green / scaleFactor;
            b_buffer[x] = m_temp_color.blue / scaleFactor;
        } else {
            r_buffer[x] = red;
            g_buffer[x] = green;
            b_buffer[x] = blue;
        }  
    }
    postProcess();
}


//================================================================================
// Multi Color Routines
//================================================================================

void
RoutinesRGB::multiGlimmer(EColorGroup colorGroup, long percent)
{
    preProcess(eMultiGlimmer, colorGroup);
    
    for (uint16_t x = 0; x < m_LED_count; x++) {
        m_temp_color = m_temp_array[0];
        if (random(1,101) < percent && percent != 0) {
            // m_temp_color is set in chooseRandomFromArray
            chooseRandomFromArray(m_temp_array, m_temp_size, true);
        }

        // a random number is generated, if its less than the percent,
        // treat this as an LED that gets a glimmer effect
        if (random(1,101) < percent && percent != 0) {
            // chooses how much to divide the input by
            byte scaleFactor = (byte)random(2,6);
            r_buffer[x] = m_temp_color.red / scaleFactor;
            g_buffer[x] = m_temp_color.green / scaleFactor;
            b_buffer[x] = m_temp_color.blue / scaleFactor;
        } else {
            r_buffer[x] = m_temp_color.red;
            g_buffer[x] = m_temp_color.green;
            b_buffer[x] = m_temp_color.blue;
        }
    }
    postProcess();
}


void
RoutinesRGB::multiFade(EColorGroup colorGroup)
{   
    preProcess(eMultiFade, colorGroup);  
    // checks if it should change the colors it is fading between.
    if (m_start_next_fade) {
        m_start_next_fade = false;
        if (m_temp_size > 1) {
            m_temp_counter = (m_temp_counter + 1) % m_temp_size;
            m_temp_color = m_temp_array[m_temp_counter];
            m_goal_color = m_temp_array[(m_temp_counter + 1) % m_temp_size];
        } else {
            m_temp_counter = 0;
            m_goal_color = m_temp_array[0];
            m_temp_color = m_temp_array[0];
        }
    }
    // fades between two colors
    m_temp_bool = true;
    m_temp_color.red = fadeBetweenValues(m_temp_color.red, m_goal_color.red);
    m_temp_color.green = fadeBetweenValues(m_temp_color.green, m_goal_color.green);
    m_temp_color.blue = fadeBetweenValues(m_temp_color.blue, m_goal_color.blue);
    m_start_next_fade = m_temp_bool;

    // draws to buffer
    for (uint16_t x = 0; x < m_LED_count; x++) {
        r_buffer[x] = m_temp_color.red;
        g_buffer[x] = m_temp_color.green;
        b_buffer[x] = m_temp_color.blue;
    }
    postProcess();
}


void
RoutinesRGB::multiRandomSolid(EColorGroup colorGroup)
{
    preProcess(eMultiRandomSolid, colorGroup); 
    if (!(m_temp_counter % m_blink_speed)) {
        switch ((EColorGroup)colorGroup) {
            case eAll:
                // uses a random color instead of the m_temp_array buffer. 
                m_temp_color = {(uint8_t)random(0, 256),
                                (uint8_t)random(0, 256),
                                (uint8_t)random(0, 256)};
                for (uint16_t x = 0; x < m_LED_count; x++) {
                    r_buffer[x] = m_temp_color.red;
                    g_buffer[x] = m_temp_color.green;
                    b_buffer[x] = m_temp_color.blue;
                }
                break;
            default:
            
                chooseRandomFromArray(m_temp_array, m_temp_size, true);
                for (uint16_t x = 0; x < m_LED_count; x++) {
                    r_buffer[x] = m_temp_color.red;
                    g_buffer[x] = m_temp_color.green;
                    b_buffer[x] = m_temp_color.blue;
                }
                break;
            }
         postProcess();
    }   
    m_temp_counter++;
}

void
RoutinesRGB::multiRandomIndividual(EColorGroup colorGroup)
{   
    preProcess(eMultiRandomIndividual, colorGroup);  

    switch ((EColorGroup)colorGroup) {
        case eAll:
            // uses random values for each individual LED
            // instead of the m_temp_array buffer. 
            for (uint16_t x = 0; x < m_LED_count; x++) {
                r_buffer[x] = (uint8_t)random(0, 256);
                g_buffer[x] = (uint8_t)random(0, 256);
                b_buffer[x] = (uint8_t)random(0, 256);
            }
            break;
        default:
            for (uint16_t x = 0; x < m_LED_count; x++) {
                // chooses a random color from m_temp_array
                chooseRandomFromArray(m_temp_array, m_temp_size, true);
                // draws the random color to the buffer.
                r_buffer[x] = m_temp_color.red;
                g_buffer[x] = m_temp_color.green;
                b_buffer[x] = m_temp_color.blue;
            }
            break;
    }
    postProcess();
}



void
RoutinesRGB::multiBarsSolid(EColorGroup colorGroup, uint8_t barSize)
{   
    m_bar_size =  barSize;
    preProcess(eMultiBarsSolid, colorGroup);  
     
    m_temp_counter = 0;
    m_temp_index = 0;
    for(uint16_t x = 0; x < m_LED_count; x++) {
        r_buffer[x] = m_temp_array[m_temp_index].red;
        g_buffer[x] = m_temp_array[m_temp_index].green;
        b_buffer[x] = m_temp_array[m_temp_index].blue;
        m_temp_counter++;
        if (m_temp_counter == barSize) {
            m_temp_counter = 0;
            m_temp_index = (m_temp_index + 1) % m_temp_size;
        }
    }
    postProcess();
}


void
RoutinesRGB::multiBarsMoving(EColorGroup colorGroup, uint8_t barSize)
{   
    m_bar_size =  barSize;
    preProcess(eMultiBarsMoving, colorGroup);
    
    uint16_t repeat_index = 0;
    // loop through all the values between 0 and m_loop_index m_loop_count times.
    for (uint16_t x = 0; x < (m_loop_count * m_loop_index); x++) {
        // if we are computing values above m_LED_count, ignore.
        if (x < m_LED_count) {
            // m_temp_counter holds the index in this instance of a repeat through
            // the looped values.
            m_temp_counter = (repeat_index + m_temp_index) % m_loop_index;
            r_buffer[x] = m_temp_array[m_temp_buffer[m_temp_counter]].red;
            g_buffer[x] = m_temp_array[m_temp_buffer[m_temp_counter]].green;
            b_buffer[x] = m_temp_array[m_temp_buffer[m_temp_counter]].blue;
            // if a loop is pushing a repeat_index over m_loop_index, go back to 0
            repeat_index = (x + 1) % m_loop_index;
        }
    }
    m_temp_index = (m_temp_index + 1) % m_loop_index;
    postProcess();
}


//================================================================================
// Helper Functions
//================================================================================


uint16_t
RoutinesRGB::fadeBetweenValues(uint16_t fadeChannel, uint16_t destinationChannel)
{
    if (fadeChannel < destinationChannel) {
        uint16_t difference = destinationChannel - fadeChannel;
        if (difference < m_fade_speed) {
            fadeChannel = destinationChannel;
        } else {
            fadeChannel = fadeChannel + m_fade_speed;
            m_temp_bool = false;
        }
    }
    else if (fadeChannel > destinationChannel) {
        uint16_t difference = fadeChannel - destinationChannel;
        if (difference < m_fade_speed) {
            fadeChannel = destinationChannel;
        } else {
            fadeChannel = fadeChannel - m_fade_speed;
            m_temp_bool = false;
        }
    }
    return fadeChannel;
}

void
RoutinesRGB::movingBufferSetup(uint16_t colorCount, uint8_t barSize)
{
    if ((barSize * colorCount) > m_LED_count) {
        // edge case handled for memory reasons, a full loop must
        // take less than the m_LED_count 
        barSize = 1;
    }
    // minimum number of values needed for a looping pattern.
    m_loop_index = barSize * colorCount;
    // minimum number of times we need to loop these values to 
    // completely fill the LEDs.
    m_loop_count = ((m_LED_count / m_loop_index) + 1); 
    
    //the buffer from 0 to m_loop_index with the proper bars
    for (uint16_t x = 0; x < m_loop_index; x++) {
        m_temp_buffer[x] = m_temp_index;
        m_temp_counter++;
        if (m_temp_counter == barSize) {
            m_temp_counter = 0;
            m_temp_index++;
            if (m_temp_index == colorCount) {
                m_temp_index = 0;
            }
        }
    }
}

void
RoutinesRGB::chooseRandomFromArray(Color* array, uint8_t max_index, boolean canRepeat)
{   
    uint16_t possible_array_color = random(0, max_index);
    if (!canRepeat && max_index > 2) {
      while (possible_array_color == m_temp_index) {
         possible_array_color = random(0, max_index);
      }
    }
    m_temp_index = possible_array_color;
    m_temp_color = array[m_temp_index];
}

