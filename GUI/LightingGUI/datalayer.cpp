/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "datalayer.h"

DataLayer::DataLayer() {
    resetToDefaults();
}

DataLayer::~DataLayer() {

}

bool DataLayer::isOn(bool isOn) {
    mIsOn = isOn;
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


bool DataLayer::color(QColor newColor) {
    mColor = newColor;
    return true;
}

QColor DataLayer::color() {
    return mColor;
}

bool DataLayer::currentMode(ELightingMode mode) {
    if ((int)mode >= 0 && (int)mode < 23) {
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


bool DataLayer::colorCount(int count) {
    if (count > 0) {
        mColorCount = count;
        return true;
    } else {
        return false;
    }
}


int DataLayer::colorCount() {
    return mColorCount;
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
    mCurrentMode = ELightingMode::eLightingModeSingleGlimmer;
    mTimeOut = 120;
    mBrightness = 50;
    mColorCount = 5;
    mSpeed = 300;
    mColor = QColor(0,255,0);
    colors = std::vector<QColor>(mColorCount, QColor(0,0,0));
    for (int i = 0; i < mColorCount; i = i + 5) {
        colors[i]     = QColor(0,    255, 0);
        colors[i + 1] = QColor(125,  0,   255);
        colors[i + 2] = QColor(0,    0,   255);
        colors[i + 3] = QColor(40,   127, 40);
        colors[i + 4] = QColor(60,   0,   160);
    }
}
