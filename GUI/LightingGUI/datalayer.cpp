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

bool DataLayer::setBrightness(int brightness) {
    if (brightness >= 0 && brightness <= 100) {
        mBrightness = brightness;
        return true;
    } else {
        return false;
    }
}

int DataLayer::getBrightness() {
    return mBrightness;
}

bool DataLayer::setCurrentMode(ELightingMode mode) {
    if (mode >= 0 && mode < 23) {
        mCurrentMode = mode;
        return true;
    } else {
        return false;
    }
}

DataLayer::ELightingMode DataLayer::getCurrentMode() {
    return mCurrentMode;
}

bool DataLayer::setTimeOut(int timeOut) {
    if (timeOut >= 0) {
        mTimeOut = timeOut;
        return true;
    } else {
        return false;
    }
}

int DataLayer::getTimeOut() {
    return mTimeOut;
}

bool DataLayer::setupSerial(QString serial) {
    mSerialPort = serial;
    return true;
}

QString DataLayer::getSerialPort() {
    return mSerialPort;
}

// NYI: UDP
bool DataLayer::setupUDP(QString ip, int port) {
    mIpAddress = ip;
    mUDPPort = port;
    return true;
}

// NYI: UDP
int DataLayer::getUDPPort() {
    return mUDPPort;
}

// NYI: UDP
QString DataLayer::getIP() {
    return mIpAddress;
}


bool DataLayer::setColorCount(int count) {
    if (count > 0) {
        mColorCount = count;
        return true;
    } else {
        return false;
    }
}


int DataLayer::getColorCount() {
    return mColorCount;
}


bool DataLayer::setSpeed(int speed) {
    if (speed > 0) {
        mSpeed = speed;
        return true;
    } else {
        return false;
    }
}


int DataLayer::getSpeed() {
    return mSpeed;
}


void DataLayer::resetToDefaults() {
    mCurrentMode = eLightingModeSingleGlimmer;
    mTimeOut = 120;
    mBrightness = 50;
    mColorCount = 5;
    mSpeed = 300;
    colors = new Color[mColorCount];
    for (int i = 0; i < mColorCount; i = i + 5) {
        colors[i] =     {0,    255, 0};
        colors[i + 1] = {125,  0,   255};
        colors[i + 2] = {0,    0,   255};
        colors[i + 3] = {40,   127, 40};
        colors[i + 4] = {60,   0,   160};
    }
}
