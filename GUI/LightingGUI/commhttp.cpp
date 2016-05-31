/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "commhttp.h"

CommHTTP::CommHTTP() {
    setupConnectionList(ECommType::eHTTP);
    mNetworkManager = new QNetworkAccessManager;
}

CommHTTP::~CommHTTP() {
    saveConnectionList();
}

void CommHTTP::sendPacket(QString packet) {
    // add the packet to the URL address.
    QString urlString = "http://" + currentConnection() + "/arduino/" + packet;
    mNetworkManager->get(QNetworkRequest(QUrl(urlString)));
}
