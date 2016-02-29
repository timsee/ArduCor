
# RGB LED Routines for Arduino

This project is based around an arduino library that generates a set of lighting routines for various arduino-based lighting arrays.

Sample sketches are provided for Rainbowduino, a NeoPixel products, or a standard RGB LED. The samples
provide an ASCII based serial interface to change the lighting routines, the brightness of the lights, and more. A Qt-based GUI is also provided which can control the arduino sketches. The GUI has been tested on Windows and Mac OS X and requires Qt 5.1 or later.

## <a name="toc"></a>Table of Contents

* [Installation](#installation)
* [Library Usage](#library-usage)
	* [Single Color Routines](#single-color)
	* [Multi Color Routines](#multi-colors)
	* [Routines with Saved Colors](#saved-colors)
	* [Getters and Setters](#getters-setters)
* [Sample Sketch Usage](#sample-usage)
* [Contributing](#contributing)
* [License](#license)


## <a name="installation"></a>Installation

1. Download the git repository
2. Copy the RoutinesRGB directory into your [Arduino Libraries directory](https://www.arduino.cc/en/Hacking/Libraries).
3. Choose the proper sample for your hardware setup from the [samples folder](samples).
4. Build and upload to your board.  

 If there is compilation issues, make sure you have the library for your hardware setup installed. For a Rainbowduino, you can find the library [here](http://www.seeedstudio.com/wiki/Rainbowduino_v3.0). For a NeoPixels, you can find the library [here](https://github.com/adafruit/Adafruit_NeoPixel). 
 

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

### <a name="saved-colors"></a>Routines with the Array Colors

These routines use the colors saved in an array to execute the routine. These routines take a parameter that determines how many colors to use:

* Array Glimmer
* Array Random Individual
* Array Random Solid
* Array Fade
* Array Bars Solid
* Array Bars Moving

### <a name="getters-setters"></a>Getters and Setters

#### Setters

* setMainColor: takes RGB values. Sets the main color used for single color routines.
* setColor: takes an index and RGB values. Sets a color in the `colors` array, at the given index. 
* setBrightness: sets the brightness between 0 and 100, with 0 being off and 100 being full power.
* setFadeSpeed: sets the speed parameter that routines that fade utilize, must be set between 1 and 100, with 1 being the slowest possible fade. 
* setBlinkSpeed: Sets how many updates it takes for a routine that switches between solid colors to update. Takes a value between 1 and 255, with 1 causing it to change on every frame. 


#### Getters

* getColor: returns the color at the given index.
* getR: returns the r value of the buffer at the given index. 
* getG: returns the g value of the buffer at the given index.
* getB: returns the b value of the buffer at the given index.

## <a name="sample-usage"></a>Sample Sketch Usage

Sample code is generated off of the main project using the generate_samples script. All sample code is stored in the samples directory.


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

### **v1.8** 
* Arduino sample code is now split into a different sketch for each supported product. These sketches generates off a single sketch using a script.
* Added a Qt GUI
* Miscellaneous changes to support a GUI.

## <a name="license"></a>License

MIT License, provided [here](LICENSE).
