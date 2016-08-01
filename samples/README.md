# Sample Sketches for RGB LED Routines

The sample sketches for Adafruit NeoPixels, SeeedStudio Rainbowduino, and a single RGB LED. 

The samples are split into three main folders:

* `arduino` contains samples that should run on any arduino.
* `yun/http` contains samples that work only on arduino yuns, but require no extra configuration.
* `yun/udp` contains samples that work only on arduino yuns and require extra configuration.

For documentation for the Yun based samples, [check here](yun).

## <a name="toc"></a>Table of Contents

* [Supported Hardware](#hardware)
    * [Rainbowduino](#rainbowduino-hardware)
    * [Adafruit NeoPixels](#adafruit-hardware)
    * [Single RGB LED](#RGBLED-hardware)
* [Sample Sketch Usage](#sample-usage)
    * [Control Packets](#control-packets)
    * [State Update Packet](#state-update)
    * [Discovery Packet](#discovery)
    * [Multi Serial Sample](#multi-sample)
    * [Lighting Protocols](https://timsee.github.io/RGB-LED-Routines/RoutinesRGB/html/a00003.html)
* [Generating Samples](#generated-samples)

## <a name="hardware"></a>Supported Hardware

#### <a name="rainbowduino-hardware"></a>Rainbowduino

To use the project with a Rainbowduino, connect an 8x8 RGB LED array or a 4x4x4 RGB LED cube to your rainbowduino.  It is recommended that you sucessfully install a Rainbowduino sample project before trying to use our sample sketch. For more information on Rainbowduino, check out their [documentation](http://www.seeedstudio.com/wiki/Rainbowduino_v3.0).

#### <a name="adafruit-hardware"></a>Adafruit NeoPixels

This project has been tested with NeoPixel Rings, NeoMatrices, and Light Strips. It is recommended that you successfully install a NeoPixels sample project before trying to use our sample sketch. For more information on NeoPixels, check out their [documentation](https://learn.adafruit.com/downloads/pdf/adafruit-neopixel-uberguide.pdf).

#### <a name="RGBLED-hardware"></a>Single RGB LED

This project supports using a single RGB LED, although certain routines lose some of their features by only having one LED. For setup, consult your RGB LED datasheet for wiring to the arduino. It will have different
behavior depending on whether its a common anode or a common cathode. To handle this, set the const `IS_COMMON_ANODE` to 1 if you are using common anode, or 0 if you are using a common cathode LED. 

## <a name="sample-usage"></a>Sample Sketch Usage

### <a name="control-packets"></a>Control Packets


The sample sketches provide a serial interface that uses ASCII strings at a baud rate of 19200. The commands are a list of integers in the following format:

```
header,device_index,param2,param3,param4;
```

The header and the device index are required. Upon receiving a valid message, the LEDs will update based on the content of the message, then echo the message back to the sender. 

#### Device Index

The second argument in a message is always a device index. This value determines which device will use the rest of the contents of the message. This is only used to its potential in the [Multi Serial Sample](#multi-sample), as its the only sample that has more than one device connected to a single arduino. If the device_index is set to 0, all connected devices will be updated by the message. If its set to any other value, only the device that has the same index as the one in the mesasge will use the message. 

#### Lighting Routine Change

| Parameter         | Values        | 
| ----------------- | ------------- |
| Header            |     0         | 
| New Routine       | (ELightingRoutine)0 - 14  | 
| Color Group (Optional) | (EColorGroup)0 - 17        | 

**Example:** `0,0,1;` *(Header 0, Device Index 0, New Routine 1)*
 `0,0,4,6;` *(Header 0, Device Index 0, New Routine 4, New Color Group 6)* 

*Note: By default, it will use its last EColorGroup for multi color routines if no color group is provided. To find a description and number representation of ELightingRoutine and EColorGroup, check out the* [Lighting Protocols](https://timsee.github.io/RGB-LED-Routines/RoutinesRGB/html/a00003.html).

#### Set Color for Single Color Routines

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     1         | 
| Red           | 0 - 255       |
| Green         | 0 - 255       |
| Blue          | 0 - 255       |
**Example:** `1,0,255,127,0;` *(Header 1, Device Index 0, Red 255, Green 127, Blue 0)*


#### Set Color in Custom Color Array

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     2         | 
| Color Index   | 0 - 10        | 
| Red           | 0 - 255       |
| Green         | 0 - 255       |
| Blue          | 0 - 255       |
**Example:** `2,0,3,255,127,0;` *(Header 2, Device Index 0 Saved Color 3, Red 255, Green 127, Blue 0)*

*Note: The Color Index must be smaller than the size of the custom color array, which is currently 10. These can be used in multi color routines by using the EColorGroup `eCustom`*

#### Set Brightness

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     3         | 
| Brightness %  | 0 - 100       |
**Example:** `3,0,90;` *(Header 3, Device Index 0, 90% brightness)*

#### Set Speed 

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     4         | 
| Desired FPS * 100     | 1 - 2000      |
**Example:** `4,0,500;` *(Header 4, Device Index 0, 5 FPS)*

*Note: The value sent is the desired FPS * 100. To do 1 FPS, send 100, to do 10 FPS, send 1000. This only sets the desired FPS. When the FPS is very low, it will be close to accurate. An extremely fast FPS will be limited by the hardware being used, the number of LEDs, and other factors.  *

#### Set Custom Color Count

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     5         | 
| Count *       | 2 - 10        |
**Example:** `5,0,3;` *(Header 5, Device Index 0, 3 colors)*

*Note: This setting controls the number of colors used for multi color routines using the custom color array.*

#### Set Idle Timeout

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     6        | 
| Idle Timeout Minutes       | 0 - 1000      |
**Example:** `6,0,120;` *(Header 6, Device Index 0, 120 Minutes)*

*Note: If no serial packet is parsed in the amount of minutes specified, the lighting mode gets set to off. If the packet `6,0,0;` is sent, the idle timeout is turned off and the lights will stay on indefinitely.*

#### Reset to Default Values

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     8         | 
| Check 1       |     42        |
| Check 2       |     71        |
**Example:** `8,42,71;` *(Header 8, Check 1, Check 2)*

*Note: This message contains two extra parameters to make it harder for it to be triggered accidentally by a corrupted packet.*

### <a name="state-update"></a>State Update Packet

| Parameter     | Values        | 
| ------------- | ------------- |
| Header        |     7       | 

**Example:** `7;` *(Header 7)*

Sending a state update gives 

The packet is formatted as:

```
$stateUpdate,$isOn,$isReachable,$red, $green,$blue,$routine,$colorGroup,$brightness
```
 
| Parameter        | Range        |  Description |
| -------------        | ------------- |  ------------- |
| stateUpdate    |     7            |                    |
| isOn              |     0 or 1     |     0 if the current routine is eOff, 1 otherwise             |
| isReachable    |  0 or 1            |  Only 1 if it is expected to be connected but it is not connected to the controller. Used by [Corluma](https://github.com/timsee/Corluma)                    |
| red, green, blue    |    0  - 255            |     Color used by single color routines                |
| routine    |    0 - 14            |      Current lighting routine          |
| colorGroup      |    0 - 17            |      Current color group         |
| brightness          |    0 - 100          |     Brightness setting of the RoutinesRGB library         |


### <a name="discovery"></a>Discovery Packet

Sending the message `DISCOVERY_PACKET` to any of the samples will cause the sample to send a message back in the format of:

```
DISCOVERY_PACKET,$numOfDevices,$stateUpdatePacket
```
where `$numOfDevices` is equal to the number of devices connected to this particular arduino. For nearly every sample, this number will be 1, so the packet sent from the arduino will be `DISCOVERY_PACKET,1`. For the Multi hardware sample, this number will be 3. This is an easy way to check whether or not the IP Address or Serial port that you are connecting to currently connects you to an Arduino running one of these samples. Appended to a response packet is also the current [stateUpdatePacket](#state-update). 

A successful discovery call and response is not required for the samples to work. They are useful in projects like [Corluma](https://github.com/timsee/Corluma), so they are left in the samples. 

### <a name="multi-sample"></a>Multi Serial Sample

The Multi Serial Sample is an example of how to use the device index in the control packets to control multiple sets of LEDs from one arduino. This sample that uses serial communication and two RoutinesRGB objects to control two halves of a Neopixels Light Strip separately. It also repeats its packets to a Rainbowduino connected over serial.

To recreate this sample, you'll also need to upload the Rainbowduino Serial Sample to a Rainbowduino. Before uploading this sample, you should change its hardware index to 3. 

### <a name="generating-samples"></a>Generating Samples

Samples are generated off of the `RGB-LED-Routines` sketch in the `src` folder of this repo. It uses the script `generate_samples.sh` which reads preprocessor directives and creates 3 versions of the sketch.

To make changes to all the samples, edit the sketch in `src/RGB-LED-Routines` and then run the `generate_samples.sh` script.



