/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "datalayer.h"
#include <QDebug>

DataLayer::DataLayer() {
    int i = 0;

    //==========
    // Custom Colors
    //==========
    mArraySizes[i] = 10;
    mColors[i] =  new QColor[mArraySizes[i]];
    i++;

    //==========
    // Water Colors
    //==========

    mArraySizes[i] = 5;
    mColors[i] = new QColor[mArraySizes[i]];
    mColors[i][0] = QColor(0,   0, 255);
    mColors[i][1] = QColor(0,   25,  225);
    mColors[i][2] = QColor(0,   0,   127);
    mColors[i][3] = QColor(0,   127, 127);
    mColors[i][4] = QColor(120, 120, 255);
    i++;

    //==========
    // Frozen Colors
    //==========
    mArraySizes[i] = 6;
    mColors[i] = new QColor[mArraySizes[i]];
    mColors[i][0] = QColor(0,   127, 255);
    mColors[i][1] = QColor(0,   127, 127);
    mColors[i][2] = QColor(200, 0,   255);
    mColors[i][3] = QColor(40,  127, 40);
    mColors[i][4] = QColor(127, 127, 127);
    mColors[i][5] = QColor(127, 127, 255);
    i++;

    //==========
    // Snow Colors
    //==========
    mArraySizes[i] = 6;
    mColors[i] = new QColor[mArraySizes[i]];
    mColors[i][0] = QColor(255, 255, 255);
    mColors[i][1] = QColor(127, 127, 127);
    mColors[i][2] = QColor(200, 200, 200);
    mColors[i][3] = QColor(0,   0,   255);
    mColors[i][4] = QColor(0,  255,  255);
    mColors[i][5] = QColor(0,  180,   180);
    i++;

    //==========
    // Cool Colors
    //==========
    mArraySizes[i] = 5;
    mColors[i] = new QColor[mArraySizes[i]];
    mColors[i][0] = QColor(0,   255, 0);
    mColors[i][1] = QColor(125, 0,   255);
    mColors[i][2] = QColor(0,   0,   255);
    mColors[i][3] = QColor(40,  127, 40);
    mColors[i][4] = QColor(60,  0,   160);
    i++;

    //==========
    // Warm Colors
    //==========
    mArraySizes[i] = 5;
    mColors[i] = new QColor[mArraySizes[i]];
    mColors[i][0] = QColor(255, 255, 0);
    mColors[i][1] = QColor(255, 0,   0);
    mColors[i][2] = QColor(255, 45,  0);
    mColors[i][3] = QColor(255, 200,  0);
    mColors[i][4] = QColor(255, 127, 0);
    i++;

    //==========
    // Fire Colors
    //==========
    mArraySizes[i] = 9;
    mColors[i] = new QColor[mArraySizes[i]];
    mColors[i][0] = QColor(255, 75,  0);
    mColors[i][1] = QColor(255, 20,  0);
    mColors[i][2] = QColor(255, 80,  0);
    mColors[i][3] = QColor(255, 5,   0);
    mColors[i][4] = QColor(0,   0,   0);
    mColors[i][5] = QColor(127, 127, 0);
    mColors[i][6] = QColor(255, 60,  0);
    mColors[i][7] = QColor(255, 45,  0);
    mColors[i][8] = QColor(127, 127, 0);

    i++;

    //==========
    // Evil Colors
    //==========
    mArraySizes[i] = 7;
    mColors[i] = new QColor[mArraySizes[i]];
    mColors[i][0] = QColor(255, 0, 0);
    mColors[i][1] = QColor(200, 0, 0);
    mColors[i][2] = QColor(127, 0, 0);
    mColors[i][3] = QColor(20,  0, 0);
    mColors[i][4] = QColor(30,  0, 40);
    mColors[i][5] = QColor(0,   0, 0);
    mColors[i][6] = QColor(80,  0, 0);
    i++;

    //==========
    // Corrosive Colors
    //==========
    mArraySizes[i] = 5;
    mColors[i] = new QColor[mArraySizes[i]];
    mColors[i][0] = QColor(0,   255, 0);
    mColors[i][1] = QColor(0,   200, 0);
    mColors[i][2] = QColor(60,  180,  60);
    mColors[i][3] = QColor(127, 135, 127);
    mColors[i][4] = QColor(10,  255,   10);
    i++;

    //==========
    // Poison Colors
    //==========
    mArraySizes[i] = 9;
    mColors[i] = new QColor[mArraySizes[i]];
    mColors[i][0] = QColor(80, 0, 180);
    mColors[i][1] = QColor(120, 0, 255);
    mColors[i][2] = QColor(0, 0,   0);
    mColors[i][3] = QColor(25, 0,  25);
    mColors[i][4] = QColor(60, 60,  60);
    mColors[i][5] = QColor(120, 0, 255);
    mColors[i][6] = QColor(80,  0, 180);
    mColors[i][7] = QColor(40,  0, 90);
    mColors[i][8] = QColor(80,  0, 180);

    i++;

    //==========
    // Rose
    //==========
    mArraySizes[i] = 6;
    mColors[i] = new QColor[mArraySizes[i]];
    mColors[i][0] = QColor(216, 30,  100);
    mColors[i][1] = QColor(255, 245, 251);
    mColors[i][2] = QColor(156, 62,  72);
    mColors[i][3] = QColor(127, 127, 127);
    mColors[i][4] = QColor(194, 30,  86);
    mColors[i][5] = QColor(194, 30,  30);

    i++;

    //==========
    // Pink Green
    //==========
    mArraySizes[i] = 4;
    mColors[i] = new QColor[mArraySizes[i]];
    mColors[i][0]  = QColor(255, 20,  147);
    mColors[i][1]  = QColor(0,   255, 0);
    mColors[i][2]  = QColor(0,   200, 0);
    mColors[i][3]  = QColor(255, 105, 180);
    i++;

    //==========
    // Red White Blue
    //==========
    mArraySizes[i] = 4;
    mColors[i] = new QColor[mArraySizes[i]];
    mColors[i][0]  = QColor(255, 255, 255);
    mColors[i][1]  = QColor(255, 0,   0);
    mColors[i][2]  = QColor(0,   0,   255);
    mColors[i][3]  = QColor(255, 255, 255);
    i++;

    //==========
    // RGB
    //==========
    mArraySizes[i] = 3;
    mColors[i] = new QColor[mArraySizes[i]];
    mColors[i][0] = QColor(255, 0,   0);
    mColors[i][1] = QColor(0,   255, 0);
    mColors[i][2] = QColor(0,   0,   255);

    i++;

    //==========
    // CMY
    //==========
    mArraySizes[i] = 3;
    mColors[i] = new QColor[mArraySizes[i]];
    mColors[i][0] = QColor(255, 255, 0);
    mColors[i][1] = QColor(0,   255, 255);
    mColors[i][2] = QColor(255,   0, 255);

    i++;

    //==========
    // Six Color
    //==========
    mArraySizes[i] = 6;
    mColors[i] = new QColor[mArraySizes[i]];
    mColors[i][0] = QColor(255, 0,   0);
    mColors[i][1] = QColor(255, 255, 0);
    mColors[i][2] = QColor(0,   255, 0);
    mColors[i][3] = QColor(0,   255, 255);
    mColors[i][4] = QColor(0,   0,   255);
    mColors[i][5] = QColor(255, 0,   255);
    i++;

    //==========
    // Seven Color
    //==========
    mArraySizes[i] = 7;
    mColors[i] = new QColor[mArraySizes[i]];
    mColors[i][0] = QColor(255, 0,   0);
    mColors[i][1] = QColor(255, 255, 0);
    mColors[i][2] = QColor(0,   255, 0);
    mColors[i][3] = QColor(0,   255, 255);
    mColors[i][4] = QColor(0,   0,   255);
    mColors[i][5] = QColor(255, 0,   255);
    mColors[i][6] = QColor(255, 255, 255);

    i++;

    //==========
    // All Colors
    //==========
    mArraySizes[i] = 12;
    mColors[i] = new QColor[mArraySizes[i]];
    for (int j = 0; j < mArraySizes[i]; j++) {
        mColors[i][j] = QColor(rand() % 256, rand() % 256, rand() % 256);
    }
    i++;


    resetToDefaults();
}

DataLayer::~DataLayer() {

}

bool DataLayer::isOn(bool isOn) {
    mIsOn = isOn;
    return true;
}

bool DataLayer::isOn() {
    return mIsOn;
}

bool DataLayer::brightness(int brightness) {
    if (brightness >= 0 && brightness <= 100) {
        mBrightness = brightness;
        return true;
    } else {
        return false;
    }
}

int DataLayer::brightness() {
    return mBrightness;
}


bool DataLayer::mainColor(QColor newColor) {
    mMainColor = newColor;
    return true;
}

QColor DataLayer::mainColor() {
    return mMainColor;
}


uint8_t DataLayer::arraySize(int index) {
    if (index < (int)EColorPreset::eColorPreset_MAX) {
        return mArraySizes[index];
    } else {
        return 0;
    }
}

QColor* DataLayer::colorArray(int index) {
    if (index < (int)EColorPreset::eColorPreset_MAX) {
        return mColors[index];
    } else {
        return nullptr;
    }
}

QColor* DataLayer::customArray() {
    return mColors[0];
}


QColor DataLayer::colorsAverage() {
    int r = 0;
    int g = 0;
    int b = 0;
    for (int i = 0; i < mColorsUsed; ++i) {
       r = r + mColors[0][i].red();
       g = g + mColors[0][i].green();
       b = b + mColors[0][i].blue();
    }
    return QColor(r / mColorsUsed,
                  g / mColorsUsed,
                  b / mColorsUsed);
}


bool DataLayer::currentMode(ELightingMode mode) {
    if ((int)mode >= 0 && (int)mode < (int)ELightingMode::eLightingMode_MAX) {
        mCurrentMode = mode;
        return true;
    } else {
        return false;
    }
}

ELightingMode  DataLayer::currentMode() {
    return mCurrentMode;
}

bool DataLayer::timeOut(int timeOut) {
    if (timeOut >= 0) {
        mTimeOut = timeOut;
        return true;
    } else {
        return false;
    }
}

int DataLayer::timeOut() {
    return mTimeOut;
}


bool DataLayer::preset(EColorPreset preset) {
    if (preset < EColorPreset::eColorPreset_MAX) {
        mPreset = preset;
        return true;
    } else {
        return false;
    }
}

EColorPreset DataLayer::preset() {
    return mPreset;
}

bool DataLayer::setupSerial(QString serial) {
    mSerialPort = serial;
    return true;
}

QString DataLayer::serialPort() {
    return mSerialPort;
}

// NYI: UDP
bool DataLayer::setupUDP(QString ip, int port) {
    mIpAddress = ip;
    mUDPPort = port;
    return true;
}

// NYI: UDP
int DataLayer::UDPPort() {
    return mUDPPort;
}

// NYI: UDP
QString DataLayer::IP() {
    return mIpAddress;
}


bool DataLayer::customColorCount(int count) {
    if (count > 0) {
        mArraySizes[0] = count;
        return true;
    } else {
        return false;
    }
}

int DataLayer::customColorCount() {
    return mArraySizes[0];
}


bool DataLayer::colorsUsed(int colorsUsed) {
    if ((colorsUsed >= 0) && (colorsUsed <= mArraySizes[0])) {
        mColorsUsed = colorsUsed;
        return true;
    } else {
        return false;
    }
}

int DataLayer::colorsUsed() {
    return mColorsUsed;
}

bool DataLayer::speed(int speed) {
    if (speed > 0) {
        mSpeed = speed;
        return true;
    } else {
        return false;
    }
}


int DataLayer::speed() {
    return mSpeed;
}


void DataLayer::resetToDefaults() {
    mCurrentMode = ELightingMode::eSingleGlimmer;
    mTimeOut = 120;
    mBrightness = 50;
    mColorsUsed = 2;
    mSpeed = 300;
    mMainColor = QColor(0,255,0);

    int j = 0;
    int custom_count = 5;
    for (int i = 0; i < mArraySizes[0]; i++) {
        if ((j % custom_count) == 0) {
            mColors[0][i] = QColor(0,    255, 0);
        } else if ((j % custom_count) == 1) {
            mColors[0][i] = QColor(125,  0,   255);
        } else if ((j % custom_count) == 2) {
            mColors[0][i] = QColor(0,    0,   255);
        } else if ((j % custom_count) == 3) {
            mColors[0][i] = QColor(40,   127, 40);
        } else if ((j % custom_count) == 4) {
            mColors[0][i] = QColor(60,   0,   160);
        }
        j++;
    }

}
