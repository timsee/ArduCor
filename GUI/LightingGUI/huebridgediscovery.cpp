#include "huebridgediscovery.h"
/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 */

HueBridgeDiscovery::HueBridgeDiscovery(QObject *parent) : QObject(parent)
{
    mDiscoveryTimer = new QTimer;
    connect(mDiscoveryTimer, SIGNAL(timeout()), this, SLOT(testBridgeIP()));

    mTimeoutTimer = new QTimer;
    connect(mTimeoutTimer, SIGNAL(timeout()), this, SLOT(handleDiscoveryTimeout()));

    mUPnPSocket = new QUdpSocket(this);
    connect(mUPnPSocket, SIGNAL(readyRead()), this, SLOT(readPendingUPnPDatagrams()));

    mNetworkManager = new QNetworkAccessManager;
    connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    mSettings = new QSettings;
    // check for bridge IP in app memory
    if (mSettings->value(kPhillipsIPAddress).toString().compare("") != 0) {
        mHasIP = true;
        mBridge.IP = mSettings->value(kPhillipsIPAddress).toString();
    } else {
        mHasIP = false;
        mBridge.IP = QString("");
    }

    // check for bridge username in app memory
    if (mSettings->value(kPhillipsUsername).toString().compare("") != 0) {
        mHasKey = true;
        mBridge.username = mSettings->value(kPhillipsUsername).toString();
    } else {
        mHasKey = false;
        mBridge.username = QString("");
    }

    // assume that all saved data is not valid until its checked.
    mIPValid = false;
    mUsernameValid = false;

    if (mHasKey && mHasIP) {
        //qDebug() << "Connection data is already saved, testing full connection";
        attemptFinalCheck();
    }
}

HueBridgeDiscovery::~HueBridgeDiscovery() {
    if(mDiscoveryTimer->isActive()) {
        mDiscoveryTimer->stop();
    }
    if(mTimeoutTimer->isActive()) {
        mTimeoutTimer->stop();
    }
}

void HueBridgeDiscovery::startBridgeDiscovery() {
    if (isConnected()) {
        mDiscoveryState = EHueDiscoveryState::eBridgeConnected;
        emit bridgeDiscoveryStateChanged((int)mDiscoveryState);
    }
    if (!mHasIP) {
        mDiscoveryState = EHueDiscoveryState::eFindingIpAddress;
        emit bridgeDiscoveryStateChanged((int)mDiscoveryState);
        // Attempt both UPnP and NUPnP asynchronously
        // UPnP polls a standard multicast UDP address on your network
        // while NUPnP does a HTTP GET request to a website set up
        // by Phillips that returns a JSON value that contains
        // all the Bridges on your network.
        attemptUPnPDiscovery();
        attemptNUPnPDiscovery();
    } else if(!mHasKey) {
         attemptSearchForUsername();
    } else if (mHasKey) {
        //had key after getting IP, test both together. This time
        // if it fails, invalidate the username instead of the prelearned
        attemptFinalCheck();
    }
}

void HueBridgeDiscovery::stopBridgeDiscovery() {
    if (mDiscoveryTimer->isActive()) {
        mDiscoveryTimer->stop();
    }
    if (mUPnPSocket->isOpen()) {
        mUPnPSocket->close();
    }
}

// ----------------------------
// Private Slots
// ----------------------------


void HueBridgeDiscovery::testBridgeIP() {
     QString urlString = "http://" + mBridge.IP + "/api";
     QNetworkRequest request = QNetworkRequest(QUrl(urlString));
     request.setHeader(QNetworkRequest::ContentTypeHeader,
                       QStringLiteral("text/html; charset=utf-8"));
     mNetworkManager->post(request, "{\"devicetype\":\"my_hue_app#corluma device\"}");
}


void HueBridgeDiscovery::replyFinished(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QString string = (QString)reply->readAll();
        //qDebug() << "Response:" << string;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(string.toUtf8());
        // check validity of the document
        if(!jsonResponse.isNull())
        {
            if(jsonResponse.isObject()) {
                if (!mIPValid || !mUsernameValid) {
                    mDiscoveryState = EHueDiscoveryState::eBridgeConnected;
                    emit bridgeDiscoveryStateChanged((int)mDiscoveryState);
                    mIPValid = true;
                    mUsernameValid = true;
                    emit connectionStatusChanged(true);
                    stopBridgeDiscovery();
                }
            }
            else if(jsonResponse.isArray()) {
                QJsonObject outsideObject = jsonResponse.array().at(0).toObject();
                if (outsideObject.value("error").isObject()) {
                    if (!mIPValid) {
                        attemptSearchForUsername();
                        mIPValid = true;
                    }
                    // error packets are sent when a message cannot be parsed
                    QJsonObject innerObject = outsideObject.value("error").toObject();
                    if (innerObject.value("description").isString()) {
                        QString description = innerObject.value("description").toString();
                       // qDebug() << "Description" << description;
                    }
                } else if (outsideObject.value("success").isObject()) {
                    // success packets are sent when a message is parsed and the Hue react in some  way.
                    QJsonObject innerObject = outsideObject.value("success").toObject();
                    if (innerObject.value("username").isString()) {
                        if (!mIPValid) {
                            mIPValid = true;
                        }
                        mBridge.username = innerObject.value("username").toString();
                        mHasKey = true;
                        qDebug() << "Discovered username:" << mBridge.username;

                        // save the username into persistent memory so it can be accessed in
                        // future sessions of the application.
                        mSettings->setValue(kPhillipsUsername, mBridge.username);
                        mSettings->sync();

                        // at this point you should have a valid IP and now need to just check
                        // the username.
                        attemptFinalCheck();
                    }
                } else if (outsideObject.value("internalipaddress").isString()) {
                    if (!mHasIP) {
                        // Used by N-UPnP, this gives the IP address of the Hue bridge
                        // if a GET is sent to https://www.meethue.com/api/nupnp
                        mBridge.IP = outsideObject.value("internalipaddress").toString();
                        // spawn a discovery timer
                        mHasIP = true;
                        qDebug() << "discovered IP via NUPnP: " << mBridge.IP;
                        // future sessions of the application.
                        mSettings->setValue(kPhillipsIPAddress, mBridge.IP);
                        mSettings->sync();

                        if (mHasKey) {
                            attemptFinalCheck();
                        } else {
                            mDiscoveryState = EHueDiscoveryState::eTestingIPAddress;
                            emit bridgeDiscoveryStateChanged((int)mDiscoveryState);
                            mTimeoutTimer->start(4000);
                            // call bridge discovery again, now that we have an IP
                            startBridgeDiscovery();
                        }
                    }

                } else {
                    qDebug() << "Document is an array, but we don't recognize it...";
                }
            }
            else {
                qDebug() << "Document is not an object";
            }
        }
        else {
            qDebug() << "Invalid JSON...";
        }
    }
}

void HueBridgeDiscovery::readPendingUPnPDatagrams() {
    while (mUPnPSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(mUPnPSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        mUPnPSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        QString payload = QString::fromUtf8(datagram);
        //qDebug() << "this is the payload" << payload;
        if(payload.contains(QString("IpBridge")) && !mHasIP) {
            mBridge.IP = sender.toString();
            mHasIP = true;
            qDebug() << "discovered IP via UPnP: " << mBridge.IP;
            // future sessions of the application.
            mSettings->setValue(kPhillipsIPAddress, mBridge.IP);
            mSettings->sync();
            //TODO: handle MAC Address in this case...

            if (mHasKey) {
                attemptFinalCheck();
            } else {
                mTimeoutTimer->stop();
                mDiscoveryState = EHueDiscoveryState::eTestingIPAddress;
                emit bridgeDiscoveryStateChanged((int)mDiscoveryState);
                mTimeoutTimer->start(4000);
                // call bridge discovery again, now that we have an IP
                startBridgeDiscovery();
            }

        }

    }
}


void HueBridgeDiscovery::handleDiscoveryTimeout() {
    if (mDiscoveryState == EHueDiscoveryState::eFindingIpAddress) {
        qDebug() << "UPnP timed out...";
        // leave UPnP bound, but call the NUPnP again just in case...
        attemptNUPnPDiscovery();
        attemptUPnPDiscovery();
        // TODO: prompt the user if this state gets hit too many times?
    } else if (mDiscoveryState == EHueDiscoveryState::eTestingIPAddress) {
        // search for IP again, the one we have is no longer valid.
        qDebug() << "IP Address not valid";
        mHasIP = false;
        startBridgeDiscovery();
    }  else if (mDiscoveryState == EHueDiscoveryState::eTestingFullConnection) {
        // first test if the IP address is valid
        if (!mIPValid && !mUsernameValid) {
            // if nothings validated, check IP address
            qDebug() << "full connection failed, test the IP address...";
            mHasIP = false;
            emit connectionStatusChanged(false);
            startBridgeDiscovery();
        } else if (mIPValid) {
            // if we have a valid IP address but haven't validated
            // the key, check the key.
            qDebug() << "full connection failed, but IP address works, check the username.";
            mHasKey = false;
        } else if (mUsernameValid) {
            qDebug() << "We have a key and an IP validated but hasvent gotten packets how coudl this be?";
            qDebug() << "this state makes no sense, invalidate both";
            mHasKey = false;
            mHasIP = false;
        } else {
            qDebug() << "Error: Something went wrong with a full connection test...";
        }
        stopBridgeDiscovery();
    }
}


// ----------------------------
// Private Discovery Attempts
// ----------------------------

void HueBridgeDiscovery::attemptUPnPDiscovery() {
    QHostAddress standardUPnPAddress = QHostAddress(QString("239.255.255.250"));
    // timeout after waiting 5 seconds
    if (mTimeoutTimer->isActive()) {
        mTimeoutTimer->stop();
    }
    mTimeoutTimer->start(5000);
    // used for discovery
    if (mUPnPSocket->state() == QAbstractSocket::UnconnectedState) {
        mUPnPSocket->bind(standardUPnPAddress, 1900, QUdpSocket::ShareAddress);
        mUPnPSocket->joinMulticastGroup(standardUPnPAddress);
    }
}

void HueBridgeDiscovery::attemptNUPnPDiscovery() {
    // start bridge IP discovery
    QString urlString = "https://www.meethue.com/api/nupnp";
    QNetworkRequest request = QNetworkRequest(QUrl(urlString));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("text/html; charset=utf-8"));
    mNetworkManager->get(request);
}

void HueBridgeDiscovery::attemptFinalCheck() {
    //create the start of the URL
    QString urlString = "http://" + mBridge.IP + "/api/" + mBridge.username;

    if (mTimeoutTimer->isActive()) {
        mTimeoutTimer->stop();
    }
    mTimeoutTimer->start(5000);

    QNetworkRequest request = QNetworkRequest(QUrl(urlString));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("text/html; charset=utf-8"));
    mNetworkManager->get(request);

    mDiscoveryState = EHueDiscoveryState::eTestingFullConnection;
    emit bridgeDiscoveryStateChanged((int)mDiscoveryState);
    // no more need for discovery packets, we've been discovered!
    stopBridgeDiscovery();
}

void HueBridgeDiscovery::attemptSearchForUsername() {
    mTimeoutTimer->stop();
    mDiscoveryState = EHueDiscoveryState::eFindingDeviceUsername;
    emit bridgeDiscoveryStateChanged((int)mDiscoveryState);
    // start bridge username discovery
    mDiscoveryTimer->start(1000);
}

// ----------------------------
// Settings Keys
// ----------------------------

const QString HueBridgeDiscovery::kPhillipsUsername = QString("PhillipsBridgeUsername");
const QString HueBridgeDiscovery::kPhillipsIPAddress = QString("PhillipsBridgeIPAddress");


