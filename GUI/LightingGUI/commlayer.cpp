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
        sendModeChange(ELightingMode::eLightingModeSingleGlimmer);
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
    QString message = QString("1,%1,%2,%3;").arg(QString::number(color.red()),
                                                 QString::number(color.green()),
                                                 QString::number(color.blue()));
    serial->write(message.toStdString().c_str());
}

void CommLayer::sendArrayColorChange(int index, QColor color) {
    QString message = QString("2,%1,%2,%3,%4;").arg(QString::number(index),
                                                    QString::number(color.red()),
                                                    QString::number(color.green()),
                                                    QString::number(color.blue()));
    serial->write(message.toStdString().c_str());
}

void CommLayer::sendModeChange(ELightingMode mode) {
    QString message = QString("0,%1;").arg(QString::number((int)mode));
    serial->write(message.toStdString().c_str());
}

void CommLayer::sendArrayModeChange(ELightingMode mode, int count) {
    if (mode == ELightingMode::eLightingModeArrayBarsMoving
          || mode == ELightingMode::eLightingModeArrayBarsSolid
          || mode == ELightingMode::eLightingModeArrayFade
          || mode == ELightingMode::eLightingModeArrayGlimmer
          || mode == ELightingMode::eLightingModeArrayRandomIndividual
          || mode == ELightingMode::eLightingModeArrayRandomSolid) {
        QString message = QString("0,%1,%2;").arg(QString::number((int)mode), QString::number(count));
        serial->write(message.toStdString().c_str());
    }
}

void CommLayer::sendBrightness(int brightness) {
    QString message = QString("3,%1;").arg(QString::number(brightness));
    serial->write(message.toStdString().c_str());
}

void CommLayer::sendSpeed(int speed) {
    QString message = QString("4,%1;").arg(QString::number(speed));
    serial->write(message.toStdString().c_str());
}

void CommLayer::sendTimeOut(int timeOut) {
    QString message = QString("5,%1;").arg(QString::number(timeOut));
    serial->write(message.toStdString().c_str());
}
