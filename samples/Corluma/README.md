# Corluma Sketches for ArduCor

The sample sketches for Adafruit NeoPixels, SeeedStudio Rainbowduino, and a single RGB LED. These implement [Corluma's](https://www.github.com/timsee/Corluma) messaging protocol, so they can communicate with a Corluma application.

The samples are split into four main folders:

* `arduino` contains samples that should run on any arduino.
* `yun/http` contains samples that work only on arduino yuns, but require no extra configuration.
* `yun/udp` contains samples that work only on arduino yuns and require extra configuration.
* `server` contains a python script to run a server that takes wireless packets and sends them over serial to an arduino. This allows you to control lights wirelessly using UDP packets.

For documentation for the Yun based samples, [check here](yun).

For a guide on how to use the server sample to control the arduino projects wirelessly, [check here](server).

## <a name="toc"></a>Table of Contents

* [Supported Hardware](#hardware)
    * [Rainbowduino](#rainbowduino-hardware)
    * [Adafruit NeoPixels](#adafruit-hardware)
    * [Single RGB LED](#RGBLED-hardware)
* [Sample Sketch Usage](#sample-usage)
    * [Control Packets](#control-packets)
    * [State Update Packet](#state-update)
    * [Discovery Packet](#discovery)
    * [Cyclic Redundancy Check](#crc)
    * [Multi Serial Sample](#multi-sample)
    * [Lighting Protocols](https://timsee.github.io/ArduCor/ArduCor/html/a00011.html)
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


The sample sketches provide a serial interface that uses ASCII strings at a baud rate of 9600. The commands are a list of integers in the following format:

```
header,device_index,param2,param3,param4&
```

The header and the device index are required. The `&` is used to mark the end of a message. For serial communication, a `;` is needed at the end of a group of messages. For UDP and HTTP, the end of their response payloads is used as the end of the group of messages.

Upon receiving a valid packet, the Arduino code loops through all the messages, updates its based on the content of each message, updates the LEDs based on its new states, then echoes the message back to the sender.

#### Device Index

The second argument in a message is always a device index. This value determines which device will use the rest of the contents of the message. This is only used to its potential in the [Multi Serial Sample](#multi-sample), as its the only sample that has more than one device connected to a single arduino. If the device_index is set to 0, all connected devices will be updated by the message. If its set to any other value, only the device that has the same index as the one in the mesasge will use the message.

#### On/Off Change

| Parameter         | Values        |
| ----------------- | ------------- |
| Header            |     0          |
| Off or On        | 0 or 1 |

**Example:** `0,0,1&` *(Header 1, Device Index 0,  Turn On)*
`0,0,0&` *(Header 1, Device Index 0, Turn Off)*

####  Routine Change
Routine changes are the most complex packets, as they can have a variety of parameters based off of their routine parameter.

#### Single Light Routines
| Parameter         | Values        |
| ----------------- | ------------- |
| Header            |     1          |
| New Routine       | (ERoutine)0 - 5  |
| Red       | 0 - 255  |
| Green    | 0 - 255  |
| Blue      | 0 - 255  |
| Speed (required for everything but eSolid) |   0 - 200 |
| Extra Parameter (required for eSingleGlimmer, eSingleFade, eSingleSawtoothFade) |   N/A  |

The extra parameter is required for eSingleGlimmer, eSingleFade, and eSingleSawtoothFade:
* *eSingleGlimmer*: between 0 - 100. Determines what percent LEDs to dim to produce a glimmering effect.
* *eFade*: either 0 or 1. 0 is a linear fade in and out, 1 is a sine fade in and out.
* *eSawtoothFade*: either 0 or 1. 0 is a fade in, 1 is a fade out.


**Example:** `1,0,1,255,0,0&` *(Header 1, Device Index 0, New Routine 1, Red 255, Green 0, Blue 0)*
`1,0,3,0,255,0,100,20&` *(Header 1, Device Index 0, New Routine 1, Red 0, Green 255, Blue 0, Speed 100, Extra Parameter 20)*

#### Multi Light Routines
| Parameter         | Values        |
| ----------------- | ------------- |
| Header            |     1          |
| New Routine       | (ERoutine)6 - 10  |
| Palette | (EPalette)0 - 16       |  
| Speed  |   0 - 200 |
| Extra Parameter (required for eMultiGlimmer, eMultiBars) |   N/A  |

The extra parameter is required for eMultiGlimmer and eMultiBars:
* *eMultiGlimmer*: between 0 - 100. Determines what percent LEDs to dim to produce a glimmering effect.
* *eMultiBars*:  between 0 - 100. Determines how large each bar of colors should be.

**Example:** `1,0,7,3&` *(Header 1, Device Index 0, New Routine 7, Palette 3)*
`1,0,6,5,20&` *(Header 1, Device Index 0, New Routine 6, Palette 5, Extra Parameter 20)*

#### Speed Parameter

The speed parameter is required for every routine except eSingleSolid, since they all change over time. The parameter uses values between 0 and 200. Each unit represents between 10 and 15 milliseconds depending on the specific arduino and its load. A value of 0 pauses the routine in its current state. A value or 1 runs the routines as slow as they can go. A value of 200 makes the routines go as fast as they can.

#### Set Color in Custom Color Array

| Parameter     | Values        |
| ------------- | ------------- |
| Header        |     2         |
| Color Index   | 0 - 10        |
| Red           | 0 - 255       |
| Green         | 0 - 255       |
| Blue          | 0 - 255       |
**Example:** `2,0,3,255,127,0&` *(Header 2, Device Index 0 Saved Color 3, Red 255, Green 127, Blue 0)*

*Note: The Color Index must be smaller than the size of the custom color array, which is currently 10. These can be used in multi color routines by using the EColorGroup `eCustom`*

#### Set Palette Brightness

| Parameter     | Values        |
| ------------- | ------------- |
| Header        |     3         |
| Brightness %  | 0 - 100       |
**Example:** `3,0,90&` *(Header 4, Device Index 0, 90% brightness)*

*Note: This brightness setting only impacts multi color routines that use palettes. For single color routines, brightness should be encoded into the RGB values used. For example, to show red at 50% brightness, set the light to R=127, G=0, B=0.*

#### Set Custom Color Count

| Parameter     | Values        |
| ------------- | ------------- |
| Header        |     4         |
| Count         | 2 - 10        |
**Example:** `4,0,3&` *(Header 4, Device Index 0, 3 colors)*

*Note: This setting controls the number of colors used for multi color routines using the custom color array.*

#### Set Idle Timeout

| Parameter     | Values        |
| ------------- | ------------- |
| Header        |     5        |
| Idle Timeout Minutes       | 0 - 1000      |
**Example:** `5,0,120&` *(Header 5, Device Index 0, 120 Minutes)*

*Note: If no serial packet is parsed in the amount of minutes specified, the lighting mode gets set to off. If the packet `5,0,0&;` is sent, the idle timeout is turned off and the lights will stay on indefinitely.*

### <a name="state-update"></a>State Update Packet

| Parameter     | Values        |
| ------------- | ------------- |
| Header        |     6       |

**Example:** `6&` *(Header 6)*

Sending a state update gives

The packet is formatted as:

```
$stateUpdate,$isOn,$isReachable,$red,$green,$blue,$routine,$palette,$brightness,$speed,$idleTimeout,$minutesUntilTimeout&
```

| Parameter        | Range        |  Description |
| -------------        | ------------- |  ------------- |
| stateUpdate    |     6            |                    |
| hardwareIndex    |     1 to maxHardwareIndex            |   index of light that the stateUpdate maps to, for an arduino with a single light this is always 1.                  |
| isOn              |     0 or 1     |     0 if the current routine is eOff, 1 otherwise             |
| isReachable    |     1            |  Only 1 if it is expected to be connected but it is not connected to the controller. Used by [Corluma](https://github.com/timsee/Corluma)                    |
| red, green, blue    |    0  - 255            |     Color used by single color routines                |
| routine    |    0 - 10            |      Current  routine          |
| palette      |    0 - 16            |      Current  palette       |
| brightness          |    0 - 100          |     Brightness setting of the ArduCor library         |
| speed          |    0 - 200          |     Speed of light updates, 0 is paused, 1 is slowest, 200 is fastest      |
| idleTimeout          |    0 - 1000          |     0 to turn off, all other numbers are number of minutes until timeout     |
| minutesUntilTimeout          |    0 - 1000          |     0 if off, all other numbers are number of minutes until timeout    |

**Example:**`6,1,1,1,255,127,0,3,1,80,200,120,60&` (hardwareIndex 1,isOn,isReachable,R:255,G:127,B:0,Routine 3,Palette 1,brightness 80,speed 120,Idle Timeout 120,minutesUntilTimeout 60)

### <a name="custom-array-update"></a>Custom Array State Update Packet

| Parameter     | Values        |
| ------------- | ------------- |
| Header        |     7       |

**Example:** `7&` *(Header 7)*

Sending a state update gives

The packet is formatted as:

```
$customArrayStateUpdate,$count,$index,$red,$green,$blue...,$index,$red,$green,$blue&
```

| Parameter              | Range         |  Description   |
| -------------          | ------------- |  ------------- |
| customArrayStateUpdate |     7         |                |
| count                  |     2 - 10    |  Custom color count             |
| red      |    0  - 255   |   Red value for index |
| green    |    0  - 255   |   Green value for index |
| blue     |    0  - 255   |   Blue value for index |

The `$count` parameter denotes how many times the `,$index,$red,$green,$blue` section of the packet will repeat. Only the custom colors with indices less than the custom color count are sent during an update request.

### <a name="discovery"></a>Discovery Packet

Sending the message `DISCOVERY_PACKET` to any of the samples will cause the sample to send a message back in the format of:

```
DISCOVERY_PACKET,$majorAPI,$minorAPI,$usingCRC,$capabilities,$maxPacketSize,$numOfDevices,@$name,$hardwareType,$productType&
```

| Parameter     | Range         |  Description   |
| ------------- | ------------- |  ------------- |
| majorAPI      |    2     |  major version of API and messaging protocol  |
| minorAPI      |     0 - 10    |  minor version of API and messaging protocol  |
| usingCRC      |     0 - 1     |  1 if all packets require a CRC, 0 if skipped*  |
| capabilities      |     0 - 1     |  0 if just arduino, 1 if arduino controlled by Raspberry Pi |
| maxPacketSize |     1 - 500  |  max number of characters accepted in a single message        |
| numOfDevices  |     1 - 20    |  Number of RGB devices connected to arduino    |
| name  |    N/A    |  A hardcoded identifier of up to 16 characters  |
| hardwareType  |    0 -  4   |  An enum denoting the type of hardware (light strip, light cube, etc.)  |
| productType   |    0 -  2   |  An enum denoting the type of product (Neopixels, Rainbowduino, LED RGB, etc.)  |


**Example:** `DISCOVERY_PACKET,3,3,0,0,200,1@Cool Light,1,1&` *(v3.3,no CRC,only arduino,max packet size of 200,1 device@named "Cool Light",hardware type 1,product type 1)*

* *NOTE: even if CRC is on, discovery packets do not require or send out a CRC!*

Discovery packets are used both as a way to check if an arduino is running a sketch with the proper messaging protocol and to set up the client sending messages to the arduino. An API level is provided to allow applications to know the exact features and messaging protocol of the light controller. The major API level is incremented when theres a significant change and previous protocols will no longer work. A minor API level is incremented when most messages will still work, but new protocols are added, or messages are switched around, or any other minor change was made.
This is an easy way to check whether or not the IP Address or Serial port that you are connecting to currently connects you to an Arduino running one of these samples.
A successful discovery call and response is not required for the samples to work, although it is recommended as its lets the client know whether or not to use CRC with packets sent. For a C++ project that can parse and send messages using this protocol, check out [Corluma](https://github.com/timsee/Corluma).

### <a name="name"></a>Naming The Lights

In order to make lights a bit easier to idenifty in other applications, there is the option to hardcode a name that is sent with the light's info at the end of the discovery packet. This name is defaulted to "MyLights" but can be changed to anything as long as it fits this criteria:
* Is 16 characters or less
* Does not contain `&`, `,`, `;`,  `@`, or `#`

### <a name="crc"></a> Cyclic Redundancy Check
A [cylic redundancy check](https://en.wikipedia.org/wiki/Cyclic_redundancy_check) is an optional setting for the sample code messing protocols. It is a value appended to the end of the packet that helps detect whether there were any changes to the raw data of the packet.
The CRC packet is formatted like this:
```
#$crc&
```
where `$crc` is the CRC computed by the sample code.
It is recommended to turn on the CRC for serial communication with a client but turn it off if you are writing the ASCII commands yourself into the Serial Monitor (computing the CRC by hand is extra work!). As for what samples to use it with, it is strongly recommended for use with the Serial or any samples that have serial somewhere in their communication stream, such as the server samples. It is not recommended for HTTP.

If you want to test if cyclic redundancy is working properly, I would recommend using this packet, which requests a state update packet with the proper CRC appended (add a `;` to the end of the packet if communicating with a serial device):

```
# state update packet
6&#449316471&

# turn on device 1
0,1,1&#3692676612&

# turn off device 1
0,1,0&#3305305925&

# single glimmer green with 100 speed on device 1
1,1,3,0,255,0,100,15&#3938804531&

# single wave green with 100 speed on device 1
1,1,2,0,255,0,100&#2246576319&

# multi bars with bar size 4, using fire palette and speed 100 on device 1
1,1,10,6,100,4&#3942318360&

```

### <a name="multi-sample"></a>Multi Device Samples

The Multi Device Samples are an example of how to use the device index in the control packets to control multiple sets of LEDs from one Arduino. The sample uses two ArduCor objects to control two halves of a Neopixels Light Strip separately. The samples work with Serial communication. When dealing with significantly more than 64 LEDs on a single arduino, it is recommended that you use a Arduino Mega so that you have more memory. The current samples is designed for Arduino Unos, but it takes a hit on max_packet_size in order to conserve memory. This requires packets to be broken up to be sent to the application, leading to slower to update speeds.

### <a name="generating-samples"></a>Generating Samples

Samples are generated off of the `ArduCor` sketch in the `src` folder of this repo. It uses the script `generate_samples.sh` which reads preprocessor directives and creates multiple versions of the sketch.

To make changes to all the samples, edit the sketch in `src/ArduCorSample` and then run the `generate_samples.sh` script.
