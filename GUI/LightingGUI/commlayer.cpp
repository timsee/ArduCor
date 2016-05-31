/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "commlayer.h"
#include <QDebug>

// set this as what you want your GUI to default to.
#define DEFAULT_COMM_TYPE ECommType::eSerial

CommLayer::CommLayer() {
    mUDP  = std::shared_ptr<CommUDP>(new CommUDP());
    mHTTP = std::shared_ptr<CommHTTP>(new CommHTTP());

#ifndef MOBILE_BUILD
    mSerial = std::shared_ptr<CommSerial>(new CommSerial());
#endif //MOBILE_BUILD
    currentCommType(DEFAULT_COMM_TYPE);
}

CommLayer::~CommLayer() {

}

#ifndef MOBILE_BUILD

void CommLayer::changeSerialPort(QString serialPort) {
    mSerial->connectSerialPort(serialPort);
}

#endif //MOBILE_BUILD

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
}

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
    QString packet = QString("%1,%2,%3,%4").arg(QString::number((int)EPacketHeader::eMainColorChange),
                                                QString::number(color.red()),
                                                QString::number(color.green()),
                                                QString::number(color.blue()));
    sendPacket(packet);
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
    if (colorGroup == -1){
        packet = QString("%1,%2").arg(QString::number((int)EPacketHeader::eModeChange),
                                      QString::number((int)routine));
    } else {
        if (routine == ELightingRoutine::eMultiBarsMoving
              || routine == ELightingRoutine::eMultiBarsSolid
              || routine == ELightingRoutine::eMultiFade
              || routine == ELightingRoutine::eMultiGlimmer
              || routine == ELightingRoutine::eMultiRandomIndividual
              || routine == ELightingRoutine::eMultiRandomSolid) {
            packet = QString("%1,%2,%3").arg(QString::number((int)EPacketHeader::eModeChange),
                                             QString::number((int)routine),
                                             QString::number(colorGroup));
        }
    }
    sendPacket(packet);
}

void CommLayer::sendBrightness(int brightness) {
    QString packet = QString("%1,%2").arg(QString::number((int)EPacketHeader::eBrightnessChange),
                                          QString::number(brightness));
    sendPacket(packet);
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
