
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
* Arduino Library API ([html](https://timsee.github.io/RGB-LED-Routines/RoutinesRGB/html/class_routines_r_g_b.html)) ([pdf](https://github.com/timsee/RGB-LED-Routines/docs/RoutinesRGB-API.pdf))
* [Sample Sketch Usage](samples)
* [Qt Based GUI](GUI)
    *  [Frontend Overview](https://timsee.github.io/RGB-LED-Routines/LightingGUI/html/front_overview.html)
    *  [Backend Overview](https://timsee.github.io/RGB-LED-Routines/LightingGUI/html/backend_overview.html)
* [Contributing](#contributing)
* [License](#license)
* [Version Notes](CHANGELOG.md)

## <a name="installation"></a>Installation

1. Download the git repository
2. Copy the RoutinesRGB directory into your [Arduino Libraries directory](https://www.arduino.cc/en/Hacking/Libraries).
3. Choose the proper sample for your hardware setup from the [samples folder](samples).
4. Build and upload to your board.  


If there are compilation issues, make sure you have the library for your hardware setup installed. For a Rainbowduino, you can find the library [here](http://www.seeedstudio.com/wiki/Rainbowduino_v3.0). For a NeoPixels, you can find the library [here](https://github.com/adafruit/Adafruit_NeoPixel). 


## <a name="library-usage"></a>Library Usage

### <a name="single-color"></a>Single Color Routines

These routines use a single color:

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

These routines use the colors saved in an array to execute the routine. These routines take a parameter that determines how many colors from the array to use:

* Array Glimmer
* Array Random Individual
* Array Random Solid
* Array Fade
* Array Bars Solid
* Array Bars Moving


## <a name="contributing"></a>Contributing

1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request.


## <a name="license"></a>License

MIT License, provided [here](LICENSE).
