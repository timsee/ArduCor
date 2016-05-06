/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "commlayer.h"
#include <QDebug>

CommLayer::CommLayer() {
    bool waitingForConnection = true;
    serial = std::shared_ptr<QSerialPort>(new QSerialPort);

    // Example use QSerialPortInfo
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        bool serialPortFound = false;
        for (QSerialPortInfo savedInfo : serialList) {
            if (!QString::compare(info.portName(), savedInfo.portName())) {
                serialPortFound = true;
            }
        }

        bool isSpecialCase = false;
        if (!serialPortFound) {
            if (!QString::compare(info.portName(), QString("Bluetooth-Incoming-Port"))) {
                isSpecialCase = true;
            }
            if(!QString::compare(info.portName(), QString("COM1"))) {
                isSpecialCase = true;
            }
            if (!isSpecialCase) {
                serialList.append(info);
            }
        }

        qDebug() << "Name : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();
        if (waitingForConnection && !isSpecialCase) {
            if (connectSerialPort(info.portName())) waitingForConnection = false;
        }
    }
}

CommLayer::~CommLayer() {

}

bool CommLayer::connectSerialPort(QString serialPortName) {
    bool serialPortFound = false;
    QSerialPortInfo connectInfo;
    if (!QString::compare(serial->portName(), serialPortName)) {
        // its already connected, no need to connect again
        return true;
    }
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        for (QSerialPortInfo savedInfo : serialList) {
            if (!QString::compare(savedInfo.portName(), serialPortName)) {
                serialPortFound = true;
                connectInfo = savedInfo;
            }
        }
    }
    if (!serialPortFound) {
       qDebug() << "Serial port not found";
       return false;
    }

    serial->setPort(connectInfo);
    if (serial->open(QIODevice::ReadWrite)) {
        serial->setBaudRate(QSerialPort::Baud19200);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setParity(QSerialPort::NoParity);
        serial->setDataBits(QSerialPort::Data8);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        // set up lights to the defaults
        sendMainColorChange(QColor(0,255,0));
        sendRoutineChange(ELightingRoutine::eSingleGlimmer);
        sendBrightness(50);
        sendTimeOut(120);
        sendSpeed(300); // desired FPS * 100
        qDebug() << "Serial Port Connected!" << connectInfo.portName();
        return true;
    } else {
        qDebug() << "serial port failed" << serial->errorString() << "\n";
        return false;
    }
}

void CommLayer::closeSerialPort() {
    if (serial->isOpen()) {
        serial->clear();
        serial->close();
    }
}

void CommLayer::sendMainColorChange(QColor color) {
    QString message = QString("%1,%2,%3,%4;").arg(QString::number((int)EPacketHeader::eMainColorChange),
                                                  QString::number(color.red()),
                                                 QString::number(color.green()),
                                                 QString::number(color.blue()));
    serial->write(message.toStdString().c_str());
}

void CommLayer::sendArrayColorChange(int index, QColor color) {
    QString message = QString("%1,%2,%3,%4,%5;").arg(QString::number((int)EPacketHeader::eCustomArrayColorChange),
                                                     QString::number(index),
                                                    QString::number(color.red()),
                                                    QString::number(color.green()),
                                                    QString::number(color.blue()));
    serial->write(message.toStdString().c_str());
}

void CommLayer::sendRoutineChange(ELightingRoutine routine, int colorGroup) {
    if (colorGroup == -1){
        QString message = QString("%1,%2;").arg(QString::number((int)EPacketHeader::eModeChange),
                                                QString::number((int)routine));
        serial->write(message.toStdString().c_str());
    } else {
        if (routine == ELightingRoutine::eMultiBarsMoving
              || routine == ELightingRoutine::eMultiBarsSolid
              || routine == ELightingRoutine::eMultiFade
              || routine == ELightingRoutine::eMultiGlimmer
              || routine == ELightingRoutine::eMultiRandomIndividual
              || routine == ELightingRoutine::eMultiRandomSolid) {
            QString message = QString("%1,%2,%3;").arg(QString::number((int)EPacketHeader::eModeChange),
                                                       QString::number((int)routine),
                                                       QString::number(colorGroup));
            serial->write(message.toStdString().c_str());
        }
    }
}

void CommLayer::sendBrightness(int brightness) {
    QString message = QString("%1,%2;").arg(QString::number((int)EPacketHeader::eBrightnessChange),
                                            QString::number(brightness));
    serial->write(message.toStdString().c_str());
}

void CommLayer::sendSpeed(int speed) {
    QString message = QString("%1,%2;").arg(QString::number((int)EPacketHeader::eSpeedChange),
                                            QString::number(speed));
    serial->write(message.toStdString().c_str());
}

void CommLayer::sendCustomArrayCount(int count) {
    QString message = QString("%1,%2;").arg(QString::number((int)EPacketHeader::eCustomColorCountChange),
                                            QString::number(count));
    serial->write(message.toStdString().c_str());
}

void CommLayer::sendTimeOut(int timeOut) {
    QString message = QString("%1,%2;").arg(QString::number((int)EPacketHeader::eIdleTimeoutChange),
                                            QString::number(timeOut));
    serial->write(message.toStdString().c_str());
}

void CommLayer::sendReset() {
    QString message = QString("%1,42,71;").arg(QString::number((int)EPacketHeader::eResetSettingsToDefaults));
    serial->write(message.toStdString().c_str());
}
