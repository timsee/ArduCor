/*!
 * \version v2.9.0
 * \date March 2, 2018
 * \author Tim Seemann
 * \copyright <a href="https://github.com/timsee/ArduCor/blob/master/LICENSE">
 *            MIT License
 *            </a>
 *
 *
 * \brief An Arduino library that provides a set of RGB lighting routines for compatible
 * LED array hardware.
 *
 */
 
#include "ArduCor.h"
#include "ColorPresets.h"

// Default brightness of LEDS, must be a value between 0 and 100.
const uint8_t  DEFAULT_BRIGHTNESS  = 50;
// used for determining how fast the fade routines fade. A smaller
// value leads to quicker fades.
const uint8_t  DEFAULT_FADE_SPEED = 100;
// used for determining how fast blink routines are. This is how many
// frames it waits until switching the LED states from on or off. 
// a lower number speeds up the blink.
const uint8_t  DEFAULT_BLINK_SPEED = 3;
// default value that determines how many colors a custom color routine should use.
// This value must be less than the size of the custom color array. 
const uint8_t  DEFAULT_CUSTOM_COUNT = 2;
// default value that determines how large "bars" are, which are groups of LEDs
// of the same color in routines that display multiple colors or multiple
// shares of the same color. 
const uint8_t  DEFAULT_BAR_SIZE = 2;

//================================================================================
// Constructors
//================================================================================

ArduCor::ArduCor(uint16_t ledCount)
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

void ArduCor::resetToDefaults()
{
    // By default, this is set to orange. However,
    // most sample sketches will override this value
    // during their setup. 
    m_main_color = {100, 25, 0};
    
    // set user configurable settings
    m_current_group = eCustom;
    m_current_routine = eSingleGlimmer;
    
    brightness(DEFAULT_BRIGHTNESS);
    m_fade_speed   = DEFAULT_FADE_SPEED;
    m_blink_speed  = DEFAULT_BLINK_SPEED;
    m_custom_count = DEFAULT_CUSTOM_COUNT;
    m_bar_size     = DEFAULT_BAR_SIZE;
    // edge case for smaller LED arrays, rather than using multiple LEDs in a "bar"
    // it defaults to one LED per bar.
    if (m_LED_count < 32) {
        m_bar_size = 1;
    }
    
    // set temp values
    m_temp_index = 0;
    m_temp_counter = 0;
    m_temp_bool = true;
    m_temp_color = {0, 0, 0};
    m_scale_factor = 0;
    m_temp_float = 0.0f;
    m_temp_step = 0;
    m_temp_goal = 0;
    m_possible_array_color = 0;
    m_is_on = true;
    
    // set routine specific variables
    m_goal_color = {0, 0, 0};
    
    // set custom colors to default colors
    for (x = 0; x < 10; x = x + 5) {
        m_custom_colors[x]     = {0,   255, 0};     // green
        m_custom_colors[x + 1] = {125, 0,   255};   // teal
        m_custom_colors[x + 2] = {0,   0,   255};   // blue
        m_custom_colors[x + 3] = {40,  127, 40};    // light green
        m_custom_colors[x + 4] = {60,  0,   160};   // purple
    }  
}


//================================================================================
// Getters and Setters
//================================================================================

bool
ArduCor::setMainColor(uint8_t r, uint8_t g, uint8_t b)
{
    if ((m_main_color.red == r) 
        && (m_main_color.green == g)
        && (m_main_color.blue == b)) {
        return false;
    } else {
        m_main_color = {r, g, b};
        return true;
    }
}

void
ArduCor::setColor(uint16_t colorIndex, uint8_t r, uint8_t g, uint8_t b)
{
    if (colorIndex < (sizeof(m_custom_colors) / sizeof(Color))) {
        m_custom_colors[colorIndex] = {r, g, b};
        // catch edge case 
        if (m_current_group == eCustom) {
            m_preprocess_flag = true;
        }
    }
}


void 
ArduCor::setCustomColorCount(uint8_t count)
{
    if (count != 0) {
        m_custom_count = count;
        // catch edge case 
        if (m_current_group == eCustom) {
            m_preprocess_flag = true;
        }
    }
}

uint8_t 
ArduCor::customColorCount()
{
    return m_custom_count;
}

void 
ArduCor::brightness(uint8_t brightness)
{
    if (brightness <= 100) {
        m_bright_level = brightness;
        m_brightness_flag = true;
        // handle edge case where a routine that normally
        // needs to be drawn only once needs a redraw        
        if (m_current_routine == eMultiBarsSolid) {
            m_temp_bool = true;
        }
    }
}

void 
ArduCor::barSize(uint8_t barSize)
{
    if ((barSize != 0) 
        && (barSize < m_LED_count) 
        && (m_bar_size != barSize)) {
        m_bar_size = barSize;
        m_preprocess_flag = true;
    }
}


void 
ArduCor::fadeSpeed(uint8_t fadeSpeed)
{
    if (fadeSpeed != 0 && fadeSpeed < 200) {
        m_fade_speed = fadeSpeed;
    }
}


void 
ArduCor::blinkSpeed(uint8_t blinkSpeed)
{
    if (blinkSpeed != 0) {
        m_blink_speed = blinkSpeed;
    }
}


ArduCor::Color
ArduCor::mainColor()
{  
    return m_main_color;
}


ArduCor::Color
ArduCor::color(uint16_t i)
{
    if (i < (sizeof(m_custom_colors) / sizeof(Color))) {
        return m_custom_colors[i];
    } else {
        return (Color){0,0,0};
    }
}

uint8_t
ArduCor::red(uint16_t i)
{
    if ((i < m_LED_count) && m_is_on) {
        return r_buffer[i];
    } else {
        return 0;
    }
}

uint8_t
ArduCor::green(uint16_t i)
{
    if ((i < m_LED_count) && m_is_on) {
        return g_buffer[i];
    } else {
        return 0;
    }
}

uint8_t
ArduCor::blue(uint16_t i)
{
    if ((i < m_LED_count) && m_is_on) {
        return b_buffer[i];
    } else {
        return 0;
    }
}


  
//================================================================================
// Pre Processing
//================================================================================
 
void
ArduCor::preProcess(ELightingRoutine routine, EColorGroup group)
{
    // prevent illegal values
    if (group >= eColorGroup_MAX) {
        group = (EColorGroup)((uint8_t)eColorGroup_MAX - 1);
    }
    if (group < 0) {
        group = (EColorGroup)0;
    }

    // detect if its a single color solid routine. This is a special case
    // since it only needs to do its processing if its color is changed. 
    if (routine == (uint8_t)eSingleSolid) {
        m_preprocess_flag = !setMainColor(m_temp_color.red, 
                                          m_temp_color.green, 
                                          m_temp_color.blue);
    }
    
    //---------
    // Routine Has Changed
    //---------
    if ((m_current_routine != routine)
        || m_preprocess_flag) {
        // reset the temps
        m_temp_index = 0;
        m_temp_counter = 0;
        m_temp_bool = true;
        m_temp_color = {0,0,0};
        m_preprocess_flag = true;
        m_brightness_flag = true;
        m_current_routine = routine;
    }
    
    //---------
    // Group Has Changed
    //---------
    if ((m_current_group != group) 
        || m_preprocess_flag) {
        
        // reset flag
        m_preprocess_flag = false;
        // reset the temps
        m_temp_index = 0;
        m_temp_counter = 0;
        m_temp_bool = true;
        m_temp_color = {0,0,0};      
        
        // reset fades, even when only the colorGroup changes
        if (routine == eMultiFade) {
            m_temp_bool = true;
            m_temp_counter = 0;
            m_temp_float = m_fade_speed / 4.0f;
        }
        
        setupColorGroup(group); 
        // setup the buffer to do a moving array.
        if (routine == eMultiBarsMoving
            || routine == eMultiBarsSolid) {
            m_temp_index = 0;
            movingBufferSetup(m_temp_size, m_bar_size);
        }
        if (routine == eSingleWave) {
            m_temp_index = 0;
            m_temp_float = m_LED_count / (2 * m_bar_size);
            movingBufferSetup(m_temp_float, m_bar_size, 1);
        }   
        if (routine == eSingleSawtoothFadeOut) {
            m_temp_counter = m_fade_speed;
        }     
        m_current_group = group;
    }
}


void 
ArduCor::setupColorGroup(EColorGroup colorGroup)
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
        for (x = 0 ; x < (sizeof(m_temp_array) / sizeof(Color)); ++x) {
            m_temp_array[x] = { (uint8_t)random(0, 256),
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
// Single Color Routines
//================================================================================

void 
ArduCor::turnOff()
{
    if (m_is_on) {
      fillColorBuffers(0,0,0);
    }
    m_is_on = false;
}

void ArduCor::turnOn() {
    if (!m_is_on) {
      fillColorBuffers(m_temp_color.red, m_temp_color.green, m_temp_color.blue);
    }
    m_is_on = true;
}


void
ArduCor::singleSolid(uint8_t red, uint8_t green, uint8_t blue)
{
    m_temp_color = {red, green, blue};
    preProcess(eSingleSolid, m_current_group);
    if (m_temp_bool) {
        fillColorBuffers(red, green, blue);  
        m_temp_bool = false;
    }
}


void
ArduCor::singleBlink(uint8_t red, uint8_t green, uint8_t blue)
{
    preProcess(eSingleBlink, m_current_group);
    // switches states between on/off based off of m_blink_speed
    if (!(m_temp_counter % m_blink_speed)) {
        if (m_temp_bool) {
            fillColorBuffers(red, green, blue); 
            m_brightness_flag = true;
        } else {
            fillColorBuffers(0,0,0); 
        }
        m_temp_bool = !m_temp_bool;
    }
    m_temp_counter++;
}


void
ArduCor::singleWave(uint8_t red, uint8_t green, uint8_t blue)
{
    preProcess(eSingleWave, m_current_group);   
    m_repeat_index = 0;
    // loop through all the values between 0 and m_loop_index m_loop_count times.
    for (x = 0; x < (m_loop_count * m_loop_index); ++x) {
        // if we are computing values above m_LED_count, ignore.
        if (x < m_LED_count) {
            // m_temp_counter holds the index in this instance of a repeat through
            // the looped values.
            m_temp_counter = (m_repeat_index + m_temp_index) % m_loop_index;
            r_buffer[x] = (uint8_t)(red * (m_temp_buffer[m_temp_counter] / m_temp_float));
            g_buffer[x] = (uint8_t)(green * (m_temp_buffer[m_temp_counter] / m_temp_float));
            b_buffer[x] = (uint8_t)(blue * (m_temp_buffer[m_temp_counter] / m_temp_float));
            // if a loop is pushing a repeat_index over m_loop_index, go back to 0
            m_repeat_index = (x + 1) % m_loop_index;
        }
    }
    m_temp_index = (m_temp_index + 1) % m_loop_index;  
}


void
ArduCor::singleGlimmer(uint8_t red, uint8_t green, uint8_t blue, uint8_t percent)
{
    preProcess(eSingleGlimmer, m_current_group);
    // set all LEDs to the base color before applying glimmer
    // to a subsection of them. 
    fillColorBuffers(red, green, blue);
    for (x = 0; x < m_LED_count; ++x) {
        // a random number is generated. If its less than the percent,
        // treat this as an LED that gets a glimmer effect
        if (random(1,101) < percent && percent != 0) {
            // set a random level for the LED to be dimmed by.
            m_scale_factor = (uint8_t)random(2,7);
            r_buffer[x] = red / m_scale_factor;
            g_buffer[x] = green / m_scale_factor;
            b_buffer[x] = blue / m_scale_factor;
        } 
    }
}


void
ArduCor::singleFade(uint8_t red, uint8_t green, uint8_t blue, bool isSine)
{
    if (isSine) {
        preProcess(eSingleSineFade, m_current_group); 
        // calculate the next value using a sine function
        m_temp_float = (sin(((m_temp_counter / (float)m_fade_speed) * 6.28f) - 1.67f) + 1) / 2.0f; 
        m_temp_step = 1;
    } else {
        preProcess(eSingleLinearFade, m_current_group); 
        // calculate how far throuhg the routine you are
        m_temp_float = m_temp_counter / (float)m_fade_speed;
        m_temp_step = 2;
    }   
    // increment/decrement the counter
    if (m_temp_bool)  m_temp_counter = m_temp_counter + m_temp_step;
    else              m_temp_counter = m_temp_counter - m_temp_step;

    // constrain the fade
    if (m_temp_counter >= m_fade_speed) m_temp_bool = false;
    else if (m_temp_counter == 0)       m_temp_bool = true;
    
    // draws the current state of the fade to the buffers
    fillColorBuffers(red * m_temp_float, 
                     green * m_temp_float, 
                     blue * m_temp_float);
}

void
ArduCor::singleSawtoothFade(uint8_t red, uint8_t green, uint8_t blue, bool fadeIn)
{
    // set up values based on whether its a fade in or a fade out. 
    if (fadeIn) {
        preProcess(eSingleSawtoothFadeIn, m_current_group); 
        m_temp_goal = m_fade_speed;
        m_temp_index = 0;    
        m_temp_step = 1;  
    } else {
        preProcess(eSingleSawtoothFadeOut, m_current_group);    
        m_temp_goal = 0; 
        m_temp_index = m_fade_speed;  
        m_temp_step = -1;
    }
    // apply the fade
    if (m_temp_bool) {
        m_temp_counter = m_temp_counter + m_temp_step;
    } else {
        m_temp_counter = m_temp_index;
        m_temp_bool = true;
    }

    // constrain the fade
    if (m_temp_counter == m_temp_goal) m_temp_bool = false;
    // draws the current state of the fade to the buffers
    fillColorBuffers((red * (m_temp_counter / (float)m_fade_speed)),
                     (green * (m_temp_counter / (float)m_fade_speed)),
                     (blue * (m_temp_counter / (float)m_fade_speed)));
}


//================================================================================
// Multi Color Routines
//================================================================================

void
ArduCor::multiGlimmer(EColorGroup colorGroup, uint8_t percent)
{
    preProcess(eMultiGlimmer, colorGroup);
    // set all LEDs to the base color before applying glimmer
    // to a subsection of them. 
    fillColorBuffers(m_temp_array[0].red, 
                     m_temp_array[0].green,
                     m_temp_array[0].blue);
    for (x = 0; x < m_LED_count; ++x) {
        if (random(1,101) < percent && percent != 0) {
            // m_temp_color is set in chooseRandomFromArray
            chooseRandomFromArray(m_temp_array, m_temp_size, true);
        } else {
            m_temp_color = m_temp_array[0];
        }

        // a random number is generated, if its less than the percent,
        // treat this as an LED that gets a glimmer effect
        if (random(1,101) < percent && percent != 0) {
            // chooses how much to divide the input by
            m_scale_factor = (uint8_t)random(2,7);
            r_buffer[x] = m_temp_color.red / m_scale_factor;
            g_buffer[x] = m_temp_color.green / m_scale_factor;
            b_buffer[x] = m_temp_color.blue / m_scale_factor;
        } else {
            r_buffer[x] = m_temp_color.red;
            g_buffer[x] = m_temp_color.green;
            b_buffer[x] = m_temp_color.blue;
        }
    }
}


void
ArduCor::multiFade(EColorGroup colorGroup)
{
    preProcess(eMultiFade, colorGroup); 
    // checks if it should change the colors it is fading between.
    if (m_temp_bool) {
        m_temp_bool = false;
        if (m_temp_size > 1) {
            m_fade_counter = 0;
            m_temp_counter = (m_temp_counter + 1) % m_temp_size;
            m_temp_color = m_temp_array[m_temp_counter];
            m_goal_color = m_temp_array[(m_temp_counter + 1) % m_temp_size];
            m_red_diff   = m_temp_color.red - m_goal_color.red;
            m_green_diff = m_temp_color.green - m_goal_color.green;
            m_blue_diff  = m_temp_color.blue - m_goal_color.blue;
        } else {
            m_temp_counter = 0;
            m_goal_color = m_temp_array[0];
            m_temp_color = m_temp_array[0];
            m_red_diff = 0;
            m_green_diff = 0;
            m_blue_diff = 0;
        }
    }
    
   // draws to buffer
   fillColorBuffers(m_temp_color.red - (m_red_diff * (m_fade_counter / m_temp_float)),
                    m_temp_color.green - (m_green_diff * (m_fade_counter / m_temp_float)),
                    m_temp_color.blue - (m_blue_diff * (m_fade_counter / m_temp_float)));

    if (m_fade_counter == m_temp_float) m_temp_bool = true;    
    m_fade_counter++;
}


void
ArduCor::multiRandomSolid(EColorGroup colorGroup)
{
    preProcess(eMultiRandomSolid, colorGroup); 
    if (!(m_temp_counter % m_blink_speed)) {
        switch ((EColorGroup)colorGroup) {
            case eAll:
                // uses a random color instead of the m_temp_array buffer. 
                m_temp_color = {(uint8_t)random(0, 256),
                                (uint8_t)random(0, 256),
                                (uint8_t)random(0, 256)};
                for (x = 0; x < m_LED_count; ++x) {
                    r_buffer[x] = m_temp_color.red;
                    g_buffer[x] = m_temp_color.green;
                    b_buffer[x] = m_temp_color.blue;
                }
                break;
            default:
                chooseRandomFromArray(m_temp_array, m_temp_size, true);
                fillColorBuffers(m_temp_color.red, m_temp_color.green, m_temp_color.blue);
                break;
        }
        // always apply the brightness after an update
        m_brightness_flag = true;
    }   
    m_temp_counter++;
}

void
ArduCor::multiRandomIndividual(EColorGroup colorGroup)
{   
    preProcess(eMultiRandomIndividual, colorGroup);  
    switch ((EColorGroup)colorGroup) {
        case eAll:
            // uses random values for each individual LED
            // instead of the m_temp_array buffer. 
            for (x = 0; x < m_LED_count; ++x) {
                r_buffer[x] = (uint8_t)random(0, 256);
                g_buffer[x] = (uint8_t)random(0, 256);
                b_buffer[x] = (uint8_t)random(0, 256);
            }
            break;
        default:
            for (x = 0; x < m_LED_count; ++x) {
                // chooses a random color from m_temp_array
                chooseRandomFromArray(m_temp_array, m_temp_size, true);
                // draws the random color to the buffer.
                r_buffer[x] = m_temp_color.red;
                g_buffer[x] = m_temp_color.green;
                b_buffer[x] = m_temp_color.blue;
            }
            break;
    }
}



void
ArduCor::multiBarsSolid(EColorGroup colorGroup, uint8_t barSizeSetting)
{   
    barSize(barSizeSetting);
    preProcess(eMultiBarsSolid, colorGroup);  
    if (m_temp_bool) {
        m_temp_counter = 0;
        m_temp_index = 0;
        for(x = 0; x < m_LED_count; ++x) {
            r_buffer[x] = m_temp_array[m_temp_index].red;
            g_buffer[x] = m_temp_array[m_temp_index].green;
            b_buffer[x] = m_temp_array[m_temp_index].blue;
            m_temp_counter++;
            if (m_temp_counter == m_bar_size) {
                m_temp_counter = 0;
                m_temp_index = (m_temp_index + 1) % m_temp_size;
            }
        }
        m_temp_bool = false;
    }
}


void
ArduCor::multiBarsMoving(EColorGroup colorGroup, uint8_t barSizeSetting)
{   
    barSize(barSizeSetting);
    preProcess(eMultiBarsMoving, colorGroup);
    m_repeat_index = 0;
    // loop through all the values between 0 and m_loop_index m_loop_count times.
    for (x = 0; x < (m_loop_count * m_loop_index); ++x) {
        // if we are computing values above m_LED_count, ignore.
        if (x < m_LED_count) {
            // m_temp_counter holds the index in this instance of a repeat through
            // the looped values.
            m_temp_counter = (m_repeat_index + m_temp_index) % m_loop_index;
            r_buffer[x] = m_temp_array[m_temp_buffer[m_temp_counter]].red;
            g_buffer[x] = m_temp_array[m_temp_buffer[m_temp_counter]].green;
            b_buffer[x] = m_temp_array[m_temp_buffer[m_temp_counter]].blue;
            // if a loop is pushing a repeat_index over m_loop_index, go back to 0
            m_repeat_index = (x + 1) % m_loop_index;
        }
    }
    m_temp_index = (m_temp_index + 1) % m_loop_index;
}

//================================================================================
// Post-Processing
//================================================================================

void
ArduCor::applyBrightness()
{
    //  brightness is required
    if (m_brightness_flag && m_is_on) {
        // if brightness is only needed once, unset the flag
        if ((m_current_routine == eSingleSolid)
            || (m_current_routine == eSingleBlink)
            || (m_current_routine == eMultiBarsSolid)
            || (m_current_routine == eMultiRandomSolid)) {
            m_brightness_flag = false;        
        }
        // loop again to apply global effects
        for(x = 0; x < m_LED_count; ++x) {
            // Since this is expensive and often run on every LED update, we avoid
            // floating point calculations for a bit of a speed increase. 
            r_buffer[x] = (uint8_t)((r_buffer[x] * (uint16_t)m_bright_level) / 100); 
            g_buffer[x] = (uint8_t)((g_buffer[x] * (uint16_t)m_bright_level) / 100);  
            b_buffer[x] = (uint8_t)((b_buffer[x] * (uint16_t)m_bright_level) / 100);  
        }
    }
}

    
bool 
ArduCor::drawColor(uint16_t i, uint8_t red, uint8_t green, uint8_t blue)
{
    // checks if its valid draw
    if (i < m_LED_count) {
        r_buffer[i] = red;
        g_buffer[i] = green;
        b_buffer[i] = blue;
        return true;
    }
    return false;
}

//================================================================================
// Helper Functions
//================================================================================

void
ArduCor::movingBufferSetup(uint16_t colorCount, uint8_t groupSize, uint8_t startingValue)
{
    if ((groupSize * colorCount) > m_LED_count) {
        // edge case handled for memory reasons, a full loop must
        // take less than the m_LED_count 
        groupSize = 1;
    }
    // minimum number of values needed for a looping pattern.
    m_loop_index = groupSize * colorCount;
    // minimum number of times we need to loop these values to 
    // completely fill the LEDs.
    m_loop_count = ((m_LED_count / m_loop_index) + 1); 
    // change the starting value for routines like singleWave
    if (startingValue < colorCount) {
        m_temp_index = startingValue;
    } else {
        startingValue = 0;
    }
      
    //the buffer from 0 to m_loop_index with the proper bars
    for (x = 0; x < m_loop_index; ++x) {
        m_temp_buffer[x] = m_temp_index;
        m_temp_counter++;
        if (m_temp_counter == groupSize) {
            m_temp_counter = 0;
            m_temp_index++;
            if (m_temp_index == colorCount) {
                m_temp_index = startingValue;
            }
        }
    }
}


void 
ArduCor::fillColorBuffers(uint8_t r, uint8_t g, uint8_t b)
{
    memset(r_buffer, r, m_LED_count);
    memset(g_buffer, g, m_LED_count);
    memset(b_buffer, b, m_LED_count);  
}

void
ArduCor::chooseRandomFromArray(Color *array, uint8_t max_index, boolean canRepeat)
{   
    m_possible_array_color = random(0, max_index);
    if (!canRepeat && max_index > 2) {
      while (m_possible_array_color == m_temp_index) {
         m_possible_array_color = (uint8_t)random(0, max_index);
      }
    }
    m_temp_index = m_possible_array_color;
    m_temp_color = array[m_temp_index];
}
