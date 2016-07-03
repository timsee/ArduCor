/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "commlayer.h"
#include <QDebug>

CommLayer::CommLayer(QWidget *parent) : QWidget(parent) {
    mUDP  = std::shared_ptr<CommUDP>(new CommUDP());
    mHTTP = std::shared_ptr<CommHTTP>(new CommHTTP());
#ifndef MOBILE_BUILD
    mSerial = std::shared_ptr<CommSerial>(new CommSerial());
#endif //MOBILE_BUILD
    mHue = std::shared_ptr<CommHue>(new CommHue());

    mSettings = new QSettings;

    // check for last connection type
    bool foundPrevious = false;
    int previousType = -1;
    if (mSettings->value(KCommDefaultType).toString().compare("") != 0) {
        previousType = mSettings->value(KCommDefaultType).toInt();
        foundPrevious = true;
        currentCommType((ECommType)previousType);
    } else {
        // no connection found, defaults to hue.
        currentCommType(ECommType::eHue);
    }

    if (foundPrevious && (mSettings->value(kCommDefaultName).toString().compare("") != 0)) {
        QString previousConnection = mSettings->value(kCommDefaultName).toString();
        //set the connection, if needed
        if ((ECommType)previousType == ECommType::eHue) {
            selectHueLight(previousConnection.toInt());
        }
    }

    mCurrentlyDiscovering = false;
    connect(mHue.get()->discovery(), SIGNAL(bridgeDiscoveryStateChanged(int)), this, SLOT(hueDiscoveryUpdate(int)));
    connect(mHue.get(), SIGNAL(hueLightStatesUpdated()), this, SLOT(hueLightStateUpdate()));
}


#ifndef MOBILE_BUILD
void CommLayer::changeSerialPort(QString serialPort) {
    mSerial->connectSerialPort(serialPort);
}
#endif //MOBILE_BUILD


void CommLayer::closeCurrentConnection() {
#ifndef MOBILE_BUILD
    if (mCommType == ECommType::eSerial) {
        mSerial->closeConnection();
    }
#endif //MOBILE_BUILD
}

ECommType CommLayer::currentCommType() {
    return mCommType;
}

void CommLayer::sendMainColorChange(QColor color) {
    if (mCommType == ECommType::eHue) {
        mHue->changeLight(mHue->currentLightIndex(),
                          mData->mainColor().saturation(),
                          (int)(mData->mainColor().value() * (mData->brightness() / 100.0f)),
                          mData->mainColor().hue() * 182);
    } else {
        QString packet = QString("%1,%2,%3,%4").arg(QString::number((int)EPacketHeader::eMainColorChange),
                                                    QString::number(color.red()),
                                                    QString::number(color.green()),
                                                    QString::number(color.blue()));
        sendPacket(packet);
    }
}

void CommLayer::sendArrayColorChange(int index, QColor color) {
    QString packet = QString("%1,%2,%3,%4,%5").arg(QString::number((int)EPacketHeader::eCustomArrayColorChange),
                                                   QString::number(index),
                                                   QString::number(color.red()),
                                                   QString::number(color.green()),
                                                   QString::number(color.blue()));
    sendPacket(packet);
}

void CommLayer::sendRoutineChange(ELightingRoutine routine, int colorGroup) {
    QString packet;
    if (mCommType == ECommType::eHue) {
        hueRoutineChange(routine, colorGroup);
    } else if (colorGroup == -1) {
        packet = QString("%1,%2").arg(QString::number((int)EPacketHeader::eModeChange),
                                      QString::number((int)routine));
        sendPacket(packet);
    } else if (routine == ELightingRoutine::eMultiBarsMoving
              || routine == ELightingRoutine::eMultiBarsSolid
              || routine == ELightingRoutine::eMultiFade
              || routine == ELightingRoutine::eMultiGlimmer
              || routine == ELightingRoutine::eMultiRandomIndividual
              || routine == ELightingRoutine::eMultiRandomSolid) {
        packet = QString("%1,%2,%3").arg(QString::number((int)EPacketHeader::eModeChange),
                                         QString::number((int)routine),
                                         QString::number(colorGroup));
        sendPacket(packet);
    }
}

void CommLayer::sendBrightness(int brightness) {
    if (mCommType == ECommType::eHue) {
        hueBrightness(brightness);
    } else {
        QString packet = QString("%1,%2").arg(QString::number((int)EPacketHeader::eBrightnessChange),
                                              QString::number(brightness));
        sendPacket(packet);
    }
}

void CommLayer::sendSpeed(int speed) {
    QString packet = QString("%1,%2").arg(QString::number((int)EPacketHeader::eSpeedChange),
                                          QString::number(speed));
    sendPacket(packet);
}

void CommLayer::sendCustomArrayCount(int count) {
    QString packet = QString("%1,%2").arg(QString::number((int)EPacketHeader::eCustomColorCountChange),
                                          QString::number(count));
    sendPacket(packet);
}

void CommLayer::sendTimeOut(int timeOut) {
    QString packet = QString("%1,%2").arg(QString::number((int)EPacketHeader::eIdleTimeoutChange),
                                          QString::number(timeOut));
    sendPacket(packet);
}

void CommLayer::sendReset() {
    QString packet = QString("%1,42,71").arg(QString::number((int)EPacketHeader::eResetSettingsToDefaults));
    sendPacket(packet);
}



void CommLayer::currentCommType(ECommType commType) {
    mCommType = commType;
    switch (mCommType)
    {
#ifndef MOBILE_BUILD
        case ECommType::eSerial:
            mSerial->discoverSerialPorts();
            mComm = (CommType*)mSerial.get();
            break;
#endif //MOBILE_BUILD
        case ECommType::eHTTP:
            mComm = (CommType*)mHTTP.get();
            break;
        case ECommType::eUDP:
            mComm = (CommType*)mUDP.get();
            break;
    }
    // save setting to persistent memory
    mSettings->setValue(KCommDefaultType, QString::number((int)mCommType));
    mSettings->sync();
}


void CommLayer::sendPacket(QString packet) {
    switch (mCommType)
    {
#ifndef MOBILE_BUILD
        case ECommType::eSerial:
            mSerial->sendPacket(packet);
            break;
#endif //MOBILE_BUILD
        case ECommType::eHTTP:
            mHTTP->sendPacket(packet);
            break;
        case ECommType::eUDP:
            mUDP->sendPacket(packet);
            break;
    }
}

bool CommLayer::isConnected() {
    switch (mCommType)
    {
#ifndef MOBILE_BUILD
        case ECommType::eSerial:
            return mSerial->isConnected();
#endif //MOBILE_BUILD
        case ECommType::eHue:
            return mHue->discovery()->isConnected();
        case ECommType::eHTTP:
        case ECommType::eUDP:
           // these don't contain connections, default to true
            return true;
    }
}

void CommLayer::startDiscovery() {
    if (mCommType == ECommType::eHue) {
        mCurrentlyDiscovering = true;
        mHue->discovery()->startBridgeDiscovery();
    }
}

void CommLayer::stopDiscovery() {
    mCurrentlyDiscovering = false;
    mHue->discovery()->stopBridgeDiscovery();
}



void CommLayer::hueBrightness(int brightness) {
    if (mData->currentRoutine() <= ELightingRoutine::eSingleSineFade) {
        QColor color = mData->mainColor();
        mHue->changeLight(mHue->currentLightIndex(), color.saturation(), (int)(color.value() * (brightness / 100.0f)), color.hue() * 182);
    } else {
        QColor averageColor = mData->colorsAverage(mData->currentColorGroup());
        mHue->changeLight(mHue->currentLightIndex(), averageColor.saturation(), (int)(averageColor.value() * (brightness / 100.0f)), averageColor.hue() * 182);
    }
}

void CommLayer::hueRoutineChange(ELightingRoutine routine, int colorGroup) {
    if (colorGroup == -1) {
        if ((int)routine == 0) {
            mHue->turnOff(mHue->currentLightIndex());
        } else {
            mHue->changeLight(mHue->currentLightIndex(),
                              mData->mainColor().saturation(),
                              mData->mainColor().value(),
                              mData->mainColor().hue() * 182);
        }
    } else {
        QColor averageColor = mData->colorsAverage((EColorGroup)colorGroup);
        mHue->changeLight(mHue->currentLightIndex(),
                          averageColor.saturation(),
                          (int)(averageColor.value() * (mData->brightness() / 100.0f)),
                          averageColor.hue() * 182);

    }
}

void CommLayer::selectHueLight(int i) {
    mHue->currentIndex(i);
    mSettings->setValue(kCommDefaultName, QString::number(i));
    mSettings->sync();
}

const QString CommLayer::KCommDefaultType = QString("CommDefaultType");
const QString CommLayer::kCommDefaultName = QString("CommDefaultName");


