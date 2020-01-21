/*!
 * \file ArduCorProtocols.h
 * \version v3.0.0
 * \date April 14, 2018
 * \author Tim Seemann
 * \copyright <a href="https://github.com/timsee/ArduCor/blob/master/LICENSE">
 *            MIT License
 *            </a>
 *
 * This file defines the protocols used for the sample sketches.
 *
 * This file also gets copied to other projects as part of integrating with this project. For example,
 * the [Corluma](https://github.com/timsee/Corluma) project has a C++ version of this file. If packets
 * between the two projects seem mixed up, check that the version of the Corluma App you are using
 * matches the version of the your ArduCor library.
 *
 * Protocol Version: 3.3
 *
 */


/*!
 * \enum ERoutine Each routine makes the LEDs shine in different ways. There are
 *       two main types of routines: Single Color Routines use a single color while Multi
 *       Color Routines rely on an EPalette.
 */
enum  ERoutine
{
    /*!
     * <b>0</b><br>
     * <i>Shows a single color at a fixed brightness.</i>
     */
    eSingleSolid,
    /*!
     * <b>1</b><br>
     * <i>Alternates between showing a single color at a fixed
     * brightness and turning the LEDs completely off.</i>
     */
    eSingleBlink,
    /*!
     * <b>2</b><br>
     * <i>Linear fade of the brightness of the LEDs.</i>
     */
    eSingleWave,
    /*!
     * <b>3</b><br>
     * <i>Randomly dims some of the LEDs to give a glimmer effect.</i>
     */
    eSingleGlimmer,
    /*!
     * <b>4</b><br>
     * <i>Fades the brightness in and out of the LEDs. Takes a parameter of either
     * 0 or 1. If its 0, it fades linearly. If its 1, it fades using a sine wave, so less time
     * in the mid range of brightness and more time in the full and very dim light.</i>
     */
    eSingleFade,
    /*!
     * <b>5</b><br>
     * <i>fades in or out using a sawtooth function. Takes a parameter of either 0 or
     * 1. If its 0, it starts off and fades to full brightness. If its 1, it starts at full
     * brightness and fades to zero. The fade is linear. </i>
     */
    eSingleSawtoothFade,
    /*!
     * <b>6</b><br>
     * <i> Uses the first color of the array as the base color
     * and uses the other colors for a glimmer effect.</i>
     */
    eMultiGlimmer,
    /*!
     * <b>7</b><br>
     * <i>Fades slowly between each color in the array.</i>
     */
    eMultiFade,
    /*!
     * <b>8</b><br>
     * <i>Chooses a random color from the array and lights all
     * all LEDs to match that color.</i>
     */
    eMultiRandomSolid,
    /*!
     * <b>9</b><br>
     * <i>Chooses a random color from the array for each
     * individual LED.</i>
     */
    eMultiRandomIndividual,
    /*!
     * <b>10</b><br>
     * <i>Draws the colors of the array in alternating
     *  groups of equal size. On each update, it moves those
     *  groups one index to the right, creating a scrolling
     *  effect.</i>
     */
    eMultiBars,
    eRoutine_MAX //total number of modes
};



/*!
 * \enum EPalette used during multi color routines to determine
 *       which colors to use in the routine. eCustom uses the custom
 *       color array, eAll generates its colors randomly. All
 *       other values use presets based around overall themes.
 */
enum EPalette
{
    /*!
     * <b>0</b><br>
     * <i>Use the custom color array instead of a preset group.</i>
     */
    eCustom,
    /*!
     * <b>1</b><br>
     * <i>Shades of blue with some teal.</i>
     */
    eWater,
    /*!
     * <b>2</b><br>
     * <i>Shades of teal with some blue, white, and light purple.</i>
     */
    eFrozen,
    /*!
     * <b>3</b><br>
     * <i>Shades of white with some blue and teal.</i>
     */
    eSnow,
    /*!
     * <b>4</b><br>
     * <i>Based on the cool colors: blue, green, and purple.</i>
     */
    eCool,
    /*!
     * <b>5</b><br>
     * <i>Based on the warm colors: red, orange, and yellow.</i>
     */
    eWarm,
    /*!
     * <b>6</b><br>
     * <i>Similar to the warm set, but with an emphasis on oranges to
     * give it a fire-like glow.</i>
     */
    eFire,
    /*!
     * <b>7</b><br>
     * <i>Mostly red, with some other, evil highlights.</i>
     */
    eEvil,
    /*!
     * <b>8</b><br>
     * <i>Greens and whites, similar to radioactive goo from
     * a 90s kids cartoon.</i>
     */
    eCorrosive,
    /*!
     * <b>9</b><br>
     * <i>A purple-based theme. Similar to poison vials from
     * a 90s kids cartoon.</i>
     */
    ePoison,
    /*!
     * <b>10</b><br>
     * <i>Shades of pink, red, and white.</i>
     */
    eRose,
    /*!
     * <b>11</b><br>
     * <i>The colors of watermelon candy. bright pinks
     * and bright green.</i>
     */
    ePinkGreen,
    /*!
     * <b>12</b><br>
     * <i>Bruce Springsteen's favorite color scheme, good ol'
     * red, white, and blue.</i>
     */
    eRedWhiteBlue,
    /*!
     * <b>13</b><br>
     * <i>red, green, and blue.</i>
     */
    eRGB,
    /*!
     * <b>14</b><br>
     * <i>Cyan, magenta, yellow.</i>
     */
    eCMY,
    /*!
     * <b>15</b><br>
     * <i>Red, yellow, green, cyan, blue, magenta.</i>
     */
    eSixColor,
    /*!
     * <b>16</b><br>
     * <i>Red, yellow, green, cyan, blue, magenta, white.</i>
     */
    eSevenColor,
    ePalette_MAX //total number of presets
};


/*!
 * \enum EPacketHeader Message headers for packets coming over serial.
 */
enum EPacketHeader
{
  /*!
   * <b>0</b><br>
   * <i>Takes one parameter, 0 turns off, 1 turns on.</i>
   */
  eOnOffChange,
  /*!
   * <b>1</b><br>
   * <i>Takes multiple parameters depending on the use case. Changes the lighting routine
   * currently getting displayed.</i>
   */
  eModeChange,
  /*!
   * <b>2</b><br>
   * <i>Takes four parameters. The first is the index of the custom color,
   * the remaining three parameters are a 0-255 representation
   * of Red, Green, and Blue.</i>
   */
  eCustomArrayColorChange,
  /*!
   * <b>3</b><br>
   * <i>Takes one parameter, sets the palette brightness between 0 and 100. Used only in multi
   * color routines and ignored in single routines. </i>
   */
  eBrightnessChange,
  /*!
   * <b>4</b><br>
   * <i>Change the number of colors used in a custom array routine.</i>
   */
  eCustomColorCountChange,
  /*!
   * <b>5</b><br>
   * <i>Set to 0 to turn off, set to any other number minutes until
   * idle timeout happens.</i>
   */
  eIdleTimeoutChange,
  /*!
   * <b>6</b><br>
   * <i>Sends back a packet that contains basic LED state information.</i>
   */
  eStateUpdateRequest,
  /*!
   * <b>7</b><br>
   * <i>Sends back a packet that contains the size of the custom array and all of the colors in it. </i>
   */
  eCustomArrayUpdateRequest,
  ePacketHeader_MAX //total number of Packet Headers
};
