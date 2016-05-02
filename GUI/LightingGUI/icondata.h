
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
 * \todo Clean up the icons that don't accurately represent their modes such as the "Bars Moving" icon.
 */
class IconData
{
public:
    /*!
     * \brief Constructor
     */
    IconData();
    /*!
     * Standard Constructor, values must  be a multiple of four.
     *
     * \param width the width
     * \param height the height
     */
    IconData(int width, int height);
    /*!
     * Required constructor when the icons use the color array.
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
     * \brief setArrayColors shows all array colors, repeating if necessary
     */
    void setArrayColors(EColorPreset preset);

    /*!
     * \brief setArrayGlimmer sets as mostly mainColor, but adds random colors
     *        as a glimmer effect
     */
    void setArrayGlimmer(EColorPreset preset);

    /*!
     * \brief setArrayFade regions slowly fade from one array color to another
     */
    void setArrayFade(EColorPreset preset);

    /*!
     * \brief setArrayRandomSolid sets icon as 4 colors from array
     */
    void setArrayRandomSolid(EColorPreset preset);

    /*!
     * \brief setArrayRandomIndividual sets each region a random color from the array
     */
    void setArrayRandomIndividual(EColorPreset preset);

    /*!
     * \brief setArrayBarsSolid draws the bars with region sizes of 2
     */
    void setArrayBarsSolid(EColorPreset preset);

    /*!
     * \brief setArrayBarsSolid draws the bars with region sizes of 2 but slightly offset
     *        'cause its hard to show motion in a static icon.
     */
    void setArrayBarsMoving(EColorPreset preset);

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
    /*!
     * \brief width getter for the data buffer's width.
     * \return the width of the IconData.
     */
    uint width();
    /*!
     * \brief height getter for the data buffer's height.
     * \return the height of the IconData.
     */
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

    /*!
     * \brief setup used by the constructors to set up the data buffers.
     * \param width the width of the data buffers.
     * \param height the height of the data buffers.
     */
    void setup(int width, int height);

    /*!
     * \brief mDataLayer a pointer to the data layer.
     */
    std::shared_ptr<DataLayer> mDataLayer;

    /*!
     * the full data used when rendering an image.
     * It matches in size the size of the image
     */
    QVector<uint8_t> mData;
    /*!
     * \brief mWidth the width of the resulting icon
     */
    uint mWidth;
    /*!
     * \brief mHeight the height of the resulting icon.
     */
    uint mHeight;
    /*!
     * \brief mDataLength the number of bytes needed to
     *        to fill mWidth * mHeight * 3.
     */
    uint mDataLength;

    /*!
     * a buffer that is used when doing processing.
     * set of RGB values in the buffer represents an entire
     * region of the mData. A function bufferToOutput() must
     * be called in any function to map mBuffer to mData.
     */
    QVector<uint8_t> mBuffer;
    /*!
     * \brief mBufferWidth the buffer's width.
     */
    uint mBufferWidth;
    /*!
     * \brief mBufferHeight the buffer's height.
     */
    uint mBufferHeight;
    /*!
     * \brief mBufferLength the number of bytes needed to fill
     *        mBufferWidth * mBufferHeight * 3.
     */
    uint mBufferLength;

    /*!
     * \brief bufferToOutput required at the end of any set of processing
     *        this maps the mBuffer values to mData, scaling mBuffer up.
     */
    void bufferToOutput();

    /*!
     * \brief getMiddleColor helper that takes two colors and computes
     *        the color in the middle of those colors by averaging their
     *        values.
     * \param first The first color being used.
     * \param second The second color being used.
     * \return A new QColor based on adding the two colors together and dividing
     *         by two.
     */
    QColor getMiddleColor(QColor first,
                          QColor second);

};

#endif // ICONDATA_H