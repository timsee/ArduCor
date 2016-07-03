#ifndef COMMHUE_H
#define COMMHUE_H

#include "commtype.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QWidget>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include "huebridgediscovery.h"

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 */

/*!
 * \brief The SHueLight struct a struct that stores all the relevant
 *        data received from a state update from the bridge.
 */
struct SHueLight {
    /*!
     * \brief isReachable true if we can communicate with it, false otherwise
     */
    bool isReachable;
    /*!
     * \brief isOn true if the light is shining any color, false if turned
     *        off by software. By using a combination of isReachable and isOn
     *        you can determine if the light is on and shining, off by software
     *        and thus able to be turned on by software again, or off by hardware
     *        and needs the light switch to be hit in order to turn it on.
     */
    bool isOn;
    /*!
     * \brief index the index of the hue, each bridge gives an index for all of the
     *        connected hues.
     */
    int index;
    /*!
     * \brief hue the hue of the color of the light from the Hue. Range is between 0 and 65535
     */
    int hue;
    /*!
     *  \bnrief brightness the brightness of the light from the Hue. Range is between 0 and 254.
     */
    int brightness;
    /*!
     * \brief saturation the saturation of the light from the Hue. Range is between 0 and 254.
     */
    int saturation;
    /*!
     * \brief type the type of Hue product connected.
     */
    QString type;
    /*!
     * \brief uniqueID a unique identifier of that particular light.
     */
    QString uniqueID;
};


/*!
 * \brief The CommHue class communicates with a Phillips Hue Bridge to control
 *        all of the currently connected Hue Lights.
 *
 *        It uses a HueBridgeDiscovery object to store data about the Hue Bridge
 *        and to run discovery methods if none is currently connected. Once a connection
 *        is established, it uses its mStateUpdateTimer to poll for updates from the Bridge
 *        about the current lights. This object also acts as a layer that takes the protocols from
 *        the rest of the application, and converts it to packets that the Bridge can understand.
 */
class CommHue : public CommType
{
     Q_OBJECT
public:
    /*!
     * \brief CommHue Constructor
     */
    CommHue();

    /*!
     * \brief CommHue Deconstructor
     */
    ~CommHue();

    /*!
     * \brief changeLight send a packet to a hue bridge to change the color of a given hue light.
     * \param lightIndex the index of the hue being changed.
     * \param saturation how saturated the color is. A higher number leads to more saturation. Must be in the range of 0 and 254, inclusive.
     * \param brightness how bright the light will be. A higher number leads to more brightness. Must be in the range of 0 and 254, inclusive.
     * \param hue the hue of the hue light's color. Must be in the range of 0 and 65535, with 0 representing pure red.
     */
    void changeLight(int lightIndex, int saturation, int brightness, int hue);

    /*!
     * \brief currentIndex sets the index of the currently controlled Hue from the hue bridge.
     * \param index the new index to control.
     */
    void currentIndex(int index);

    /*!
     * \brief currentLightIndex getter for the current hue index
     * \return the current hue index.
     */
    int currentLightIndex() { return mCurrentIndex; }

    /*!
     * \brief turnOn turns on the Hue light at a given index
     * \param lightIndex the index of the hue you want to turn on.
     */
    void turnOn(int lightIndex);

    /*!
     * \brief turnOff turns off the Hue light at a given index
     * \param lightIndex the index of the hue you want to turn off.
     */
    void turnOff(int lightIndex);

    /*!
     * \brief connectedHues returns vector of the currently connected Hue lights. Used by the settings page
     *        to display the list.
     * \return  a vector of the currently connected hue lights.
     */
    std::vector<SHueLight> connectedHues() { return mConnectedHues; }

    /*!
     * \brief discovery returns a pointer to the object used to discover the Hue Bridge. This can be used
     *        to connect to its signals or to check its current state.
     * \return the discovery object for finding the Hue Bridge.
     */
    HueBridgeDiscovery *discovery() { return mDiscovery; }

signals:
    /*!
     * \brief hueLightStatesUpdated signals whenever the light states have updated.
     */
    void hueLightStatesUpdated();

private slots:

    /*!
     * \brief replyFinished called whenever the mNetworkManager receives a response to its HTTP requests.
     *        Used for parsing the responses.
     */
    void replyFinished(QNetworkReply*);

    /*!
     * \brief updateLightStates called on a timer continually to poll the states of the Hue lights.
     */
    void updateLightStates();

    /*!
     * \brief connectionStatusHasChanged called by the HueBridgeDiscovery object whenever its connection
     *        changes.
     */
    void connectionStatusHasChanged(bool);

private:
    /*!
     * \brief mNetworkManager Qt's HTTP connection object
     */
    QNetworkAccessManager *mNetworkManager;

    /*!
     * \brief mStateUpdateTimer Continually polls the Hue Bridge for any state changes with its connected
     *        lights every few seconds.
     */
    QTimer *mStateUpdateTimer;

    /*!
     * \brief mDiscovery object used to discover and connect to a Hue Bridge.
     */
    HueBridgeDiscovery *mDiscovery;

    /*!
     * \brief a vector of the struct that contains the states of the connected Hue
     *        lights. This is maintained by a timer which updates this vector every
     *        few seconds.
     */
    std::vector<SHueLight> mConnectedHues;

    /*!
     * \brief mCurrentIndex index of whatever light is currently geting controlled.
     */
    int mCurrentIndex;

    /*!
     * \brief mUrlStart Every packet sent to the hue bridge starts with
     *        this string, which is formatted as http://$BRIDGEIP/api/$USERNAME
     */
    QString mUrlStart;

    /*!
     * \brief updateHueLightState used by the parser for network replies, this method
     *        takes a JsonObject that should represent all the current states of a Hue
     *        light, verifies that its values look correct. If all values are valid,
     *        it updates mConnectedHues with a new struct representing the updated values.
     * \param object the JSON representation of the Hue light's state.
     * \param i The index of the hue.
     * \param true if object is valid and the states are updated, false otherwise.
     */
    bool updateHueLightState(QJsonObject object, int i);
};

#endif // COMMHUE_H
