/*!
 * \version v1.9.0
 * \date April 24, 2016
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


const uint16_t DEFAULT_BRIGHTNESS  = 50;
const uint8_t  DEFAULT_FADE_SPEED  = 10;
const uint8_t  DEFAULT_BLINK_SPEED = 3;

   
//================================================================================
// Constructors
//================================================================================

RoutinesRGB::RoutinesRGB(uint16_t ledCount, uint16_t colorCount)
{
    m_LED_count = ledCount;
    // catch an illegal argument
    if (m_LED_count == 0) {
        m_LED_count = 1;
    }
    
    m_color_count = colorCount;
    // catch an illegal argument
    if (m_color_count == 0) {
        m_color_count = 1;
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
    
    
    //=================================
    // allocate and set up the presets
    //=================================        
        
    m_preset_total = 19;
    int i = 0;
    
    //==========
    // allocate array of arrays
    //==========
    if ((m_colors =(Color**)malloc(m_preset_total * sizeof(Color*)))) {
        memset(m_colors, 0, ledCount);
    }
      
    //==========
    // Allocate array of array sizes
    //==========  
    if((m_preset_count = (uint8_t*)malloc(m_preset_total))) {
        memset(m_preset_count, 0, m_preset_total);
    }    
    
    //==========
    // Custom Colors
    //==========
    m_preset_count[i] = colorCount;

    uint16_t byteCount = colorCount * 3; 
    if((m_colors[i] = (Color*)malloc(byteCount))) {
        memset(m_colors[i], 0, ledCount);
    }
    i++;
    
    //==========
    // Water Colors
    //==========
    m_preset_count[i] = 5;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    }
    
    m_colors[i][0] = {0,   0,   255};
    m_colors[i][1] = {0,   25,  225};
    m_colors[i][2] = {0,   0,   127};
    m_colors[i][3] = {0,   127, 127};
    m_colors[i][4] = {120, 120, 255};

    i++;
    
    //==========
    // Frozen Colors
    //==========
    m_preset_count[i] = 6;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    }
    
    m_colors[i][0] = {0,   127, 255};
    m_colors[i][1] = {0,   127, 127};
    m_colors[i][2] = {200, 0,   255};
    m_colors[i][3] = {40,  127, 40};
    m_colors[i][4] = {127, 127, 127};
    m_colors[i][5] = {127, 127, 255};

    i++;

    //==========
    // Snow Colors
    //==========
    m_preset_count[i] = 6;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    }
    
    m_colors[i][0] = {255, 255, 255};
    m_colors[i][1] = {127, 127, 127};
    m_colors[i][2] = {200, 200, 200};
    m_colors[i][3] = {0,   0,   255};
    m_colors[i][4] = {0,  255,  255};
    m_colors[i][5] = {0,  180,   180};
    
    i++;
        
    //==========
    // Cool Colors
    //==========
    m_preset_count[i] = 5;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    }
    
    m_colors[i][0] = {0,   255, 0};
    m_colors[i][1] = {125, 0,   255};
    m_colors[i][2] = {0,   0,   255};
    m_colors[i][3] = {40,  127, 40};
    m_colors[i][4] = {60,  0,   160};
     
    i++;

    //==========
    // Warm Colors
    //==========
    m_preset_count[i] = 5;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    }
    
    m_colors[i][0] = {255, 255, 0};
    m_colors[i][1] = {255, 0,   0};
    m_colors[i][2] = {255, 45,  0};
    m_colors[i][3] = {255, 200, 0};
    m_colors[i][4] = {255, 127, 0};
    
    i++;

    //==========
    // Fire Colors
    //==========
    m_preset_count[i] = 9;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    }
    
    m_colors[i][0] = {255, 75, 0};
    m_colors[i][1] = {255, 20, 0};
    m_colors[i][2] = {255, 80, 0};
    m_colors[i][3] = {255, 5,  0};
    m_colors[i][4] = {0,   0,  0};
    m_colors[i][5] = {127, 127,0};
    m_colors[i][6] = {255, 60, 0};
    m_colors[i][7] = {255, 45,0};
    m_colors[i][8] = {127, 127,0};

    i++;
    
    //==========
    // Evil Colors
    //==========
    m_preset_count[i] = 7;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    }
    
    m_colors[i][0] = {255, 0, 0};
    m_colors[i][1] = {200, 0, 0};
    m_colors[i][2] = {127, 0, 0};
    m_colors[i][3] = {20,  0, 0};
    m_colors[i][4] = {30,  0, 40};
    m_colors[i][5] = {0,   0, 0};
    m_colors[i][6] = {80,  0, 0};

    i++;
    
    //==========
    // Corrosive Colors
    //==========
    m_preset_count[i] = 5;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    }
    
    m_colors[i][0] = {0,   255, 0};
    m_colors[i][1] = {0,   200, 0};
    m_colors[i][2] = {60,  180, 60};
    m_colors[i][3] = {127, 135, 127};
    m_colors[i][4] = {10,  255, 10};
    
    i++;
    
    //==========
    // Poison Colors
    //==========
    m_preset_count[i] = 9;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    }
    
    m_colors[i][0] = {80, 0, 180};
    m_colors[i][1] = {120, 0, 255};
    m_colors[i][2] = {10, 0,   20};
    m_colors[i][3] = {25, 0,  25};
    m_colors[i][4] = {60, 40,  60};
    m_colors[i][5] = {120, 0, 255};
    m_colors[i][6] = {80, 0, 180};
    m_colors[i][7] = {40, 0, 90};
    m_colors[i][8] = {80, 0, 180};

    i++;
   
    //==========
    // Rose
    //==========
    m_preset_count[i] = 6;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    }
        
    m_colors[i][0] = {216, 30, 100};
    m_colors[i][1] = {156, 62, 72};
    m_colors[i][2] = {255, 245, 251};
    m_colors[i][3] = {127, 127, 127};
    m_colors[i][4] = {194, 30,  86};
    m_colors[i][5] = {194, 30,  30};

    i++;
    
    //==========
    // Pink Green
    //==========
    m_preset_count[i] = 4;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    }   
    
    m_colors[i][0]  = {255, 20,  147};
    m_colors[i][1]  = {0,   255, 0};
    m_colors[i][2]  = {0,   200, 0};
    m_colors[i][3]  = {255, 105, 180};
        
    i++;     

    //==========
    // Red White Blue
    //==========
    m_preset_count[i] = 4;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    }        
        
        
    m_colors[i][0]  = {255, 255, 255};
    m_colors[i][1]  = {255, 0,   0};
    m_colors[i][2]  = {0,   0,   255};
    m_colors[i][3]  = {255, 255, 255};
    
    i++;

    //==========
    // All Colors
    //==========
    m_preset_count[i] = 8;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    }
    i++;

    //==========
    // RGB
    //==========
    m_preset_count[i] = 3;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    }    
    
    m_colors[i][0] = {255, 0,   0};
    m_colors[i][1] = {0,   255, 0};
    m_colors[i][2] = {0,   0,   255};

    i++;
    
    //==========
    // CMY
    //==========
    m_preset_count[i] = 3;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    } 
        
    m_colors[i][0] = {255, 255, 0};
    m_colors[i][1] = {0,   255, 255};
    m_colors[i][2] = {255,   0, 255};

    i++;   
    
    //==========
    // Six Color
    //==========
    m_preset_count[i] = 6;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    }   
              
    m_colors[i][0] = {255, 0,   0};
    m_colors[i][1] = {255, 255, 0};
    m_colors[i][2] = {0,   255, 0};
    m_colors[i][3] = {0,   255, 255};
    m_colors[i][4] = {0,   0,   255};
    m_colors[i][5] = {255, 0,   255};

    i++;   

    //==========
    // Seven Color
    //==========
    m_preset_count[i] = 7;
    
    if((m_colors[i] = (Color*)malloc((uint16_t)(m_preset_count[i] * 3)))) {
        memset(m_colors[i], 0, (uint16_t)(m_preset_count[i] * 3));
    }        
    
    m_colors[i][0] = {255, 0,   0};
    m_colors[i][1] = {255, 255, 0};
    m_colors[i][2] = {0,   255, 0};
    m_colors[i][3] = {0,   255, 255};
    m_colors[i][4] = {0,   0,   255};
    m_colors[i][5] = {255, 0,   255};
    m_colors[i][6] = {255, 255, 255};

    i++;   
    
    // all colors gets set before use since it changes each times
    resetToDefaults(); 
}

void RoutinesRGB::resetToDefaults()
{
    m_bright_level = DEFAULT_BRIGHTNESS;
    m_fade_speed   = DEFAULT_FADE_SPEED;
    m_blink_speed  = DEFAULT_BLINK_SPEED;
    m_bar_size = 0;
    m_bar_count = 0;
    
    m_current_preset = eCustom;
    m_current_mode = eSingleGlimmer;
    
    m_temp_index = 0;
    m_temp_counter = 0;
    m_temp_bool = true;
    m_temp_color = {0,0,0};
    
    // colors used by specific routines
    m_fade_color = {255, 0,0};
    m_goal_color = {0,0,0};
    
    // used at start of fade to switch colors
    m_start_next_fade = true;
    // turn off the lights when idle
    m_idle_timeout_on = true;
    // stored value for fade
    m_solid_fade_value = 0;
    
    // counters for routines
    m_fade_counter  = 0;
    m_fade_array_counter = 0;
    
    for (int i = 0; i < m_color_count; i++) {
        m_colors[0][i] = {0, 0, 0};
    }  
    
    // All LEDs buffer
    for (int i = 0; i < m_preset_count[(int)eAll]; i++) {
         m_colors[(int)eAll][i] =  {(uint8_t)random(0, 256),
                                    (uint8_t)random(0, 256),
                                    (uint8_t)random(0, 256)};
    }
    
}

//================================================================================
// Getters and Setters
//================================================================================

void
RoutinesRGB::setMainColor(uint8_t r, uint8_t g, uint8_t b)
{
    m_main_color = (RoutinesRGB::Color){r, g, b};
}

void
RoutinesRGB::setColor(uint16_t colorIndex, uint8_t r, uint8_t g, uint8_t b)
{
    if (colorIndex < m_color_count) {
        m_colors[0][colorIndex] = (RoutinesRGB::Color){r, g, b};
    }
}


void 
RoutinesRGB::setColorCount(uint8_t count)
{
    if (count != 0) {
        m_preset_count[0] = count;
    }
}

uint8_t 
RoutinesRGB::colorCount()
{
    return m_preset_count[0];
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
    if (i < m_color_count) {
        return m_colors[0][i];
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
RoutinesRGB::preProcess(ELightingMode newState, EColorPreset newPreset)
{
    m_preprocess_flag = false;
    
    // prevent illegal values
    if (newPreset >= m_preset_total) {
        newPreset = (EColorPreset)(m_preset_total - 1);
    }
    if (newPreset < 0) {
        newPreset = (EColorPreset)0;
    }
    
    //---------
    // Either State or Preset Has Changed
    //---------
    if ( m_current_mode != newState
        || m_current_preset != newPreset) { 
        // setup the temp values with the preset.
        if (newState == eMultiGlimmer
            || newState == eMultiFade
            || newState == eMultiBarsSolid
            || newState == eMultiBarsMoving
            || newState == eMultiRandomSolid
            || newState == eMultiRandomIndividual) {
            m_temp_array =  m_colors[newPreset];
            m_temp_size =  m_preset_count[newPreset];
        }
        // setup the buffer to do a moving array.
        if (newState == eMultiBarsMoving
            || newState == eMultiBarsSolid) {
            movingBufferSetup(m_temp_size, m_bar_size);
        }
    }
        
    //---------
    // State Has Changed
    //---------
    if (m_current_mode != newState) {
        // change the current state
        m_current_mode = newState;
        // reset the temps
        m_temp_index = 0;
        m_temp_counter = 0;
        m_temp_bool = true;
        m_temp_color = {0,0,0};
        
        // always reset blink
        if (m_current_mode == eSingleBlink) {
            m_temp_counter = 0;
            m_temp_bool = true;
        }
        m_preprocess_flag = true;
    }
    
    //---------
    // Preset Has Changed
    //---------
    if (((m_current_preset != newPreset) 
        || m_preprocess_flag)
        && (newPreset != -1)) {

        // reset fades, even when only the preset changes
        if (m_current_mode == eMultiFade) {
            m_start_next_fade = true;
            m_fade_array_counter = 0;
        }
        
        // for "all" color routines, generate random colors for the array.
        if (newPreset == eAll) {
            for (int i = 0; i < m_preset_count[(int)10]; i++) {
                 m_colors[(int)eAll][i] =  {(uint8_t)random(0, 256),
                                            (uint8_t)random(0, 256),
                                            (uint8_t)random(0, 256)};
            }
        }
        m_current_preset = newPreset;
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
RoutinesRGB::solid(uint8_t red, uint8_t green, uint8_t blue)
{
    preProcess(eSingleSolid, m_current_preset);
    for (uint16_t x = 0; x < m_LED_count; x++) {
        r_buffer[x] = red;
        g_buffer[x] = green;
        b_buffer[x] = blue;
    }
    postProcess();
}


void
RoutinesRGB::blink(uint8_t red, uint8_t green, uint8_t blue)
{
    preProcess(eSingleBlink, m_current_preset);
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
RoutinesRGB::fade(uint8_t red, uint8_t green, uint8_t blue, uint8_t fadeSpeed, boolean shouldUpdate)
{
    preProcess(eSingleFade, m_current_preset);
    if (shouldUpdate) {
        // catch illegal argument and fix it
        if (fadeSpeed == 0) fadeSpeed = 1;
    
        // apply the fade
        if (m_temp_bool)  m_solid_fade_value++;
        else              m_solid_fade_value--;
    
        // constrain the fade
        if (m_solid_fade_value == fadeSpeed) m_temp_bool = false;
        else if (m_solid_fade_value == 0)    m_temp_bool = true;
    }
    
    // set up the buffers
    for (int x = 0; x < m_LED_count; x++) {
        r_buffer[x] = (uint8_t)(red   * (m_solid_fade_value / (float)fadeSpeed));
        g_buffer[x] = (uint8_t)(green * (m_solid_fade_value / (float)fadeSpeed));
        b_buffer[x] = (uint8_t)(blue  * (m_solid_fade_value / (float)fadeSpeed));
    }
    postProcess();
}



void
RoutinesRGB::glimmer(uint8_t red, uint8_t green, uint8_t blue, long percent, boolean shouldUpdate)
{
    preProcess(eSingleGlimmer, m_current_preset);
    for (uint16_t x = 0; x < m_LED_count; x++) {
        // a random number is generated. If its less than the percent,
        // treat this as an LED that gets a glimmer effect
        if (random(1,101) < percent && percent != 0 && shouldUpdate) {
            // set a random level for the LED to be dimmed by.
            byte scaleFactor = (byte)random(2,6);
            
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
    postProcess();
}


//================================================================================
// Routines With The Color Array
//================================================================================

void
RoutinesRGB::arrayGlimmer(EColorPreset preset, long percent)
{
    preProcess(eMultiGlimmer, preset);
    
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

            byte rScaled = m_temp_color.red / scaleFactor;
            byte gScaled = m_temp_color.green / scaleFactor;
            byte bScaled = m_temp_color.blue / scaleFactor;

            r_buffer[x] = rScaled;
            g_buffer[x] = gScaled;
            b_buffer[x] = bScaled;
        } else {
            r_buffer[x] = m_temp_color.red;
            g_buffer[x] = m_temp_color.green;
            b_buffer[x] = m_temp_color.blue;
        }
    }
    postProcess();
}


void
RoutinesRGB::arrayFade(EColorPreset preset)
{   
    preProcess(eMultiFade, preset);  

    if (m_start_next_fade) {
        m_start_next_fade = false;
        if (m_temp_size > 1) {
            m_fade_array_counter = (m_fade_array_counter + 1) % m_temp_size;
            m_temp_color = m_temp_array[m_fade_array_counter];
            m_goal_color = m_temp_array[(m_fade_array_counter + 1) % m_temp_size];
        } else {
            m_fade_array_counter = 0;
            m_goal_color = m_temp_array[0];
            m_temp_color = m_temp_array[0];
        }
    }

    m_temp_bool = true;
    m_temp_color.red = fadeBetweenValues(m_temp_color.red, m_goal_color.red);
    m_temp_color.green = fadeBetweenValues(m_temp_color.green, m_goal_color.green);
    m_temp_color.blue = fadeBetweenValues(m_temp_color.blue, m_goal_color.blue);
    m_start_next_fade = m_temp_bool;

    for (uint16_t x = 0; x < m_LED_count; x++) {
        r_buffer[x] = m_temp_color.red;
        g_buffer[x] = m_temp_color.green;
        b_buffer[x] = m_temp_color.blue;
    }
    postProcess();
}


void
RoutinesRGB::arrayRandomSolid(EColorPreset preset)
{
    preProcess(eMultiRandomSolid, preset); 
      
    if (!(m_temp_counter % m_blink_speed)) {
        switch ((EColorPreset)preset) {
            case eAll:
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
RoutinesRGB::arrayRandomIndividual(EColorPreset preset)
{   
    preProcess(eMultiRandomIndividual, preset);  

    switch ((EColorPreset)preset) {
        case eAll:
            for (uint16_t x = 0; x < m_LED_count; x++) {
                chooseRandomFromArray(m_temp_array, m_temp_size, true);
                r_buffer[x] = (uint8_t)random(0, 256);
                g_buffer[x] = (uint8_t)random(0, 256);
                b_buffer[x] = (uint8_t)random(0, 256);
            }
            break;
        default:
            for (uint16_t x = 0; x < m_LED_count; x++) {
                chooseRandomFromArray(m_temp_array, m_temp_size, true);
                r_buffer[x] = m_temp_color.red;
                g_buffer[x] = m_temp_color.green;
                b_buffer[x] = m_temp_color.blue;
            }
            break;
    }
    postProcess();
}



void
RoutinesRGB::arrayBarsSolid(EColorPreset preset, uint8_t barSize)
{   
    m_bar_size =  barSize;
    preProcess(eMultiBarsSolid, preset);  
     
    m_temp_counter = 0;
    m_temp_index = 0;
    // set up the buffers
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
RoutinesRGB::arrayBarsMoving(EColorPreset preset, uint8_t barSize)
{   
    m_bar_size =  barSize;
    preProcess(eMultiBarsMoving, preset);
    
    for(uint16_t x = 0; x < m_LED_count - m_temp_index; x++) {
        r_buffer[x] = m_temp_array[m_temp_buffer[x + m_temp_index]].red;
        g_buffer[x] = m_temp_array[m_temp_buffer[x + m_temp_index]].green;
        b_buffer[x] = m_temp_array[m_temp_buffer[x + m_temp_index]].blue;
    }
    int z = 0;
    for (uint16_t x = m_LED_count - m_temp_index; x < m_LED_count; x++) {
        r_buffer[x] = m_temp_array[m_temp_buffer[z]].red;
        g_buffer[x] = m_temp_array[m_temp_buffer[z]].green;
        b_buffer[x] = m_temp_array[m_temp_buffer[z]].blue;
        z++;
    }
    m_temp_index = (m_temp_index + 1) % m_temp_size;
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
    m_bar_count = colorCount;
    uint8_t index = 0;
    uint8_t counter = 0;
    for (uint16_t x = 0; x < m_LED_count; x++) {
        m_temp_buffer[x] = index;
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

