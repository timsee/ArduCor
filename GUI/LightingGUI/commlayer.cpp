/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "commlayer.h"
#include <QDebug>

/*!
 * \todo save the previous IP address and have it set through the GUI
 *       instead of as a hardcoded value
 */
#define SERVER_IP "192.168.0.125"

// set this as what you want your GUI to default to.
#define DEFAULT_COMM_TYPE ECommType::eUDP

CommLayer::CommLayer() {
    mCommType = DEFAULT_COMM_TYPE;

#ifndef MOBILE_BUILD
    mSerial = std::shared_ptr<CommSerial>(new CommSerial());
    bool serialSetup = false;
#endif //MOBILE_BUILD
    mHTTP = std::shared_ptr<CommHTTP>(new CommHTTP());
    mUDP  = std::shared_ptr<CommUDP>(new CommUDP());

    switch (mCommType)
    {
#ifndef MOBILE_BUILD
        case ECommType::eSerial:
            mSerial->setup("");
            mComm = (CommType*)mSerial.get();
            serialSetup = true;
            break;
#endif //MOBILE_BUILD
        case ECommType::eHTTP:
            mHTTP->setup(QString(SERVER_IP));
            mComm = (CommType*)mHTTP.get();
            break;
        case ECommType::eUDP:
            mUDP->setup(QString(SERVER_IP));
            mComm = (CommType*)mUDP.get();
            break;
    }

#ifndef MOBILE_BUILD
    if (!serialSetup) {
        mSerial->setup("");
    }
#endif //MOBILE_BUILD
}

CommLayer::~CommLayer() {

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
