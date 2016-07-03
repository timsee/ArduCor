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
* Cleaned up API of the Arduino library.
* Fixed bugs.

