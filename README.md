
# RGB LED Routines for Arduino

Provides a set of lighting routines for Adafruit NeoPixels products, SeeedStudio Rainbowduino, and a single standard RGB LED. 


## <a name="toc"></a>Table of Contents

* [Installation](#installation)
* [Hardware Setup](#hardware)
	* [Rainbowduino](#rainbowduino-hardware)
	* [Adafruit NeoPixels](#adafruit-hardware)
	* [Single RGB LED](#RGBLED-hardware)
* [Usage](#usage)
	* [Saved Colors](#saved-colors)
	* [Serial Interface](#serial-interface)
	* [Lighting Modes](#lighting-modes)
		* [Optional Parameters](#optional-params) 
* [Contributing](#contributing)
* [License](#license)


## <a name="installation"></a>Installation


1. Download the git repository
2. Add the contents of the project's libraries folder to your [Arduino Libraries](https://www.arduino.cc/en/Hacking/Libraries).
3. Open the project, compile, and upload to your board.  

## <a name="hardware"></a>Hardware Setup

#### <a name="rainbowduino-hardware"></a>Rainbowduino

To use the project with a Rainbowduino, connect an 8x8x8 RGB LED array or a 4x4x4 RGB LED cube to your rainbowduino. After that, set up these defines:
* set `IS_RAINBOWDUINO` to 1, `IS_NEOPIXEL` to 0, and `IS_SINGLE_LED` to 0
* set LED_COUNT to 64

```
#define IS_RAINBOWDUINO  1
#define IS_NEOPIXEL      0
#define IS_SINGLE_LED    0

#define LED_COUNT        64 
```


#### <a name="adafruit-hardware"></a>Adafruit NeoPixels


To use the project with a NeoPixels product, set up these defines:
* set `IS_NEOPIXEL` to 1, `IS_RAINBOWDUINO` to 0, and `IS_SINGLE_LED` to 0
* set LED_COUNT to the number of LEDs you're using.
* set CONTROL_PIN to the pin to the pin connected to the NeoPixel

Here is an example with 16 LEDs on control pin 6: 

```
#define IS_RAINBOWDUINO  0
#define IS_NEOPIXEL      1
#define IS_SINGLE_LED    0

#define LED_COUNT        16

#define CONTROL_PIN      6  
```

#### <a name="RGBLED-hardware"></a>Single RGB LED


This project supports using a single RGB LED, although certain routines lose some of their features by only having one LED. For setup, consult your RGB LED datasheet for wiring to the arduino. Then, set up these defines: 

* set `IS_SINGLE_LED` to 1, `IS_RAINBOWDUINO` to 0, and `IS_NEOPIXEL` to 0
* set the R, G, and B pins to the pins being used to control those channels.
* set `IS_COMMON_ANODE` to 1 if you are using a common anode LED, or 0 if its a common cathode. 

Here is an example with a common anode where red is on pin 6, green is on pin 5, and blue is on pin 4:

```
#define IS_RAINBOWDUINO  0
#define IS_NEOPIXEL      0
#define IS_SINGLE_LED    1

#define R_PIN            6     // pin connected to R LED
#define G_PIN            5     // pin connected to G LED
#define B_PIN            4     // pin connected to B LED
#define IS_COMMON_ANODE  1     // 0 if common cathode, 1 if common anode
```

## <a name="usage"></a>Usage

### <a name="saved-colors"></a>Saved Colors

The sketch is designed to save a series of colors in the array `Color color[NUM_OF_COLORS];`. These colors are used by some of the lighting routines. Routines that use a single color tend to use `color[0]`. Modes that start with the word *Saved* use this set of colors.

You can change the value of `NUM_OF_COLORS` and the routines will scale to support as many colors as you want, limited only by space on your arduino. These colors are originally set in the `setup()` function, but can be changed through the serial interface. 

### <a name="serial-interface"></a>Serial Interface

The serial interface is designed to take ASCII strings at a baud rate of 19200. The commands are designed to be a list of integers in the following format:

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


#### Override with Temporary Color 

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        | 5             | 
| Red           | 0 - 255       |
| Green         | 0 - 255       |
| Blue          | 0 - 255       |
**Example:** `2,255,127,0;` *(Header 1, Red 255, Green 127, Blue 0)*

*Note: This command overrides the current mode with a glimmering color for the number of frames defined by `OVERRIDE_LENGTH`. If another override  packet comes in during this time, it will override the earlier packet. After no packets have been received and `OVERRIDE_LENGTH` has been reached, it will go back to its previous mode.*

### <a name="lighting-modes"></a>Lighting Modes

These modes are currently implemented in the sketch:

| Mode | Name           | Notes         |
|:---:| -------------- | ------------- |
| 0    | All LEDs Off   |               |
| 1    | Solid Constant | Displays `color[0]` continuously.            |
| 2    | Solid Blink    | Blinks `color[0]` on and off. Speed is controlled by `BLINK_EVERY_X`. |
| 3    | Solid Fade     | Fades `color[0]` in and out. Speed is controlled by `SOLID_FADE_SPEED`. |
| 4    | Solid Glimmer  | Displays `color[0]` with some of its LEDs randomly dimmed on each update. |
| 5    | Green          |               |
| 6    | Teal           |               |
| 7    | Blue           |               |
| 8    | Purple         |               |
| 9    | Red            |               |
| 10   | Orange         |               |
| 11   | White          |               |
| 12   | Random Individual | Each LED gets assigned a random value for each channel. |
| 13   | Random Solid | Every LED gets assigned the same random value for each channel. |
| 14   | Fade All Colors | Fades through all the colors of the rainbow. |
| 15   | Saved Glimmer | Displays `color[0]` with some of its LEDs randomly dimmed on each update and some LEDs randomly switched to other saved colors. |
| 16   | Saved Random Individual | Each LED gets assigned a random value from the set of saved colors. |
| 17   | Saved Random Solid | Every LED gets assigned a random value from the set of saved colors. |
| 18   | Saved Fade  | Fades between all saved colors. |
| 19   | Saved Bars Solid | Sets LEDs in groups of alternating colors based off of `BAR_SIZE` |
| 20   | Saved Bars Moving | Sets LEDs in groups of alternating colors based off of `BAR_SIZE` and on each frame moves each bar up by one LED to give the effect of scrolling LEDs. |

### <a name="optional-params"></a>Optional Parameters

#### Saved Bars Solid

| Parameter     | Range        | 
| ------------- | ------------- |
| Number of Bars        | 0 - `NUM_OF_COLORS`             | 

Changes the number of colors used for the Bars Solid routine. 

#### Saved Bars Moving

| Parameter     | Range        | 
| ------------- | ------------- |
| Number of Bars        | 0 - `NUM_OF_COLORS`             | 


Changes the number of colors used for the Bars Moving routine. 


## <a name="contributing"></a>Contributing

1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request.


## <a name="changelog"></a>Change Log

#### **v1.0** *October 14th, 2015*
* Initial version

#### **v1.5** *November 17th, 2015*
* Added support for *x* number of saved colors, defined by the const `NUM_OF_COLORS`
* Cleaned code and conformed closer to the [Arduino Style Guide](https://www.arduino.cc/en/Reference/StyleGuide).
* Added Idle timeout
* Added 6 new lighting routines.

## <a name="license"></a>License


MIT License, provided [here](LICENSE).
