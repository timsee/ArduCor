#include "commhue.h"

#include <QJsonValue>
#include <QJsonDocument>
#include <QVariantMap>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 */

CommHue::CommHue() {
    setupConnectionList(ECommType::eHue);

    mDiscovery = new HueBridgeDiscovery;
    connect(mDiscovery, SIGNAL(connectionStatusChanged(bool)), this, SLOT(connectionStatusHasChanged(bool)));

    mNetworkManager = new QNetworkAccessManager;
    connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    mStateUpdateTimer = new QTimer;
    connect(mStateUpdateTimer, SIGNAL(timeout()), this, SLOT(updateLightStates()));

    mConnectedHues = std::vector<SHueLight>(0);
    mCurrentIndex = 1;
}

CommHue::~CommHue() {
    saveConnectionList();
}

void CommHue::currentIndex(int index) {
    mCurrentIndex = index;
}

void CommHue::changeLight(int lightIndex, int saturation, int brightness, int hue) {
    if (discovery()->isConnected()) {
        QString urlString = mUrlStart + "/lights/" + QString::number(lightIndex) + "/state";
        if (saturation > 254) {
            saturation = 254;
        }
        if (brightness > 254) {
            brightness = 254;
        }

        QString body = "{\"on\":true ,\"sat\":" + QString::number(saturation) + ", \"bri\":" + QString::number(brightness) + ",\"hue\":" + QString::number(hue) + "}";
        //qDebug() << "Sending to" << urlString << " with packet " << body;
        mNetworkManager->put(QNetworkRequest(QUrl(urlString)), body.toStdString().c_str());
    }
}

void CommHue::turnOn(int lightIndex) {
    if (discovery()->isConnected()) {
        QString urlString = mUrlStart + "/lights/" + QString::number(lightIndex) + "/state";
        mNetworkManager->put(QNetworkRequest(QUrl(urlString)), "{\"on\":true}");
    }
}

void CommHue::turnOff(int lightIndex) {
    if (discovery()->isConnected()) {
        QString urlString = mUrlStart + "/lights/" + QString::number(lightIndex) + "/state";
        mNetworkManager->put(QNetworkRequest(QUrl(urlString)), "{\"on\":false}");
    }
}

void CommHue::replyFinished(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QString string = (QString)reply->readAll();
        //qDebug() << "Response:" << string;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(string.toUtf8());
        // check validity of the document
        if(!jsonResponse.isNull())
        {
            if(jsonResponse.isObject()) {
                QJsonObject object = jsonResponse.object();
                // update state LEDs
                for (int i = 0; i < 10; ++i) {
                    if (object.value(QString::number(i)).isObject()) {
                        updateHueLightState(object.value(QString::number(i)).toObject(), i);
                    }
                }
            }
            else if(jsonResponse.isArray()) {
                QJsonObject outsideObject = jsonResponse.array().at(0).toObject();
                if (outsideObject.value("error").isObject()) {

                } else if (outsideObject.value("success").isObject()) {

                }  else {
                    qDebug() << "its our job to enable them";
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

void CommHue::updateLightStates() {
    QString urlString = mUrlStart + "/lights/";
    QNetworkRequest request = QNetworkRequest(QUrl(urlString));
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("text/html; charset=utf-8"));
    mNetworkManager->get(request);
}

bool CommHue::updateHueLightState(QJsonObject object, int i) {
    // check if valid packet
    if (object.value("type").isString()
            && object.value("uniqueid").isString()
            && object.value("state").isObject()) {
        QJsonObject stateObject = object.value("state").toObject();
        if (stateObject.value("on").isBool()
                && stateObject.value("reachable").isBool()
                && stateObject.value("hue").isDouble()
                && stateObject.value("bri").isDouble()
                && stateObject.value("sat").isDouble()) {

            // packet passes valdiity check, set all values
            SHueLight light;
            light.isReachable = stateObject.value("reachable").toBool();
            light.isOn = stateObject.value("on").toBool();
            light.index = i;
            light.brightness = stateObject.value("bri").isDouble();
            light.hue = stateObject.value("hue").isDouble();
            light.saturation = stateObject.value("saturation").isDouble();
            light.type = object.value("type").toString();
            light.uniqueID = object.value("uniqueid").toString();

            // update vector with new values
            if (light.index > mConnectedHues.size()) {
                mConnectedHues.resize(light.index);
                mConnectedHues[i - 1] = light;
            } else {
                //qDebug() << "update" << i;
                mConnectedHues[i - 1] = light;
            }
            emit hueLightStatesUpdated();
            return true;
        } else {
            qDebug() << "Invalid parameters...";
        }
    } else {
        qDebug() << "Invalid parameters...";
    }
    return false;
 }

void CommHue::connectionStatusHasChanged(bool status) {
    // always stop the timer if its active, we'll restart if its a new connection
    if (mStateUpdateTimer->isActive()) {
        mStateUpdateTimer->stop();
    }
    if (status) {
        updateLightStates();
        SHueBridge bridge = mDiscovery->bridge();
        mUrlStart = "http://" + bridge.IP + "/api/" + bridge.username;
        mStateUpdateTimer->start(3000);
        // call update method immediately
        updateLightStates();
    }
}
