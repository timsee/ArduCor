# Sample Sketches for RGB LED Routines

The sample sketches for Adafruit NeoPixels, SeeedStudio Rainbowduino, and a single RGB LED. 

## <a name="toc"></a>Table of Contents

* [Supported Hardware](#hardware)
    * [Rainbowduino](#rainbowduino-hardware)
    * [Adafruit NeoPixels](#adafruit-hardware)
    * [Single RGB LED](#RGBLED-hardware)
* [Sample Sketch Usage](#sample-usage)
    * [ASCII Communication](#ascii-communication)
    * [Lighting Protocols](https://timsee.github.io/RGB-LED-Routines/RoutinesRGB/html/a00003.html)
* [Generating Samples](#generated-samples)

## <a name="hardware"></a>Supported Hardware

#### <a name="rainbowduino-hardware"></a>Rainbowduino

To use the project with a Rainbowduino, connect an 8x8x8 RGB LED array or a 4x4x4 RGB LED cube to your rainbowduino.  It is recommended that you sucessfully install a Rainbowduino sample project before trying to use our sample sketch. For more information on Rainbowduino, check out their [documentation](http://www.seeedstudio.com/wiki/Rainbowduino_v3.0).

#### <a name="adafruit-hardware"></a>Adafruit NeoPixels

This project supports everything in the NeoPixels line. It is recommended that you successfully install a NeoPixels sample project before trying to use our sample sketch. For more information on NeoPixels, check out their [documentation](https://learn.adafruit.com/downloads/pdf/adafruit-neopixel-uberguide.pdf).

#### <a name="RGBLED-hardware"></a>Single RGB LED


This project supports using a single RGB LED, although certain routines lose some of their features by only having one LED. For setup, consult your RGB LED datasheet for wiring to the arduino. It will have different
behavior depending on whether its a common anode or a common cathode. To handle this, set the const `IS_COMMON_ANODE` to 1 if you are using common anode, or 0 if you are using a common cathode LED. 

## <a name="sample-usage"></a>Sample Sketch Usage

### <a name="ascii-communication"></a>ASCII Communication


The sample sketches provide a serial interface that uses ASCII strings at a baud rate of 19200. The commands are a list of integers in the following format:

```
Header,param1,param2,param3,param4;
```

#### Lighting Routine Change

| Parameter         | Values        | 
| ----------------- | ------------- |
| Header            |     0         | 
| New Routine       | (ELightingRoutine)0 - 10  |  
| Color Group (Optional) | (EColorGroup)0 - 17        |  

**Example:** `0,1;` *(Header 0, New Routine 1)*
 `0,4,6;` *(Header 0, New Routine 4, New Color Group 6)* 

*Note: By default, it will use its last EColorGroup for multi color routines if no color group is provided. To find a description and number representation of ELightingRoutine and EColorGroup, check out the [Lighting Protocols](https://timsee.github.io/RGB-LED-Routines/RoutinesRGB/html/a00003.html). *

#### Set Color for Single Color Routines

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     1         | 
| Red           | 0 - 255       |
| Green         | 0 - 255       |
| Blue          | 0 - 255       |
**Example:** `1,255,127,0;` *(Header 1, Saved Color 3, Red 255, Green 127, Blue 0)*


#### Set Color in Custom Color Array

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     2         | 
| Color Index   | 0 - 10 | 
| Red           | 0 - 255       |
| Green         | 0 - 255       |
| Blue          | 0 - 255       |
**Example:** `2,3,255,127,0;` *(Header 2, Saved Color 3, Red 255, Green 127, Blue 0)*

*Note: The Color Index must be smaller than the size of the custom color array, which is currently 10. These can be used in multi color routines by using the EColorGroup `eCustom`*

#### Set Brightness

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     3         | 
| Brightness %  | 0 - 100       |
**Example:** `3,90;` *(Header 3, 90% brightness)*

#### Set Speed 

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     4         | 
| FPS * 100     | 1 - 2000      |
**Example:** `4,500;` *(Header 4, 5 FPS)*

*Note: The value sent is the desired FPS * 100. To do 1 FPS, send 100, to do 10 FPS, send 1000.*

#### Set Custom Color Count

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     5         | 
| Count *       | 1 - 10        |
**Example:** `5,3;` *(Header 5, 3 colors)*

*Note: This setting controls the number of colors used for multi color routines using the custom color array.*

#### Set Idle Timeout

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     6        | 
| Idle Timeout Minutes       | 0 - 1000      |
**Example:** `6,120;` *(Header 6, 120 Minutes)*

*Note: If no serial packet is parsed in the amount of minutes specified, the lighting mode gets set to off. If the packet `6,0;` is sent, the idle timeout is turned off and the lights will stay on indefinitely.*

#### Reset to Default Values

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     7         | 
| Check 1       |     42        |
| Check 2       |     71        |
**Example:** `7,42,71;` *(Header 7, Check 1, Check 2)*

*Note: This message contains two extra parameters to make it harder for it to be triggered accidentally by a corrupted packet.*

### <a name="generating-samples"></a>Generating Samples

Samples are generated off of the `RGB-LED-Routines` sketch in the `src` folder of this repo. It uses the script `generate_samples.sh` which reads preprocessor directives and creates 3 versions of the sketch. It does this since the arduino IDE doesn't always play nicely with preprocesor directives and it will outright ignore them for includes, making the sample size larger than it needs to be. Each of the versions generated contains only the relevant code for one of the hardware setups. To make changes to all the samples, changes need to be made the `RGB-LED-Routines` sketch and then the `generate_samples.sh` script must be ran again. To test any independent sketch from the `RGB-LED-Routines` sketch, you can set the preprocessor directives on the first few lines of the sketch. Only one define should be set to `1` at any given time. 

There is also the option for a custom sketch, which is useful if you have a complex light setup but you still want to use the master project for development.



