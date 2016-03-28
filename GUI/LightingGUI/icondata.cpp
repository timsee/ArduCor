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
    mData = std::unique_ptr<std::vector<uint8_t> >(new std::vector<uint8_t>(mDataLength, 0));

    mBufferWidth = 4;
    mBufferHeight = 4;
    mBufferLength = mBufferWidth * mBufferHeight * 3;
    mBuffer = std::unique_ptr<std::vector<uint8_t> >(new std::vector<uint8_t>(mBufferLength, 0));

    // zero out the arrays
    for (uint i = 0; i < mBufferLength; i ++) {
        (*mBuffer.get())[i] = 0;
    }
    for (uint i = 0; i < mDataLength; i ++) {
        (*mData.get())[i] = 0;
    }
}

void IconData::bufferToOutput() {
    uint j = 0;
    uint k = 0;

    for (uint i = 0; i < mDataLength; i = i + 3) {
        if ((i % (mWidth * 3)) < mWidth * 3 / 4) {
            (*mData.get())[i] = (*mBuffer.get())[j];
            (*mData.get())[i + 1] = (*mBuffer.get())[j + 1];
            (*mData.get())[i + 2] = (*mBuffer.get())[j + 2];
        }
        else if ((i % (mWidth * 3)) < mWidth * 6 / 4) {
             (*mData.get())[i] = (*mBuffer.get())[j + 3];
             (*mData.get())[i + 1] = (*mBuffer.get())[j + 4];
             (*mData.get())[i + 2] = (*mBuffer.get())[j + 5];
        }
        else if ((i % (mWidth * 3)) < mWidth * 9 / 4) {
             (*mData.get())[i] = (*mBuffer.get())[j + 6];
             (*mData.get())[i + 1] = (*mBuffer.get())[j + 7];
             (*mData.get())[i + 2] = (*mBuffer.get())[j + 8];
        }
        else if ((i % (mWidth * 3)) < mWidth * 3) {
             (*mData.get())[i] = (*mBuffer.get())[j + 9];
             (*mData.get())[i + 1] = (*mBuffer.get())[j + 10];
             (*mData.get())[i + 2] = (*mBuffer.get())[j + 11];
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
        (*mBuffer.get())[i] = color.red();
        (*mBuffer.get())[i + 1] = color.green();
        (*mBuffer.get())[i + 2] = color.blue();
    }
    bufferToOutput();
}

void IconData::setRandomColors() {
    for (uint i = 0; i < mBufferLength; i++) {
        int random = rand() % 256;
        (*mBuffer.get())[i] = (uchar)random;
    }
    bufferToOutput();
}

void IconData::setArrayColors() {
    if (mDataLayer == NULL) {
        qDebug() << "ERROR: the data layer is not set, cannot use the arary colors!";
    }
    int colorCount = mDataLayer->colorCount();
    int j = 0;
    for (uint i = 0; i < mBufferLength; i = i + 3) {
        (*mBuffer.get())[i] = mDataLayer->colors[j].red();
        (*mBuffer.get())[i + 1] = mDataLayer->colors[j].green();
        (*mBuffer.get())[i + 2] = mDataLayer->colors[j].blue();
        j = (j + 1) % colorCount;
    }
    bufferToOutput();
}


void IconData::setRandomSolid() {
    for (uint i = 0; i < mBufferLength; i = i + 12) {
        int r = rand() % 256;
        int g = rand() % 256;
        int b = rand() % 256;
        for (int j = 0; j < 12; j = j + 3) {
            (*mBuffer.get())[i + j] = (uchar)r;
            (*mBuffer.get())[i + j + 1] = (uchar)g;
            (*mBuffer.get())[i + j + 2] = (uchar)b;
        }
    }
    bufferToOutput();
}

void IconData::setFadeAllColors() {
    //TODO: convert to QColor and use those instead of our struct
    QColor RED = QColor(255, 0, 0);
    QColor ORANGE = QColor(255, 60, 0);
    QColor YELLOW = QColor(255, 255, 0);
    QColor GREEN = QColor(0, 255, 0);
    QColor TEAL = QColor(0, 255, 80);
    QColor BLUE = QColor(0, 0, 255);
    QColor PURPLE = QColor(80, 0, 255);
    QColor PINK = QColor(255, 0, 255);

    QColor *colors = setupFadeGroup(RED, ORANGE, YELLOW, GREEN,
                                    TEAL, BLUE, PURPLE,  PINK);
    int j = 0;
    for (uint i = 0; i < mBufferLength; i = i + 3) {
        (*mBuffer.get())[i] = colors[j].red();
        (*mBuffer.get())[i + 1] = colors[j].green();
        (*mBuffer.get())[i + 2] = colors[j].blue();
        j++;
    }
    bufferToOutput();
}

void IconData::setArrayGlimmer() {
    for (uint i = 0; i < mBufferLength; i = i + 3) {
        int shouldChange = rand() % 100;
        if (shouldChange <= 20) {
            int index = rand() % mDataLayer->colorCount();
            (*mBuffer.get())[i] = mDataLayer->colors[index].red();
            (*mBuffer.get())[i + 1] = mDataLayer->colors[index].green();
            (*mBuffer.get())[i + 2] = mDataLayer->colors[index].blue();
        } else {
            (*mBuffer.get())[i] = mDataLayer->colors[0].red();
            (*mBuffer.get())[i + 1] = mDataLayer->colors[0].green();
            (*mBuffer.get())[i + 2] = mDataLayer->colors[0].blue();
        }
    }
    addGlimmer(); // this already draws to output.
}

void IconData::setArrayFade() {
    QColor *colors = setupFadeGroup(mDataLayer->colors[0], mDataLayer->colors[1], mDataLayer->colors[2], mDataLayer->colors[3],
                                    mDataLayer->colors[4], mDataLayer->colors[0], mDataLayer->colors[1],  mDataLayer->colors[2]);
    int j = 0;
    for (uint i = 0; i < mBufferLength; i = i + 3) {
        (*mBuffer.get())[i] = colors[j].red();
        (*mBuffer.get())[i + 1] = colors[j].green();
        (*mBuffer.get())[i + 2] = colors[j].blue();
        j++;
    }
    bufferToOutput();
}

void IconData::setArrayRandomSolid() {
    for (uint i = 0; i < mBufferLength; i = i + 12) {
        QColor randomColor = mDataLayer->colors[rand() % mDataLayer->colorCount()];
        for (int j = 0; j < 12; j = j + 3) {
            (*mBuffer.get())[i + j] = randomColor.red();
            (*mBuffer.get())[i + j + 1] = randomColor.green();
            (*mBuffer.get())[i + j + 2] = randomColor.blue();
        }
    }
    bufferToOutput();
}

void IconData::setArrayRandomIndividual() {
    for (uint i = 0; i < mBufferLength; i = i + 3) {
        int index = rand() % mDataLayer->colorCount();
        (*mBuffer.get())[i] = mDataLayer->colors[index].red();
        (*mBuffer.get())[i + 1] = mDataLayer->colors[index].green();
        (*mBuffer.get())[i + 2] = mDataLayer->colors[index].blue();
    }
    bufferToOutput();
}

void IconData::setArrayBarsSolid() {
    int colorIndex = 0;
    for (uint i = 0; i < mBufferLength; i = i + 12) {
        QColor color = mDataLayer->colors[colorIndex % mDataLayer->colorCount()];
        for (int j = 0; j < 6; j = j + 3) {
            (*mBuffer.get())[i + j] = color.red();
            (*mBuffer.get())[i + j + 1] = color.green();
            (*mBuffer.get())[i + j + 2] = color.blue();
        }
        color = mDataLayer->colors[(colorIndex + 1) % mDataLayer->colorCount()];
        for (int j = 6; j < 12; j = j + 3) {
            (*mBuffer.get())[i + j] = color.red();
            (*mBuffer.get())[i + j + 1] = color.green();
            (*mBuffer.get())[i + j + 2] = color.blue();
        }
        colorIndex = (colorIndex + 2) % mDataLayer->colorCount();
    }
    bufferToOutput();
}

void IconData::setArrayBarsMoving() {
    int colorIndex = 0;
    QColor color;
    for (uint i = 3; i < mBufferLength; i = i + 12) {
        color = mDataLayer->colors[colorIndex % mDataLayer->colorCount()];
        for (int j = 0; j < 6; j = j + 3) {
            (*mBuffer.get())[i + j] = color.red();
            (*mBuffer.get())[i + j + 1] = color.green();
            (*mBuffer.get())[i + j + 2] = color.blue();
        }
        color = mDataLayer->colors[(colorIndex + 1) % mDataLayer->colorCount()];
        for (int j = 6; j < 12; j = j + 3) {
            (*mBuffer.get())[i + j] = color.red();
            (*mBuffer.get())[i + j + 1] = color.green();
            (*mBuffer.get())[i + j + 2] = color.blue();
        }
        colorIndex = (colorIndex + 2) % mDataLayer->colorCount();
    }
    color = mDataLayer->colors[colorIndex % mDataLayer->colorCount()];
    (*mBuffer.get())[0] = color.red();
    (*mBuffer.get())[1] = color.green();
    (*mBuffer.get())[2] = color.blue();
    (*mBuffer.get())[45] = color.red();
    (*mBuffer.get())[46] = color.green();
    (*mBuffer.get())[47] = color.blue();
    bufferToOutput();
}

void IconData::addGlimmer() {
    for (int i = 0; i < 5 ; i++) {
        int index = rand() % 16;
        (*mBuffer.get())[index * 3] = (*mBuffer.get())[index * 3] / 2;
        (*mBuffer.get())[index * 3 + 1] = (*mBuffer.get())[index * 3 + 1] / 2;
        (*mBuffer.get())[index * 3 + 2] = (*mBuffer.get())[index * 3 + 2] / 2;
    }
    bufferToOutput();
}

void IconData::addBlink() {
    bool isOn = false;
    for (uint i = 0; i < mBufferLength; i = i + 3) {
        if (isOn) {
            (*mBuffer.get())[i] = 0;
            (*mBuffer.get())[i + 1] = 0;
            (*mBuffer.get())[i + 2] = 0;
        }
        isOn = !isOn;
    }
    bufferToOutput();
}

void IconData::addFade() {
    int k = 1;
    for (int i = 0; i < 12; i = i + 3) {
        for (int j = 0; j < 4; j = j + 1) {
            (*mBuffer.get())[(j * 12) + i] = (*mBuffer.get())[(j * 12) + i] / k;
            (*mBuffer.get())[(j * 12) + i + 1] = (*mBuffer.get())[(j * 12) + i + 1] / k;
            (*mBuffer.get())[(j * 12) + i + 2] = (*mBuffer.get())[(j * 12) + i + 2] / k;
        }
        k++;
    }
    bufferToOutput();
}

uint IconData::getDataLength() {
    return mDataLength;
}

uint IconData::getWidth() {
    return mWidth;
}

uint IconData::getHeight() {
    return mHeight;
}

uint8_t* IconData::getData() {
    return  &(*mData.get())[0];
}


QImage IconData::renderAsQImage() {
    return QImage(&(*mData.get())[0], mWidth, mHeight, QImage::Format_RGB888);
}

QPixmap IconData::renderAsQPixmap() {
    return QPixmap::fromImage(renderAsQImage());
}


QColor *IconData::setupFadeGroup(QColor c_1, QColor c_2,
                                 QColor c_3, QColor c_4,
                                 QColor c_5, QColor c_6,
                                 QColor c_7, QColor c_8) {
    QColor *output = new QColor[16];
    output[0] = c_1;
    output[1] = getMiddleColor(c_1, c_2);
    output[2] = c_2;
    output[3] = getMiddleColor(c_2, c_3);
    output[4] = c_3;
    output[5] = getMiddleColor(c_3, c_4);
    output[6] = c_4;
    output[7] = getMiddleColor(c_4, c_5);
    output[8] = c_5;
    output[9] = getMiddleColor(c_5, c_6);
    output[10] = c_6;
    output[11] = getMiddleColor(c_6, c_7);
    output[12] = c_7;
    output[13] = getMiddleColor(c_7, c_8);
    output[14] = c_8;
    output[15] = getMiddleColor(c_8, c_1);
    return output;
}

QColor IconData::getMiddleColor(QColor c_1,
                                QColor c_2) {
    QColor output = {0, 0, 0};
    output.setRed(abs(c_1.red() - c_2.red()) / 2 + std::min(c_1.red(), c_2.red()));
    output.setGreen(abs(c_1.green() - c_2.green()) / 2 + std::min(c_1.green(), c_2.green()));
    output.setBlue(abs(c_1.blue() - c_2.blue()) / 2 + std::min(c_1.blue(), c_2.blue()));
    return output;
}
