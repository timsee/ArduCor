# Sample Sketches for RGB LED Routines

Provides a set of sample sketches for Adafruit NeoPixels products, SeeedStudio Rainbowduino, and a single standard RGB LED. 

## <a name="toc"></a>Table of Contents

* [Supported Hardware](#hardware)
    * [Rainbowduino](#rainbowduino-hardware)
    * [Adafruit NeoPixels](#adafruit-hardware)
    * [Single RGB LED](#RGBLED-hardware)
* [Sample Sketch Usage](#sample-usage)
	* [Messaging Protocols](#messaging-protocols)
	* [Lighting Modes](#lighting-modes)
* [Generating Samples](#generated-samples)


## <a name="hardware"></a>Supported Hardware

#### <a name="rainbowduino-hardware"></a>Rainbowduino

To use the project with a Rainbowduino, connect an 8x8x8 RGB LED array or a 4x4x4 RGB LED cube to your rainbowduino.  It is recommended that you sucessfully install a Rainbowduino sample project before trying to use these sample sketches. For more information on Rainbowduino, check out their [documentation](http://www.seeedstudio.com/wiki/Rainbowduino_v3.0)

#### <a name="adafruit-hardware"></a>Adafruit NeoPixels


This project supports everything in the NeoPixels line. It is recommended that you successfully install a NeoPixels sample project before trying to use these sample sketches. For more information on NeoPixels, check out their [documentation](https://learn.adafruit.com/downloads/pdf/adafruit-neopixel-uberguide.pdf)

#### <a name="RGBLED-hardware"></a>Single RGB LED


This project supports using a single RGB LED, although certain routines lose some of their features by only having one LED. For setup, consult your RGB LED datasheet for wiring to the arduino. It will have different
behavior depending on whether its a common anode or a common cathode. To handle this, set the const `IS_COMMON_ANODE` to 1 if you are using common anode, or 0 if you are using a common cathode LED. 
	
### <a name="messaging-protocols"></a>Mesasging Protocols


The sample sketch provides a serial interface that is designed to take ASCII strings at a baud rate of 19200. The commands are a list of integers in the following format:

```
Header,param1,param2,param3,param4;
```

#### Mode Change

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     0         | 
| New Mode      | 0 - 20        |  
**Example:** `0,1;` *(Header 0, New Mode 1)* 

*Note: To find the number representation of the modes, check the number in [Lighting Modes](#lighting-modes)*

#### Set Main Color

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     1         | 
| Red           | 0 - 255       |
| Green         | 0 - 255       |
| Blue          | 0 - 255       |
**Example:** `1,255,127,0;` *(Header 1, Saved Color 3, Red 255, Green 127, Blue 0)*

*Note: In the sample sketches, this color is used for single color routines.*

#### Set color in Colors Array

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     2         | 
| Saved Color   | 0 - NUM_OF_COLORS | 
| Red           | 0 - 255       |
| Green         | 0 - 255       |
| Blue          | 0 - 255       |
**Example:** `2,3,255,127,0;` *(Header 2, Saved Color 3, Red 255, Green 127, Blue 0)*

*Note: The number of array colors is defined by the const `NUM_OF_COLORS` in the code. By default it is set to 5.*

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

#### Set Idle Timeout

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     5        | 
| Idle Timeout Minutes       | 0 - 1000      |
**Example:** `5,120;` *(Header 5, 120 Minutes)*

*Note: If no serial packet is parsed in the amount of minutes specified, the lighting mode gets set to off. If the packet `5,0;` is sent, the idle timeout is turned off and the lights will stay on indefinitely.*


### <a name="lighting-modes"></a>Sample Lighting Modes

These modes are currently implemented in the sample sketch:

| Mode | Name           |
|:---:| -------------- | 
| 0    | All LEDs Off   |  
| 1    | Constant |
| 2    | Blink    |
| 3    | Fade     | 
| 4    | Glimmer  | 
| 5    | Red            |
| 6    | Orange         |
| 7    | Yellow         |
| 8    | Green          |
| 9    | Teal           |
| 10   | Blue           |
| 11   | Purple         |
| 12   | Light Blue     |
| 13   | Pink           |
| 14   | White          |
| 15   | Random Individual |
| 16   | Random Solid |
| 17   | Fade All Colors |
| 18   | Array Glimmer |
| 19   | Array Random Individual |
| 20   | Array Random Solid |
| 21   | Array Fade  |
| 22   | Array Bars Solid |
| 23   | Array Bars Moving |

All routines that work with saved colors can take an additional optional parameter which controls how many array colors are used. The parameter must be between the 1 and `NUM_OF_COLORS`. 

### <a name="generating-samples"></a>Generating Samples

Samples are generated off of the `RGB-LED-Routines` sketch in the `src` folder of this repo. It uses the script `generate_samples.sh` which reads preprocessor directives and creates 3 versions of the sketch. Each of these versions contains only the relevant code for one of the hardware setups. To make changes to all the samples, changes need to be made the `RGB-LED-Routines` sketch and then the `generate_samples.sh` script must be ran again. To test any independent sketch from the `RGB-LED-Routines` sketch, you can set the preprocessor directives on the first few lines of the sketch. Only one define should be set to `1` at any given time. 

There is also the option for a custom sketch, which is useful if you have a complex light setup but you still want to use the master project instead of sketches for development.



