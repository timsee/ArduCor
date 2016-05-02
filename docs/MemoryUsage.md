
# Arduino Memory Usage

### Library Memory Usage

The library uses ~1100 bytes of program storage space and ~125 bytes of global variables. At runtime, the library will also allocate memory based off of how many LEDs it is set up to use. It will allocate `LED_COUNT * 4 ` bytes of memory. For example, if you are using 64 LEDs it will allocate 256 bytes of memory. 

This small footprint makes it compatible with any Arduino, even down to the [Arduino Gemma](https://www.arduino.cc/en/Main/ArduinoGemma)with its 8 KB of flash memory and 512 bytes of SRAM. We intend to keep our footprint small enough to fit onto this board as we continue development. 

### Sample Memory Usage

The code samples use more memory since they provide a messaging system and use other libraries. These may use up to ~16000 bytes of flash memory and around ~500 bytes for global variables. This means they only take up half the flash memory of an [Arduino Uno](https://www.arduino.cc/en/Main/ArduinoBoardUno) and only one quarter of the SRAM, but 
the samples may be a tight fit on a board like the Gemma. If this is the case, the easiest solution is to take out the serial communication.