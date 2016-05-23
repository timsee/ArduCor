/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "commhttp.h"

CommHTTP::CommHTTP() {

}


void CommHTTP::setup(QString param1) {
    mNetworkManager = new QNetworkAccessManager;
    // assign the IP address to the name parameter.
    name(param1);
}


void CommHTTP::sendPacket(QString packet) {
    // add the packet to the URL address.
    QString urlString = "http://" + name() + "/arduino/" + packet;
    mNetworkManager->get(QNetworkRequest(QUrl(urlString)));
}


void CommHTTP::closeConnection() {
    // HTTP has a connection, but it does not maintain it after a packet
    // is sent, making this function useless.
    // It is just added to comply with being a CommType
}
