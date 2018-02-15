# Server

This sample is meant for controlling _RGB-LED-Routines_ wirelessly without using the Arduino Yun samples. Instead, we use a computer running this server as a passthrough which takes UDP packets and sends them over serial to one or more arduino devices.

## <a name="hardware"></a>Required Hardware

* An Arduino running a serial sample of `RGB-LED-Routines`
* A computer running python 2.7
* A usb cable for communication between the Arduino and Raspberry Pi

#### <a name="usage"></a>Usage

The script can support 1 or more serial devices. To start up the script, run:
```
python UDPtoSerialAdapter.py $serialDevice1 $serialDevice2 ... $serialDeviceN
```
where each  `$serialDevice*` is the path to the serial device you want to connect to.


You'll get a readout similar to this if it can find a serial device, connect to it, control it, and setup a UDP server:

```
Setup Serial...
Serial Device #0 found with lighting devices [1]
Serial stream confirmed with 1 device.
Setup the UDP Socket...
```

The `Serial Device #...` line will be different depending on how many arduinos are connected and what light indices they use. For this sample, we are using a single Arduino with a single lighting hardware connected to it.


The final line, `Setup the UDP Socket...`  means its set up a UDP server, but its waiting for an initial discovery packet to be sent over UDP. Send it a packet using [Corluma](https://github.com/timsee/Corluma) or your preferred method for sending UDP packets. Once its fully online, it'll print out these lines:
```
UDP packet received!
Starting main loop...
```
Once you see this, your server is ready for forwarding packets to the arduino. Have fun!

#### <a name="Guides"></a>Guides

* [Raspberry Pi Setup](RaspberryPiSetup.md)

#### <a name="FAQ"></a>FAQ

* *Do I need to change anything in my sample sketches to use this server?* To connect to only one serial device through the server, nothing needs to be changed. To connect to multiple serial devices, you need to set the `DEFAULT_HW_INDEX` of each of the samples to be unique.  You may also name each device so that they are easier to discern in another application.

* *What computers is this server compatible with?* I test on my 2016 Macbook Pro and deploy to a Raspberry Pi 3B+. The server isn't using any hardware specific function calls, so as long as you have a unix machine that runs python 2.7, you should be set. It has not been tested for Windows.

* *How does the server handle multiple devices talking to it?* The server will only communicate with the last device it received a packet from. This makes it such that it can handle multiple devices, but previous devices need to keep sending packets if they want to keep getting updates.




