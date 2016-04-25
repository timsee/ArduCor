/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include <qDebug>

#include <stdlib.h>
#include <ctime>
#include <algorithm>

#include "icondata.h"

IconData::IconData() {
    setup(64, 64);
}

IconData::IconData(int width, int height) {
    setup(width, height);
}

IconData::IconData(int width, int height, std::shared_ptr<DataLayer>data) {
    mDataLayer = data;
    setup(width, height);
}

void IconData::setup(int width, int height) {
    srand(time(NULL));

    // first, check that both are a multiple of four
    bool printError = false;
    while ((width % 4)) {
        width = width - 1;
        printError = true;
    }
    while((height % 4)) {
        height = height - 1;
        printError = true;
    }

    if (printError) {
        qDebug() << "Warning: a value was given to icon data that wasn't a multiple of four. The value has been adjusted!";
    }

    // always uses RGB_888 so multiply width and height by 3
    mWidth = width;
    mHeight = height;
    mDataLength = width * height * 3;
    mData = QVector<uint8_t>(mDataLength);

    mBufferWidth = 4;
    mBufferHeight = 4;
    mBufferLength = mBufferWidth * mBufferHeight * 3;
    mBuffer = QVector<uint8_t>(mBufferLength);

    // zero out the arrays
    for (uint i = 0; i < mBufferLength; i ++) {
        mBuffer[i] = 0;
    }
    for (uint i = 0; i < mDataLength; i ++) {
        mData[i] = 0;
    }
}

void IconData::bufferToOutput() {
    uint j = 0;
    uint k = 0;
    for (uint i = 0; i < mDataLength; i = i + 3) {
        if ((i % (mWidth * 3)) < mWidth * 3 / 4) {
            mData[i] = mBuffer[j];
            mData[i + 1] = mBuffer[j + 1];
            mData[i + 2] = mBuffer[j + 2];
        }
        else if ((i % (mWidth * 3)) < mWidth * 6 / 4) {
             mData[i] = mBuffer[j + 3];
             mData[i + 1] = mBuffer[j + 4];
             mData[i + 2] = mBuffer[j + 5];
        }
        else if ((i % (mWidth * 3)) < mWidth * 9 / 4) {
             mData[i] = mBuffer[j + 6];
             mData[i + 1] = mBuffer[j + 7];
             mData[i + 2] = mBuffer[j + 8];
        }
        else if ((i % (mWidth * 3)) < mWidth * 3) {
             mData[i] = mBuffer[j + 9];
             mData[i + 1] = mBuffer[j + 10];
             mData[i + 2] = mBuffer[j + 11];
        }

        if (!((i + 3) % (mWidth * 3))) {
            k++;
        }
        if (k == (mHeight / 4)) {
            j = j + 4 * 3;
            k = 0;
        }
    }
}

void IconData::setSolidColor(QColor color) {
    for (uint i = 0; i < mBufferLength; i = i + 3) {
        mBuffer[i] = color.red();
        mBuffer[i + 1] = color.green();
        mBuffer[i + 2] = color.blue();
    }
    bufferToOutput();
}


void IconData::setArrayColors(EColorPreset preset) {
    if (mDataLayer == NULL) {
        qDebug() << "ERROR: the data layer is not set, cannot use the arary colors!";
    }
    int colorCount = mDataLayer->arraySize((int)preset);
    QColor *color_array = mDataLayer->colorArray((int)preset);

    int j = 0;
    for (uint i = 0; i < mBufferLength; i = i + 3) {
        mBuffer[i] = color_array[j].red();
        mBuffer[i + 1] = color_array[j].green();
        mBuffer[i + 2] = color_array[j].blue();
        j = (j + 1) % colorCount;
    }
    bufferToOutput();
}



void IconData::setArrayGlimmer(EColorPreset preset) {
    int colorCount = mDataLayer->arraySize((int)preset);
    QColor *color_array = mDataLayer->colorArray((int)preset);
    for (uint i = 0; i < mBufferLength; i = i + 3) {
        int shouldChange = rand() % 100;
        if (shouldChange <= 20) {
            int index = rand() % colorCount;
            mBuffer[i] = color_array[index].red();
            mBuffer[i + 1] = color_array[index].green();
            mBuffer[i + 2] = color_array[index].blue();
        } else {
            mBuffer[i] = color_array[0].red();
            mBuffer[i + 1] = color_array[0].green();
            mBuffer[i + 2] = color_array[0].blue();
        }
    }
    addGlimmer(); // this already draws to output.
}

void IconData::setArrayFade(EColorPreset preset) {
    int colorCount = mDataLayer->arraySize((int)preset);
    QColor *color_array = mDataLayer->colorArray((int)preset);

    int k = 0;
    int tempIndex = 0;
    int *arrayIndices = new int[8];
    while (k < 8) {
        tempIndex = (tempIndex + 1) % colorCount;
        arrayIndices[k] = tempIndex;
        k++;
    }
    int count = 16;
    QColor *output = new QColor[count];
    int colorIndex = 0;
    for (int i = 0; i < count - 2; i = i + 2) {
        output[i] = color_array[arrayIndices[colorIndex]];
        output[i + 1] = getMiddleColor(color_array[arrayIndices[colorIndex]], color_array[arrayIndices[colorIndex + 1]]);
        colorIndex++;
    }
    // wrap the last value around
    output[count - 2] = color_array[arrayIndices[colorIndex]];
    output[count - 1] = getMiddleColor(color_array[arrayIndices[colorIndex]], color_array[arrayIndices[0]]);

    int j = 0;
    for (uint i = 0; i < mBufferLength; i = i + 3) {
        mBuffer[i] = output[j].red();
        mBuffer[i + 1] = output[j].green();
        mBuffer[i + 2] = output[j].blue();
        j++;
    }

    bufferToOutput();
}

void IconData::setArrayRandomSolid(EColorPreset preset) {
    int colorCount = mDataLayer->arraySize((int)preset);
    QColor *color_array = mDataLayer->colorArray((int)preset);

    for (uint i = 0; i < mBufferLength; i = i + 12) {
        QColor randomColor = color_array[rand() % colorCount];
        for (int j = 0; j < 12; j = j + 3) {
            mBuffer[i + j] = randomColor.red();
            mBuffer[i + j + 1] = randomColor.green();
            mBuffer[i + j + 2] = randomColor.blue();
        }
    }
    bufferToOutput();
}

void IconData::setArrayRandomIndividual(EColorPreset preset) {
    if (preset == EColorPreset::eAll) {
        for (uint i = 0; i < mBufferLength; i++) {
            int random = rand() % 256;
            mBuffer[i] = (uchar)random;
        }
    } else {
        int colorCount = mDataLayer->arraySize((int)preset);
        QColor *color_array = mDataLayer->colorArray((int)preset);
        for (uint i = 0; i < mBufferLength; i = i + 3) {
            int index = rand() % colorCount;
            mBuffer[i] = color_array[index].red();
            mBuffer[i + 1] = color_array[index].green();
            mBuffer[i + 2] = color_array[index].blue();
        }
    }
    bufferToOutput();
}

void IconData::setArrayBarsSolid(EColorPreset preset) {
     if (preset == EColorPreset::eAll) {
         for (uint i = 0; i < mBufferLength; i = i + 12) {
             int r = rand() % 256;
             int g = rand() % 256;
             int b = rand() % 256;
             for (int j = 0; j < 12; j = j + 3) {
                 mBuffer[i + j] = (uchar)r;
                 mBuffer[i + j + 1] = (uchar)g;
                 mBuffer[i + j + 2] = (uchar)b;
             }
         }
     } else {
         int colorCount = mDataLayer->arraySize((int)preset);
         QColor *color_array = mDataLayer->colorArray((int)preset);

         int colorIndex = 0;
         for (uint i = 0; i < mBufferLength; i = i + 12) {
             QColor color = color_array[colorIndex % colorCount];
             for (int j = 0; j < 6; j = j + 3) {
                 mBuffer[i + j] = color.red();
                 mBuffer[i + j + 1] = color.green();
                 mBuffer[i + j + 2] = color.blue();
             }
             color = color_array[(colorIndex + 1) % colorCount];
             for (int j = 6; j < 12; j = j + 3) {
                 mBuffer[i + j] = color.red();
                 mBuffer[i + j + 1] = color.green();
                 mBuffer[i + j + 2] = color.blue();
             }
             colorIndex = (colorIndex + 2) % colorCount;
         }
     }
    bufferToOutput();
}

void IconData::setArrayBarsMoving(EColorPreset preset) {
    int colorCount = mDataLayer->arraySize((int)preset);
    QColor *color_array = mDataLayer->colorArray((int)preset);

    int colorIndex = 0;
    QColor color;
    for (uint i = 3; i < mBufferLength; i = i + 12) {
        color = color_array[colorIndex % colorCount];
        for (int j = 0; j < 6; j = j + 3) {
            mBuffer[i + j] = color.red();
            mBuffer[i + j + 1] = color.green();
            mBuffer[i + j + 2] = color.blue();
        }
        color = color_array[(colorIndex + 1) % colorCount];
        for (int j = 6; j < 12; j = j + 3) {
            if (i + j + 2 < mBufferLength) {
                mBuffer[i + j] = color.red();
                mBuffer[i + j + 1] = color.green();
                mBuffer[i + j + 2] = color.blue();
            }
        }
        colorIndex = (colorIndex + 2) % colorCount;
    }
    color = color_array[colorIndex % colorCount];
    mBuffer[0] = color.red();
    mBuffer[1] = color.green();
    mBuffer[2] = color.blue();
    mBuffer[45] = color.red();
    mBuffer[46] = color.green();
    mBuffer[47] = color.blue();
    bufferToOutput();
}

void IconData::addGlimmer() {
    for (int i = 0; i < 5 ; i++) {
        int index = rand() % 16;
        mBuffer[index * 3] = mBuffer[index * 3] / 2;
        mBuffer[index * 3 + 1] = mBuffer[index * 3 + 1] / 2;
        mBuffer[index * 3 + 2] = mBuffer[index * 3 + 2] / 2;
    }
    bufferToOutput();
}

void IconData::addBlink() {
    bool isOn = false;
    for (uint i = 0; i < mBufferLength; i = i + 3) {
        if (isOn) {
            mBuffer[i] = 0;
            mBuffer[i + 1] = 0;
            mBuffer[i + 2] = 0;
        }
        isOn = !isOn;
    }
    bufferToOutput();
}

void IconData::addFade() {
    int k = 1;
    for (int i = 0; i < 12; i = i + 3) {
        for (int j = 0; j < 4; j = j + 1) {
            mBuffer[(j * 12) + i] = mBuffer[(j * 12) + i] / k;
            mBuffer[(j * 12) + i + 1] = mBuffer[(j * 12) + i + 1] / k;
            mBuffer[(j * 12) + i + 2] = mBuffer[(j * 12) + i + 2] / k;
        }
        k++;
    }
    bufferToOutput();
}

uint IconData::dataLength() {
    return mDataLength;
}

uint IconData::width() {
    return mWidth;
}

uint IconData::height() {
    return mHeight;
}

uint8_t* IconData::data() {
    return  mData.data();
}


QImage IconData::renderAsQImage() {
    return QImage(mData.data(), mWidth, mHeight, QImage::Format_RGB888);
}

QPixmap IconData::renderAsQPixmap() {
    return QPixmap::fromImage(renderAsQImage());
}

QColor IconData::getMiddleColor(QColor c_1,
                                QColor c_2) {
    QColor output = {0, 0, 0};
    output.setRed(abs(c_1.red() - c_2.red()) / 2 + std::min(c_1.red(), c_2.red()));
    output.setGreen(abs(c_1.green() - c_2.green()) / 2 + std::min(c_1.green(), c_2.green()));
    output.setBlue(abs(c_1.blue() - c_2.blue()) / 2 + std::min(c_1.blue(), c_2.blue()));
    return output;
}
