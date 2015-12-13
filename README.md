
# RGB LED Routines for Arduino

Provides a set of lighting routines for Adafruit NeoPixels products, SeeedStudio Rainbowduino, and a single standard RGB LED. 


## <a name="toc"></a>Table of Contents

* [Installation](#installation)
* [Hardware Setup](#hardware)
	* [Rainbowduino](#rainbowduino-hardware)
	* [Adafruit NeoPixels](#adafruit-hardware)
	* [Single RGB LED](#RGBLED-hardware)
* [Library Usage](#library-usage)
	* [Single Color Routines](#single-color)
	* [Multi Color Routines](#multi-colors)
	* [Routines with Saved Colors](#saved-colors)
	* [Getters and Setters](#getters-setters)
* [Sample Sketch Usage](#sample-usage)
	* [Serial Interface](#serial-interface)
	* [Lighting Modes](#lighting-modes)
* [Contributing](#contributing)
* [License](#license)


## <a name="installation"></a>Installation


1. Download the git repository
2. Copy the RoutinesRGB directory into your [Arduino Libraries directory](https://www.arduino.cc/en/Hacking/Libraries).
3. If any are missing, copy the missing libraries from the [libraries directory](libraries) into the same Arduino Libraries directory used in step 2.
4. Open the project, compile, and upload to your board.  

## <a name="hardware"></a>Hardware Setup

#### <a name="rainbowduino-hardware"></a>Rainbowduino

To use the project with a Rainbowduino, connect an 8x8x8 RGB LED array or a 4x4x4 RGB LED cube to your rainbowduino. After that, set up these constants:
* set `IS_RAINBOWDUINO` to 1, `IS_NEOPIXEL` to 0, and `IS_SINGLE_LED` to 0
* set LED_COUNT to 64

```
const int IS_RAINBOWDUINO = 1;
const int IS_NEOPIXEL     = 0;
const int IS_SINGLE_LED   = 0;

const int LED_COUNT       = 64; 
```


#### <a name="adafruit-hardware"></a>Adafruit NeoPixels


To use the project with a NeoPixels product, set up these constants:
* set `IS_NEOPIXEL` to 1, `IS_RAINBOWDUINO` to 0, and `IS_SINGLE_LED` to 0
* set LED_COUNT to the number of LEDs you're using.
* set CONTROL_PIN to the pin to the pin connected to the NeoPixel

Here is an example with 16 LEDs on control pin 6: 

```
const int IS_RAINBOWDUINO = 0;
const int IS_NEOPIXEL     = 1;
const int IS_SINGLE_LED   = 0;

const int LED_COUNT       = 16;

const int CONTROL_PIN     = 6;  
```

#### <a name="RGBLED-hardware"></a>Single RGB LED


This project supports using a single RGB LED, although certain routines lose some of their features by only having one LED. For setup, consult your RGB LED datasheet for wiring to the arduino. Then, set up these constants: 

* set `IS_SINGLE_LED` to 1, `IS_RAINBOWDUINO` to 0, and `IS_NEOPIXEL` to 0
* set the R, G, and B pins to the pins being used to control those channels.
* set `IS_COMMON_ANODE` to 1 if you are using a common anode LED, or 0 if its a common cathode. 

Here is an example with a common anode where red is on pin 6, green is on pin 5, and blue is on pin 4:

```
const int IS_RAINBOWDUINO = 0;
const int IS_NEOPIXEL     = 0;
const int IS_SINGLE_LED   = 1;

const int R_PIN           = 6;     // pin connected to R LED
const int G_PIN           = 5;     // pin connected to G LED
const int B_PIN           = 4;     // pin connected to B LED
const int IS_COMMON_ANODE = 1;     // 0 if common cathode, 1 if common anode
```

## <a name="library-usage"></a>Library Usage

### <a name="single-color"></a>Single Color Routines

These routines take an R, G, and B values their parameters:

* Solid
* Blink
* Fade
* Glimmer

### <a name="multi-colors"></a>Multi Color Routines

These routines use all possible colors and require no additional parameters:

* Random Solid
* Random Individual 
* Fade All Colors 

### <a name="saved-colors"></a>Routines with Saved Colors

These routines use the colors saved in memory to execute the routine. These routines take a parameter that determines how many colors to use:

* Saved Glimmer
* Saved Random Individual
* Saved Random Solid
* Saved Fade
* Saved Bars Solid
* Saved Bars Moving

### <a name="getters-setters"></a>Getters and Setters

#### Setters

* setColor: takes an index and RGB values. Sets a color in the "saved colors" array, at the given index. 
* setBrightness: sets the brightness between 0 and 100, with 0 being off and 100 being full power.
* setFadeSpeed: sets the speed parameter that routines that fade utilize, must be set between 1 and 100, with 1 being the slowest possible fade. 
* setBlinkSpeed: Sets how many updates it takes for a routine that switches between solid colors to update. Takes a value between 1 and 255, with 1 causing it to change on every frame. 


#### Getters

* getColor: returns the color at the given index.
* getR: returns the r value of the buffer at the given index. 
* getG: returns the g value of the buffer at the given index.
* getB: returns the b value of the buffer at the given index.

## <a name="sample-usage"></a>Sample Sketch Usage

### <a name="serial-interface"></a>Serial Interface

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

#### Set Saved Colors

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     1         | 
| Saved Color   | 0 - NUM_OF_COLORS | 
| Red           | 0 - 255       |
| Green         | 0 - 255       |
| Blue          | 0 - 255       |
**Example:** `1,3,255,127,0;` *(Header 1, Saved Color 3, Red 255, Green 127, Blue 0)*

*Note: The number of saved is defined by the const `NUM_OF_COLORS` in the code. By default it is set to 5.*

#### Set Brightness

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     2         | 
| Brightness %  | 0 - 100       |
**Example:** `2,90;` *(Header 2, 90% brightness)*

#### Set Delay 

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     3         | 
| Delay Milliseconds      | 1 - 1000      |
**Example:** `3,50;` *(Header 3, 500 msec updates)*

*Note: This delay is defined as the delay value multiplied by 10 msec.*

#### Set Idle Timeout

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     4        | 
| Idle Timeout Minutes       | 0 - 1000      |
**Example:** `4,120;` *(Header 4, 120 Minutes)*

*Note: If no serial packet is parsed in the amount of minutes specified, the lighting mode gets set to off. If the packet `4,0;` is sent, the idle timeout is turned off and the lights will stay on indefinitely.*


### <a name="lighting-modes"></a>Sample Lighting Modes

These modes are currently implemented in the sample sketch:

| Mode | Name           | Notes         |
|:---:| -------------- | ------------- |
| 0    | All LEDs Off   |               |
| 1    | Constant | Displays `color[0]` continuously.            |
| 2    | Blink    | Blinks `color[0]` on and off. |
| 3    | Fade     | Fades `color[0]` in and out.  |
| 4    | Glimmer  | Displays `color[0]` with some of its LEDs randomly dimmed on each update. |
| 5    | Red            |               |
| 6    | Orange         |               |
| 7    | Yellow         |               |
| 8    | Green          |               |
| 9    | Teal           |               |
| 10   | Blue           |               |
| 11   | Purple         |               |
| 12   | Light Blue     |               |
| 13   | Pink           |               |
| 14   | White          |               |
| 15   | Random Individual | Each LED gets assigned a random value for each channel. |
| 16   | Random Solid | Every LED gets assigned the same random value for each channel. |
| 17   | Fade All Colors | Fades through all the colors of the rainbow. |
| 18   | Saved Glimmer | Displays `color[0]` with some of its LEDs randomly dimmed on each update and some LEDs randomly switched to other saved colors. |
| 19   | Saved Random Individual | Each LED gets assigned a random value from the set of saved colors. |
| 20   | Saved Random Solid | Every LED gets assigned a random value from the set of saved colors. |
| 21   | Saved Fade  | Fades between all saved colors. |
| 22   | Saved Bars Solid | Sets LEDs in groups of alternating colors based off of `BAR_SIZE` |
| 23   | Saved Bars Moving | Sets LEDs in groups of alternating colors based off of `BAR_SIZE` and on each frame moves each bar up by one LED to give the effect of scrolling LEDs. |

All routines that work with saved colors can take an additional optional parameter which controls how many saved colors are used. The parameter must be between the 1 and `NUM_OF_COLORS`. 

## <a name="contributing"></a>Contributing

1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request.


## <a name="changelog"></a>Change Log

### **v1.0** 
* Initial version

### **v1.5** 
* Added support for *x* number of saved colors, defined by the const `NUM_OF_COLORS`
* Cleaned code and conformed closer to the [Arduino Style Guide](https://www.arduino.cc/en/Reference/StyleGuide).
* Added Idle timeout
* Added 6 new lighting routines.

### **v1.7** 
* Refactored the lighting routines into their ownlibrary.
* Cleaned up the API.
* Reduced memory usage. 

## <a name="license"></a>License

MIT License, provided [here](LICENSE).
