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
 
#include "RoutinesRGB.h"


const uint16_t DEFAULT_BRIGHTNESS  = 50;
const uint8_t  DEFAULT_FADE_SPEED  = 10;
const uint8_t  DEFAULT_BLINK_SPEED = 3;


/*!
 * The different states that the library can be ran in. These get changed by
 * using different API calls. 
 */
enum ELightingState
{
  eSolid,
  eBlink,
  eFade,
  eGlimmer,
  eRandomIndividual,
  eRandomSolid,
  eFadeAllColors,
  eSavedGlimmer,
  eSavedRandomIndividual,
  eSavedRandomSolid,
  eSavedFade,
  eSavedBarSolid,
  eSavedBarMoving
};


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

    uint16_t byteCount = colorCount * 3; 
    if((colors = (Color*)malloc(byteCount))) {
        memset(colors, 0, ledCount);
    } 
    
    m_bright_level = DEFAULT_BRIGHTNESS;
    m_fade_speed   = DEFAULT_FADE_SPEED;
    m_blink_speed  = DEFAULT_BLINK_SPEED;
    m_bar_size = 0;
    m_bar_count = 0;
    
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
    m_fade_saved_counter = 0;  
}


//================================================================================
// Getters and Setters
//================================================================================


void
RoutinesRGB::setColor(uint16_t colorIndex, uint8_t r, uint8_t g, uint8_t b)
{
    if (colorIndex < m_color_count) {
        colors[colorIndex] = (RoutinesRGB::Color){r, g, b};
    }
}

void 
RoutinesRGB::setBrightness(uint8_t brightness)
{
    if (brightness <= 100) {
        m_bright_level = brightness;
    }
}


void 
RoutinesRGB::setFadeSpeed(uint8_t fadeSpeed)
{
    if (fadeSpeed != 0) {
        m_fade_speed = fadeSpeed;
    }
}


void 
RoutinesRGB::setBlinkSpeed(uint8_t blinkSpeed)
{
    if (blinkSpeed != 0) {
        m_blink_speed = blinkSpeed;
    }
}

RoutinesRGB::Color
RoutinesRGB::getColor(uint16_t i)
{
    if (i < m_color_count) {
        return colors[i];
    } else {
        return (Color){0,0,0};
    }
}


uint8_t
RoutinesRGB::getR(uint16_t i)
{
    if (i < m_LED_count) {
        return r_buffer[i];
    } else {
        return 0;
    }
}

uint8_t
RoutinesRGB::getG(uint16_t i)
{
    if (i < m_LED_count) {
        return g_buffer[i];
    } else {
        return 0;
    }
}

uint8_t
RoutinesRGB::getB(uint16_t i)
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
RoutinesRGB::preProcess(uint16_t state)
{
    ELightingState new_state = (ELightingState)state;
    if (new_state != m_current_state) {
        // change the current state
        m_current_state = new_state;
        // reset the temps
        m_temp_index = 0;
        m_temp_counter = 0;
        m_temp_bool = true;
        m_temp_color = {0,0,0};
        // apply state specific changes
         if (m_current_state == eBlink) {
           m_temp_counter = 0;
           m_temp_bool = true;
         }
         else if (m_current_state == eRandomSolid) {
           m_temp_counter = 0;
           m_temp_index = 1;
         } 
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
    preProcess(eSolid);
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
    preProcess(eBlink);
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
RoutinesRGB::fade(uint8_t red, uint8_t green, uint8_t blue, uint8_t fadeSpeed)
{
    preProcess(eFade);
    // catch illegal argument and fix it
    if (fadeSpeed == 0) fadeSpeed = 1;
    
    // apply the fade
    if (m_temp_bool)  m_solid_fade_value++;
    else              m_solid_fade_value--;
    
    // constrain the fade
    if (m_solid_fade_value == fadeSpeed) m_temp_bool = false;
    else if (m_solid_fade_value == 0)    m_temp_bool = true;
    
    // set up the buffers
    for (int x = 0; x < m_LED_count; x++) {
        r_buffer[x] = (uint8_t)(red   * (m_solid_fade_value / (float)fadeSpeed));
        g_buffer[x] = (uint8_t)(green * (m_solid_fade_value / (float)fadeSpeed));
        b_buffer[x] = (uint8_t)(blue  * (m_solid_fade_value / (float)fadeSpeed));
    }
    postProcess();
}



void
RoutinesRGB::glimmer(uint8_t red, uint8_t green, uint8_t blue, long percent)
{
    preProcess(eGlimmer);
    for (uint16_t x = 0; x < m_LED_count; x++) {
        // a random number is generated. If its less than the percent,
        // treat this as an LED that gets a glimmer effect
        if (random(1,101) < percent && percent != 0) {
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
// Multi Color Routines
//================================================================================


void
RoutinesRGB::randomIndividual()
{
    preProcess(eRandomIndividual);
    for (uint16_t x = 0; x < m_LED_count; x++) {
        r_buffer[x] = (uint8_t)random(0, 256);
        g_buffer[x] = (uint8_t)random(0, 256);
        b_buffer[x] = (uint8_t)random(0, 256);
    }
    postProcess();
}


void
RoutinesRGB::randomSolid()
{
    preProcess(eRandomSolid);
    if (!(m_temp_counter % m_blink_speed)) {
        byte r = (uint8_t)random(0, 256);
        byte g = (uint8_t)random(0, 256);
        byte b = (uint8_t)random(0, 256);
        for (uint16_t x = 0; x < m_LED_count; x++) {
            r_buffer[x] = r;
            g_buffer[x] = g;
            b_buffer[x] = b;
        }
        postProcess();
    }
    m_temp_counter++;
}

void
RoutinesRGB::fadeBetweenAllColors()
{
    preProcess(eFadeAllColors);
    fadeAllColorsIncrement();
    for (uint16_t x = 0; x < m_LED_count; x++) {
        r_buffer[x] = m_fade_color.r;
        g_buffer[x] = m_fade_color.g;
        b_buffer[x] = m_fade_color.b;
    }
    postProcess();
}



//================================================================================
// Routines With Saved Colors
//================================================================================


void
RoutinesRGB::savedGlimmer(uint16_t colorCount, long percent)
{
    // protect from illegal arguments
    colorCount = constrain(colorCount, 1, m_color_count);
    
    preProcess(eSavedGlimmer);
    for (uint16_t x = 0; x < m_LED_count; x++) {
        m_temp_color = colors[0];
        if (random(1,101) < percent && percent != 0) {
            // m_temp_color is set in chooseRandomSaved
            chooseRandomSaved(colorCount, true);
        }
        
        // a random number is generated, if its less than the percent,
        // treat this as an LED that gets a glimmer effect
        if (random(1,101) < percent && percent != 0) {
            // chooses how much to divide the input by
            byte scaleFactor = (byte)random(2,6);
            
            byte rScaled = m_temp_color.r / scaleFactor;
            byte gScaled = m_temp_color.g / scaleFactor;
            byte bScaled = m_temp_color.b / scaleFactor;
            
            r_buffer[x] = rScaled;
            g_buffer[x] = gScaled;
            b_buffer[x] = bScaled;
        } else {
            r_buffer[x] = m_temp_color.r;
            g_buffer[x] = m_temp_color.g;
            b_buffer[x] = m_temp_color.b;
        }  
    }
    postProcess();
}


void
RoutinesRGB::savedRandomSolid(uint16_t colorCount)
{
    preProcess(eSavedRandomSolid);
    if (!(m_temp_counter % m_blink_speed)) {
        // protect from illegal arguments
        colorCount = constrain(colorCount, 1, m_color_count);
        
        chooseRandomSaved(colorCount, false);
        for (uint16_t x = 0; x < m_LED_count; x++) {
            r_buffer[x] = m_temp_color.r;
            g_buffer[x] = m_temp_color.g;
            b_buffer[x] = m_temp_color.b;
        }
        postProcess();
    }
    m_temp_counter++;
}

void
RoutinesRGB::savedRandomIndividual(uint16_t colorCount)
{
    preProcess(eSavedRandomIndividual);
    for (uint16_t x = 0; x < m_LED_count; x++) {
        // protect from illegal arguments
        colorCount = constrain(colorCount, 1, m_color_count);
        
        chooseRandomSaved(colorCount, true);
        r_buffer[x] = m_temp_color.r;
        g_buffer[x] = m_temp_color.g;
        b_buffer[x] = m_temp_color.b;
    }
    postProcess();
}


void
RoutinesRGB::savedFade(uint16_t colorCount)
{
    preProcess(eSavedFade);
    
    // protect from illegal arguments
    colorCount = constrain(colorCount, 1, m_color_count);
    
    if (m_start_next_fade) {
        m_start_next_fade = false;
        if (colorCount > 1) {
            m_fade_saved_counter = (m_fade_saved_counter + 1) % colorCount;
            m_temp_color = colors[m_fade_saved_counter];
            m_goal_color = colors[(m_fade_saved_counter + 1) % colorCount];
        } else {
            m_fade_saved_counter = 0;
            m_goal_color = colors[0];
            m_temp_color = colors[0];
        }
    }
    
    m_temp_bool = true;
    m_temp_color.r = fadeBetweenValues(m_temp_color.r, m_goal_color.r);
    m_temp_color.g = fadeBetweenValues(m_temp_color.g, m_goal_color.g);
    m_temp_color.b = fadeBetweenValues(m_temp_color.b, m_goal_color.b);
    m_start_next_fade = m_temp_bool;
    
    for (uint16_t x = 0; x < m_LED_count; x++) {
        r_buffer[x] = m_temp_color.r;
        g_buffer[x] = m_temp_color.g;
        b_buffer[x] = m_temp_color.b;
    }
    postProcess();
}


void
RoutinesRGB::savedBarSolid(uint16_t colorCount, uint8_t barSize)
{
    // protect from illegal arguments
    colorCount = constrain(colorCount, 1, m_color_count);
    // if theres a change, set up the buffer differently.
    if (colorCount != m_bar_count || barSize != m_bar_size) {
        movingBufferSetup(colorCount, barSize);
    }
    
    preProcess(eSavedBarSolid);
    
    m_temp_counter = 0;
    m_temp_index = 0;
    // set up the buffers
    for(uint16_t x = 0; x < m_LED_count; x++) {
        r_buffer[x] = colors[m_temp_index].r;
        g_buffer[x] = colors[m_temp_index].g;
        b_buffer[x] = colors[m_temp_index].b;
        m_temp_counter++;
        if (m_temp_counter == barSize) {
            m_temp_counter = 0;
            m_temp_index = (m_temp_index + 1) % colorCount;
        }
    }
    postProcess();
}


void
RoutinesRGB::savedBarMoving(uint16_t colorCount, uint8_t barSize)
{
    // protect from illegal arguments
    colorCount = constrain(colorCount, 1, m_color_count);
    // if theres a change, set up the buffer differently.
    if (colorCount != m_bar_count || barSize != m_bar_size) {
        movingBufferSetup(colorCount, barSize);
    }
    preProcess(eSavedBarMoving);
    
    for(uint16_t x = 0; x < m_LED_count - m_temp_index; x++) {
        r_buffer[x] = colors[m_temp_buffer[x + m_temp_index]].r;
        g_buffer[x] = colors[m_temp_buffer[x + m_temp_index]].g;
        b_buffer[x] = colors[m_temp_buffer[x + m_temp_index]].b; 
    }
    int z = 0;
    for (uint16_t x = m_LED_count - m_temp_index; x < m_LED_count; x++) {
        r_buffer[x] = colors[m_temp_buffer[z]].r;
        g_buffer[x] = colors[m_temp_buffer[z]].g;
        b_buffer[x] = colors[m_temp_buffer[z]].b;
        z++;
    }
    m_temp_index = (m_temp_index + 1) % m_LED_count;
    postProcess();
}


//================================================================================
// Helper Functions
//================================================================================


void
RoutinesRGB::fadeAllColorsIncrement()
{
    if (m_fade_counter < 255 / m_fade_speed) {
        m_fade_color.g = m_fade_color.g + m_fade_speed;
    } else if (m_fade_counter < 255 / m_fade_speed * 2) {
        m_fade_color.r = m_fade_color.r - m_fade_speed;
    } else if (m_fade_counter < 255 / m_fade_speed * 3) {
        m_fade_color.b = m_fade_color.b + m_fade_speed;
    } else if (m_fade_counter < 255 / m_fade_speed * 4) {
        m_fade_color.g = m_fade_color.g - m_fade_speed;
    } else if (m_fade_counter < 255 / m_fade_speed * 5) {
        m_fade_color.r = m_fade_color.r + m_fade_speed;
    } else if (m_fade_counter < 255 / m_fade_speed * 6) {
        m_fade_color.b = m_fade_color.b - m_fade_speed;
    } else {
        m_fade_counter = -1;
    }
    m_fade_counter++;
}


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
    m_bar_size =  barSize;
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
RoutinesRGB::chooseRandomSaved(uint16_t colorCount, boolean canRepeat)
{   
    uint16_t possible_saved = random(0, colorCount);
    if (!canRepeat && colorCount > 2) {
      while (possible_saved == m_temp_index) {
         possible_saved = random(0, colorCount);
      }
    }
    m_temp_index = possible_saved;
    m_temp_color = colors[m_temp_index];
}


