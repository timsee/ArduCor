
#ifndef ICONDATA_H
#define ICONDATA_H

#include "datalayer.h"

#include <QPixmap>
#include <QImage>

#include <stdio.h>
#include <memory>


/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief The IconData class creates all the grids of colors that are seen throughout
 * the app's icons. The grids are made by using a very small buffer of RGB values to do the 
 * computation. For display, this buffer gets upscaled to the desired icon size. This 
 * gives it the square pixel-y effect.
 *
 * \todo Clean up some of the messier data manipulations such as the fades.
 * \todo Clean up the icons that don't accurately represent their modes such as the "Bars Moving" icon.
 */
class IconData
{
public:
    IconData();
    /*!
     * Standard Constructor, values must
     * be a multiple of four.
     *
     * \param width the width
     * \param height the height
     */
    IconData(int width, int height);
    /*!
     * Required constructor when the icons
     * use the color array.
     *
     * \param width the width
     * \param height the height
     * \param data pointer to the data layer to access array data.
     */
    IconData(int width, int height, std::shared_ptr<DataLayer> data);

    /*!
     * \brief setSolidColor sets the icon as a solid color
     */
    void setSolidColor(QColor color);
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
     * \brief setArrayBarsSolid draws the bars with region sizes of 2 but slightly offset
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
    uint dataLength();
    uint8_t *data();
    uint width();
    uint height();

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

    std::shared_ptr<DataLayer> mDataLayer;

    /*!
     * the full data used when rendering an image.
     * It matches in size the size of the image
     */
    QVector<uint8_t> mData;
    uint mWidth;
    uint mHeight;
    uint mDataLength;
    /*!
     * a buffer that is used when doing processing.
     * set of RGB values in the buffer represents an entire
     * region of the mData. A function bufferToOutput() must
     * be called in any function to map mBuffer to mData.
     */
    QVector<uint8_t> mBuffer;
    uint mBufferWidth;
    uint mBufferHeight;
    uint mBufferLength;

    /*!
     * \brief bufferToOutput required at the end of any set of processing
     *        this maps the mBuffer values to mData, scaling mBuffer up.
     */
    void bufferToOutput();

    QColor getMiddleColor(QColor c_1,
                          QColor c_2);

    QColor *setupFadeGroup(QColor c_1, QColor c_2,
                           QColor c_3, QColor c_4,
                           QColor c_5, QColor c_6,
                           QColor c_7, QColor c_8);

};

#endif // ICONDATA_H
