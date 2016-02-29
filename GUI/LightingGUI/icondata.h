/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#ifndef ICONDATA_H
#define ICONDATA_H

#include "datalayer.h"

#include <QPixmap>
#include <QImage>

#include <stdio.h>

/*!
 * \brief The IconData class creates all the grids of colors that are seen throughout
 * the app's icons. The grids are made by using a buffer to do the computation and
 * then upscaling it to the actual image size to give the square pixel-y effect.
 */
class IconData
{
public:
    IconData();
    IconData(int width, int height);
    IconData(int width, int height, DataLayer *data); // necessary for array colors

    /*!
     * \brief setSolidColor sets the icon as a solid color
     * \param r a value between 0-255
     * \param g a value between 0-255
     * \param b a value between 0-255
     */
    void setSolidColor(int r, int g, int b);
    /*!
     * \brief setRandomColors sets every region to a random color.
     */
    void setRandomColors();

    /*!
     * \brief setRandomSolid draws 4 random colors on icon
     */
    void setRandomSolid();

    /*!
     * \brief setArrayColors shows all array colors, repeating if necessary
     */
    void setArrayColors();

    /*!
     * \brief setFadeAllColors regions slowly fade from one color to another
     */
    void setFadeAllColors();

    /*!
     * \brief setArrayGlimmer sets as mostly mainColor, but adds random colors
     *        as a glimmer effect
     */
    void setArrayGlimmer();

    /*!
     * \brief setArrayFade regions slowly fade from one array color to another
     */
    void setArrayFade();

    /*!
     * \brief setArrayRandomSolid sets icon as 4 colors from array
     */
    void setArrayRandomSolid();

    /*!
     * \brief setArrayRandomIndividual sets each region a random color from the array
     */
    void setArrayRandomIndividual();

    /*!
     * \brief setArrayBarsSolid draws the bars with region sizes of 2
     */
    void setArrayBarsSolid();

    /*!
     * \brief setArrayBarsSolid draws the bars with region sizes of 2 but slightly offsetof
     *        'cause its hard to show motion in a static icon.
     */
    void setArrayBarsMoving();

    /*!
     * \brief addGlimmer adds glimmer lighting effect to the preexisting data
     */
    void addGlimmer();
    /*!
     * \brief addBlink turns half the columns of preexisting data black to give blink effect
     *        in static image
     */
    void addBlink();
    /*!
     * \brief addFade takes preexisting data and fades it to black column by column
     */
    void addFade();

    /*!
     * getters for private values
     */
    uint getDataLength();
    uint8_t *getData();
    uint getWidth();
    uint getHeight();

    /*!
     * \brief renderAsQImage takes the data and outputs it as a QImage
     * \return a QImage representation of the data
     */
    QImage renderAsQImage();
    /*!
     * \brief renderAsQPixmap takes the dat and outputs it as a QPixmap
     * \return a QPixmap representation of the data
     */
    QPixmap renderAsQPixmap();

private:

    void setup(int width, int height);

    DataLayer *mDataLayer;

    /*!
     * the full data used when rendering an image.
     * It matches in size the size of the image
     */
    uint8_t *mData;
    uint mWidth;
    uint mHeight;
    uint mDataLength;
    /*!
     * a buffer that is used when doing processing.
     * set of RGB values in the buffer represents an entire
     * region of the mData. A function bufferToOutput() must
     * be called in any function to map mBuffer to mData.
     */
    uint8_t *mBuffer;
    uint mBufferWidth;
    uint mBufferHeight;
    uint mBufferLength;

    /*!
     * \brief bufferToOutput required at the end of any set of processing
     *        this maps the mBuffer values to mData, scaling mBuffer up.
     */
    void bufferToOutput();

    DataLayer::Color getMiddleColor(DataLayer::Color c_1,
                                    DataLayer::Color c_2);

    DataLayer::Color* setupFadeGroup(DataLayer::Color c_1, DataLayer::Color c_2,
                                     DataLayer::Color c_3, DataLayer::Color c_4,
                                     DataLayer::Color c_5, DataLayer::Color c_6,
                                     DataLayer::Color c_7, DataLayer::Color c_8);

};

#endif // ICONDATA_H
