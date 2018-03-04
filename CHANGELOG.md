# Change Log

### **v1.0**
* Initial version

### **v1.5**
* Added support for *x* number of saved colors, defined by the const `NUM_OF_COLORS`.
* Cleaned code and conformed closer to the [Arduino Style Guide](https://www.arduino.cc/en/Reference/StyleGuide).
* Added Idle timeout.
* Added 6 new lighting routines.

### **v1.7**
* Refactored the lighting routines into their own library.
* Cleaned up the API.
* Reduced memory usage.

### **v1.8**
* Arduino sample code is now split into a different sketch for each supported product.
* Added a Qt GUI.
* Updated API to better support a GUI.

### **v1.8.1**
#### Documentation Update
* Added full Doxygen support for `LightingGUI` and `RoutinesRGB`.
* Cleanup of project's markdown.
* Added `custom` option to sample code sketch generation scripts.

### **v1.8.3**
#### Code Cleaning Update
* Rewrote sample generation script to be more reusable.
* Cleaned code and fixed bugs in GUI project.

### **v1.8.5**
#### Color Picker Update
* Added a ColorPicker widget for the GUI.
* Cleaned code and fixed bugs in GUI project.

### **v1.9.0**
#### Color Presets Update
* Added 17 color presets. These can be used with multi color routines and are based around basic themes, such as *Fire* or *Snow*.
* Refactored GUI, sample code, and arduino library to support the color presets.
* Cleaned code and fixed bugs across all projects.

### **v1.9.1**
#### Memory Reduction Update
* Significantly reduced memory usage of arduino samples and library.
* Moved color presets to PROGMEM.
* Documented all projects.
* Cleaned code and fixed bugs across all projects.

### **v1.9.2**
#### API and Bug Fixes Update
* Refactored names of enumerated types and API calls to be more uniform.
* Polished menu bar for the GUI.
* Added Application Icon for the GUI.
* Cleaned code and fixed bugs across all projects.
* Updated project documentation to reflect API changes.
* Cleaned code and fixed bugs across all projects.

### **v1.9.5**
#### Wireless, Android, and iOS Update
* Added 6 Arduino Yun samples, 3 for HTTP and 3 using UDP.
* Extended the GUI's CommLayer to support HTTP and UDP.
* Added support for Android and iOS builds.
* Updated GUI's layout to support the new screen sizes.
* Fixed bugs, probably created more.

### **v1.9.6**
#### Settings Page Update
* Upgraded the GUI's settings screen.
* Optimized the arduino library.
* Fixed GUI layout issues on certain devices.
* Fixed bugs.

### **v1.9.7**
#### Arduino Library and Linux Update
* Added 4 single color routines.
* Added post-processing options to arduino library.
* Tested and added Linux support to GUI.
* Fixed bugs.

### **v1.9.8**
#### Phillips Hue Update
* Added support for the Phillips Hue to the GUI.
* Refactored GUI code to support more types of connections.
* Fixed bugs.

### **v2.0.0**
#### Two Way Communication Update
* Move GUI project to Corluma Repository.
* Added state update packets.
* Added optional discovery methods.
* Added better support for connecting multiple devices to the same arduino.
* Removed custom sample, added multi device sample.
* Fixed bugs.

### **v2.1.0**
#### Cleanup Update
* Fixed bugs.
* Added new message for requesting all the used values in the custom color array.
* Changed the message protocol format used in samples.
* Changed multi device sample so that it works on HTTP and UDP.

### **v2.1.1**
#### Cleanup Update Part 2
* Fixed bugs in samples.

### **v2.2.0**
#### Samples Update
* Added an optional CRC to messages.
* Updated discovery packes.
* Fixed more bugs in samples.

### **v2.3.0**
#### Raspberry Pi Sample Update
* Added a new sample using a Raspberry Pi and an Arduino instead of an Arduino Yun. 

### **v2.8.0**
#### Samples Optimization Update
*Note: This update modifies the messaging protocols. Applications designed to work with 2.8 or later will not work with 2.3 or earlier.*
* Removed use of arduino's String datatype in samples, in favor of char arrays. This sacrifices readability for efficiency.
* Modified packet echo to only echo packets that were successfully parsed.
* Converted the Raspberry Pi sample and documentation to be a general server that can run on multiple devices.
* Converted the server sample from working with 1 serial device to working with multiple serial devices.
* Discovery packets in the samples now send both a major and minor API level, denoting the functions available in the sample and the messaging protocol that should be used to access them.
* Added an edge case to the library that handles bar sizes for routines on devices using less than 32 LEDs.

### **v2.8.2**
#### Naming Update
* Adds the option to hardcode a name for an arduino. This name is sent in the discovery packet and can be used as an identifier in other applications.
* Added more edge case handling to the python server.
* incremented the API level to 2.1

### **v2.9.0**
#### Reinventing the Light Switch Update
* Renamed the project from `RGB-LED-Routines` to `ArduCor`
* Changed turning lights on/off from being handled as a mode. Instead, now on/off commands have their own packet header.
* Discovery Packets now send a `ELightType` with each discovery packet, to denote the type of hardware they are (light cube, single LED, light strip, etc.)
* Discovery Packets now send a `EProductType` with each discovery packet, to denote the type of product they are (Neopixels, Rainbowduino, etc.)
* Modified the "Frozen" color group.
* Fixed miscellaneous bugs.
* Incremented API level to 3.0

