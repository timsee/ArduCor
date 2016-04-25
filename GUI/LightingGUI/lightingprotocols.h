#ifndef LIGHTINGPROTOCOLS_H
#define LIGHTINGPROTOCOLS_H
/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 *
 * This file defines the protocols used for the Arduino libraries and the GUI.
 *
 * A slightly modified version of this file exists in the arduino project. None of the
 * modifications change the naming, documentation, or order of the protocols. Instead, the
 * changes allow the GUI version to use the strongly typed enums that were made available in C++11.
 *
 */

/*!
 * \enum ELightingMode The mode is for determining what LED routines to use. Some routines
 *  use a single color and others use multiple colors.
 */
enum class ELightingMode
{
    /*!
     * <b>0</b><br>
     * <i>Turns off the LEDs.</i>
     */
    eOff,
    /*!
     * <b>1</b><br>
     * <i>Shows a single color at a fixed brightness.</i>
     */
    eSingleSolid,
    /*!
     * <b>2</b><br>
     * <i>Alternates between showing a single color at a fixed
     * brightness and turning the LEDs completely off.</i>
     */
    eSingleBlink,
    /*!
     * <b>3</b><br>
     * <i>Linear fade of the brightness of the LEDs.</i>
     */
    eSingleFade,
    /*!
     * <b>4</b><br>
     * <i> Randomly dims some of the LEDs to give a glimmer effect.</i>
     */
    eSingleGlimmer,
    /*!
     * <b>5</b><br>
     * <i> Uses the first color of the array as the base color
     * and uses the other colors for a glimmer effect.</i>
     */
    eMultiGlimmer,
    /*!
     * <b>6</b><br>
     * <i>Fades slowly between each color in the array.</i>
     */
    eMultiFade,
    /*!
     * <b>7</b><br>
     * <i>Chooses a random color from the array and lights all
     * all LEDs to match that color.</i>
     */
    eMultiRandomSolid,
    /*!
     * <b>8</b><br>
     * <i>Chooses a random color from the array for each
     * individual LED.</i>
     */
    eMultiRandomIndividual,
    /*!
     * <b>9</b><br>
     * <i>Draws the colors of the array in alternating
     *  groups of equal size.</i>
     */
    eMultiBarsSolid,
    /*!
     * <b>10</b><br>
     * <i>Draws the colors of the array in alternating
     *  groups of equal size. On each update, it moves those
     *  groups one index to the right, creating a scrolling
     *  effect.</i>
     */
    eMultiBarsMoving,
    eLightingMode_MAX //total number of modes
};



/*!
 * \enum EColorPreset used during multi color routines to determine
 * which colors to use in the routine. eCustom uses the custom color array,
 * while all other values use presets based around overall themes.
 */
enum class EColorPreset
{
    /*!
     * <b>0</b><br>
     * <i>Use the custom color array instead of a preset routine.</i>
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
    eCorrorsive,
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
     * <i>Rather than using using presets, it uses all
     * possible colors.</i>
     */
    eAll,
    /*!
     * <b>14</b><br>
     * <i>red, green, and blue.</i>
     */
    eRGB,
    /*!
     * <b>15</b><br>
     * <i>Cyan, magenta, yellow.</i>
     */
    eCMY,
    /*!
     * <b>16</b><br>
     * <i>Red, yellow, green, cyan, blue, magenta.</i>
     */
    eSixColor,
    /*!
     * <b>17</b><br>
     * <i>Red, yellow, green, cyan, blue, magenta, white.</i>
     */
    eSevenColor,
    eColorPreset_MAX //total number of presets
};
#endif // LIGHTINGPROTOCOLS_H
