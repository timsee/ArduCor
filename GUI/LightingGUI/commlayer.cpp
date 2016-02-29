/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#include "commlayer.h"
#include <QDebug>

CommLayer::CommLayer() {
    bool waitingForConnection = true;
    serial = new QSerialPort;

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
        sendMainColorChange(0,255,0);
        sendModeChange(DataLayer::eLightingModeSingleGlimmer);
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

void CommLayer::sendMainColorChange(int r, int g, int b) {
    QString message = QString("1,%1,%2,%3;").arg(QString::number(r), QString::number(g), QString::number(b));
    serial->write(message.toStdString().c_str());
}

void CommLayer::sendArrayColorChange(int color, int r, int g, int b) {
    QString message = QString("2,%1,%2,%3,%4;").arg(QString::number(color), QString::number(r), QString::number(g), QString::number(b));
    serial->write(message.toStdString().c_str());
}

void CommLayer::sendModeChange(DataLayer::ELightingMode mode) {
    QString message = QString("0,%1;").arg(QString::number((int)mode));
    serial->write(message.toStdString().c_str());
}

void CommLayer::sendArrayModeChange(DataLayer::ELightingMode mode, int count) {
    if (mode == DataLayer::ELightingMode::eLightingModeSavedBarsMoving
          || mode == DataLayer::ELightingMode::eLightingModeSavedBarsSolid
          || mode == DataLayer::ELightingMode::eLightingModeSavedFade
          || mode == DataLayer::ELightingMode::eLightingModeSavedGlimmer
          || mode == DataLayer::ELightingMode::eLightingModeSavedRandomIndividual
          || mode == DataLayer::ELightingMode::eLightingModeSavedRandomSolid) {
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
