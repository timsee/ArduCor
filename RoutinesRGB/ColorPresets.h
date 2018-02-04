/*!
 * \file ColorPresets.h
 * \version v2.1.1
 * \date January 29, 2018
 * \author Tim Seemann
 * \copyright <a href="https://github.com/timsee/RGB-LED-Routines/blob/master/LICENSE">
 *            MIT License
 *            </a>
 *
 * These color presets are stored in program memory and loaded into a buffer when accessed.
 * This makes the presets read-only, but in return, it allows them to take a much smaller hit
 * on SRAM usage. 
 *
 */
 
#include <avr/pgmspace.h>


//==========
// Water 
//==========
const PROGMEM uint8_t waterColors[]        = { 0,   0,   255,
                                               0,   25,  225,
                                               0,   0,   127,  
                                               120, 120, 255,
                                               0,   0,   255,
                                               0,   25,  225,
                                               0,   0,   127,
                                               0,   127, 127,
                                               120, 120, 255 };

//==========
// Frozen 
//==========                                   
const PROGMEM uint8_t frozenColors[]       = { 0,   127, 255,
                                               0,   127, 127,
                                               200, 0,   255,    
                                               40,  127, 40,
                                               127, 127, 127,
                                               127, 127, 255 };
           
//==========
// Snow 
//==========                  
const PROGMEM uint8_t snowColors[]         = { 255, 255, 255,
                                               127, 127, 127,
                                               200, 200, 200,
                                               0,   0,   255,
                                               0,   255, 255,
                                               0,   180, 180 };

//==========
// Cool 
//==========                                                   
const PROGMEM uint8_t coolColors[]         = { 0,   255, 0,
                                               125, 0,   255,
                                               0,   0,   255,  
                                               40,  127, 40,
                                               60,  0,   160 };
                                                   

//==========
// Warm 
//==========
const PROGMEM uint8_t warmColors[]         = { 255, 255, 0,
                                               255, 0,   0,
                                               255, 45,  0,
                                               255, 200, 0,
                                               255, 127, 0 };    

//==========
// Fire 
//==========
const PROGMEM uint8_t fireColors[]         = { 255, 70,  0,
                                               255, 20,  0,
                                               255, 80,  0,
                                               255, 5,   0,
                                               64,  6,   0,
                                               127, 127, 0,
                                               255, 60,  0,
                                               255, 45,  0,
                                               127, 127, 0 };    
                                                               
//==========
// Evil 
//==========
const PROGMEM uint8_t evilColors[]         = { 255, 0,   0,
                                               200, 0,   0,
                                               127, 0,   0,
                                               20,  0,   0,
                                               20,  0,   0,
                                               30,  0,   40,
                                               10,  0,   0,
                                               80,  0,   0 };    

//==========
// Corrosive 
//==========
const PROGMEM uint8_t corrosiveColors[]    = { 0,   255, 0,
                                               0,   200, 0,
                                               60,  180, 60,
                                               127, 135, 127,
                                               10,  255, 10 };    
    

//==========
// Poison 
//==========
const PROGMEM uint8_t poisonColors[]       = { 80,  0,   180,
                                               120, 0,   255,
                                               10,  0,   20,      
                                               25,  0,   25,
                                               60,  40,  60,
                                               120, 0,   255,
                                               80,  0,   180 };    
    
//==========
// Rose 
//==========
const PROGMEM uint8_t roseColors[]         = { 216, 30,  100,
                                               156, 62,  72,
                                               255, 245, 251,
                                               127, 127, 127,
                                               194, 30,  86,
                                               194, 30,  30 };   
  
//==========
// Pink Green
//==========
const PROGMEM uint8_t pinkGreenColors[]    = { 255, 20,  147,
                                               0,   255, 0,
                                               0,   200, 0,
                                               255, 105, 180 };   

//==========
// Red White Blue
//==========
const PROGMEM uint8_t redWhiteBlueColors[] = { 255, 255, 255,
                                               255, 0,   0,
                                               0,   0,   255,
                                               255, 255, 255 };    
       
                     
//==========
// RGB
//==========
const PROGMEM uint8_t RGBColors[]          = { 255, 0,   0,
                                               0,   255, 0,
                                               0,   0,   255 };    
                           
                     
//==========
// CMY
//==========
const PROGMEM uint8_t CMYColors[]          = { 255, 255, 0,
                                               0,   255, 255,
                                               255, 0,   255 };    
                           
//==========
// Six Colors
//==========
const PROGMEM uint8_t sixColors[]          = { 255, 0,   0,
                                               255, 255, 0,
                                               0,   255, 0,
                                               0,   255, 255,
                                               0,   0,   255,
                                               255, 0,   255 };    

                             
//==========
// Seven Colors
//==========
const PROGMEM uint8_t sevenColors[]        = { 255, 0,   0,
                                               255, 255, 0,
                                               0,   255, 0,
                                               0,   255, 255,
                                               0,    0,   255,
                                               255,  0,   255,
                                               255, 255, 255 }; 
                                            

const uint8_t* const colorPresets[] PROGMEM = { waterColors, 
                                                frozenColors,
                                                snowColors,
                                                coolColors,
                                                warmColors,
                                                fireColors,
                                                evilColors,
                                                corrosiveColors,
                                                poisonColors,
                                                roseColors,
                                                pinkGreenColors,
                                                redWhiteBlueColors,
                                                RGBColors,
                                                CMYColors,
                                                sixColors,
                                                sevenColors };
         
                                            
const PROGMEM uint8_t presetSizes []        = { 9, 
                                                6,
                                                6,
                                                5,
                                                5,
                                                9,
                                                7,
                                                5,
                                                7,
                                                6,
                                                4,
                                                4,
                                                3,
                                                3,
                                                6,
                                                7 };
         
                                                                                   
