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
    setupConnectionList(ECommType::eUDP);
    mSocket = new QUdpSocket();
}

CommUDP::~CommUDP() {
    saveConnectionList();
}

void CommUDP::sendPacket(QString packet) {
    // Wow, it doesn't take much code to send UDP packets in Qt...
    mSocket->writeDatagram(packet.toUtf8().data(),              // payload
                           QHostAddress(currentConnection()),   // IP address
                           PORT);                               // Port
}
