
/*!
 * 
 * ArduCor
 * Sample Sketch
 *
 * Supports SeeedStudio Rainbowduino projects.
 * 
 * Date: May 13, 2018
 * Github repository: http://www.github.com/timsee/ArduCor
 * License: MIT-License, LICENSE provided in root of git repo
 */
#include <ArduCor.h>

#include <Rainbowduino.h>

//================================================================================
// Settings
//================================================================================

const int  LED_COUNT         = 64;     // the number of LEDs controlled by your Rainbowduino

//=======================
// ArduCor Setup
//=======================
// Library used to generate the RGB LED routines.
ArduCor routines = ArduCor(LED_COUNT);


//================================================================================
// Setup and Loop
//================================================================================

void setup()
{
  // initialize the hardware
  Rb.init();

  // choose the default color for the single
  // color routines. This can be changed at any time.
  // and its set it to green in sample routines.
  // If its not set, it defaults to a faint orange.
  routines.setMainColor(0, 255, 0);

  // This sets how bright the LEDs shine.It expects a value between
  // 0 and 100. 0 means off, 100 means full brightness.
  routines.brightness(50);
}

void loop()
{
  /*!
   * Each time a routines function is called, it updates the LEDs in the ArduCor library.
   * This loop continually updates the ArduCor library, applies the brightness settings 
   * from ArduCor, then updates the lighting hardware to show the new LED values.
   */
  
  // the simple sample shows both a single and a multi color routine, 
  // set this flag to switch between them.
  bool useMultiColorRoutine = false;
  // For the glimmer routine, a parameter determines how many LEDs get the "glimmer" effect.
  const byte glimmerPercent = 15;
  if (useMultiColorRoutine) {
      // if you're using the multi color routine, it'll fade between colors in the 
      // fire palette.
      routines.multiGlimmer(eFire,  
                            glimmerPercent); // percent of pixels to glimmer
  } else {
     routines.singleGlimmer(routines.mainColor().red, 
                            routines.mainColor().green, 
                            routines.mainColor().blue,
                            glimmerPercent); // percent of pixels to glimmer
  }
     
  routines.applyBrightness(); 
  // updates the LED hardware with the values in the routines object.
  updateLEDs(); 
  // delay for half a second
  delay(500);
}


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

