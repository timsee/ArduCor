/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "commudp.h"

#include <QDebug>

// port used by the server
#define PORT 10008

CommUDP::CommUDP() {

}


void CommUDP::setup(QString param1) {
    mSocket = new QUdpSocket();
    // assign the IP address to the name parameter.
    name(param1);
}


void CommUDP::sendPacket(QString packet) {
    // Wow, it doesn't take much code to write UDP packets in Qt...
    mSocket->writeDatagram(packet.toUtf8().data(),   // payload
                           QHostAddress(name()),     // IP address
                           PORT);                    // Port
}


void CommUDP::closeConnection() {
    // UDP is connection-less so nothing needs to be done here,
    // This function is just added to comply with being a CommType.
}
