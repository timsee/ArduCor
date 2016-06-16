
# RGB LED Routines for Arduino

This project is based around an arduino library that generates a set of lighting routines for various arduino-based lighting arrays. It works for all arduinos except the ATtiny series.

Sample sketches are provided for Rainbowduino, a NeoPixel products, or a standard RGB LED. The samples
provide an ASCII based serial interface to change the lighting routines, the brightness of the lights, and more. A Qt-based GUI is also provided which can control the arduino sketches. The GUI has been tested on Windows, Linux, Mac OS X, Android, and iOS. It requires Qt 5.2 or later and C++11.

## <a name="toc"></a>Table of Contents

* [Installation](#installation)
* [Library Usage](#library-usage)
    * [Single Color Routines](#single-routines)
    * [Multi Color Routines](#multi-routines)
* Arduino Library API ([html](https://timsee.github.io/RGB-LED-Routines/RoutinesRGB/html/a00001.html) ([pdf](https://github.com/timsee/RGB-LED-Routines/blob/master/docs/RoutinesRGB-API.pdf))
* [Sample Sketch Usage](samples)
    * [Yun Samples](samples/yun)
* [Qt Based GUI](GUI)
    * [Frontend Overview](https://timsee.github.io/RGB-LED-Routines/LightingGUI/html/front_overview.html)
    * [Backend Overview](https://timsee.github.io/RGB-LED-Routines/LightingGUI/html/backend_overview.html)
* [Contributing](#contributing)
* [License](#license)
* [Version Notes](CHANGELOG.md)

## <a name="installation"></a>Installation

1. Download the git repository
2. Copy the RoutinesRGB directory into your [Arduino Libraries directory](https://www.arduino.cc/en/Hacking/Libraries).
3. Choose the proper sample for your hardware setup from the [samples folder](samples).
4. Build and upload to your board.  


If there are compilation issues, make sure you have the library for your hardware setup installed. For a Rainbowduino, you can find the library [here](http://www.seeedstudio.com/wiki/Rainbowduino_v3.0). For a NeoPixel, you can find the library [here](https://github.com/adafruit/Adafruit_NeoPixel). 


## <a name="library-usage"></a>Library Usage

### <a name="single-routines"></a>Single Color Routines

These routines use a single color:

* Single Solid
* Single Blink
* Single Wave
* Single Glimmer
* Single Linear Fade
* Single Sawtooth Fade In
* Single Sawtooth Fade Out
* Single Singe Fade

### <a name="multi-routines"></a>Multi Colors Routines

These routines use groups of colors saved in an array to execute the routine. This can be a custom group of colors or it can come from a pallete of preset colros. All routines have a parameter used to determine the color group for the routine. A description of the different color groups is [here](https://timsee.github.io/RGB-LED-Routines/RoutinesRGB/html/a00003.html). 

* Multi Glimmer
* Multi Random Individual
* Multi Random Solid
* Multi Fade
* Multi Bars Solid
* Multi Bars Moving

### <a name="post-processing"></a>Post Processing

Tese are optional and can be applied after a routine. The `applyBrightness` option is recommended, since it simplifies dimming the LEDs by making their overall brightness a parameter between 0 and 100. 

* Apply Brightness
* Draw Color on LED

## <a name="contributing"></a>Contributing

1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request.


## <a name="license"></a>License

MIT License, provided [here](LICENSE).
