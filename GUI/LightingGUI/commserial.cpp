#ifndef MOBILE_BUILD

/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "commserial.h"

#include <QDebug>

CommSerial::CommSerial() {
    setupConnectionList(ECommType::eSerial);
    serial = std::shared_ptr<QSerialPort>(new QSerialPort);
    mIsConnected = false;
}

CommSerial::~CommSerial() {
    saveConnectionList();
    if (mIsConnected) {
        closeConnection();
    }
}


void CommSerial::discoverSerialPorts() {
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
            if (!QString::compare(info.portName(), QString("cu.Bluetooth-Incoming-Port"))) {
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
        if (!isSpecialCase) {
            addConnection(info.portName());
        }
    }
    // if none is connected, attempt automatic connection
    if (!mIsConnected && (numberOfConnections() > 0)) {
        int index = 0;
        while (!mIsConnected && index < numberOfConnections()) {
            selectConnection(index);
            mIsConnected = connectSerialPort(currentConnection());
            index++;
        }
    }
}

void CommSerial::sendPacket(QString packet) {
    QString packetString = packet + ";";
    serial->write(packetString.toStdString().c_str());
}


void CommSerial::closeConnection() {
    if (serial->isOpen()) {
        serial->clear();
        serial->close();
    }
}


bool CommSerial::connectSerialPort(QString serialPortName) {
    // close a preexisting connection if it exists
    if (mIsConnected) {
        closeConnection();
    }
    bool serialPortFound = false;
    QSerialPortInfo connectInfo;
    if (!QString::compare(serial->portName(), serialPortName)) {
        // its already connected, no need to connect again
        return true;
    }
    for (QSerialPortInfo savedInfo : serialList) {
        if (!QString::compare(savedInfo.portName(), serialPortName)) {
            serialPortFound = true;
            connectInfo = savedInfo;
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
        qDebug() << "Serial Port Connected!" << connectInfo.portName();
        return true;
    } else {
        qDebug() << "serial port failed" << serial->errorString();
        return false;
    }
}

#endif //MOBILE_BUILD
