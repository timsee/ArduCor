
#ifndef COMMLAYER_H
#define COMMLAYER_H

#include "lightingprotocols.h"
#include <QColor>
#include <memory>
#include <QWidget>
#include <QTimer>
#include "datalayer.h"

#ifndef MOBILE_BUILD
#include "commserial.h"
#endif //MOBILE_BUILD
#include "commhttp.h"
#include "commudp.h"
#include "commhue.h"

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 */

/*!
 * \brief The CommLayer class provides communication protocols
 *  that allow the user to connect and send packets to an LED
 *  array. Currently it supports serial, UDP, and HTTP.
 *
 */
class CommLayer : public QWidget
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor
     */
    CommLayer(QWidget *parent = 0);

    /*!
     * \brief sendMainColorChange change the main color of the lighting settings
     *        in the GUI, this is the color displayed in the leftmost menu.
     * \param color a QColor representation of the color being used for single LED Routines.
     */
    void sendMainColorChange(QColor color);

    /*!
     * \brief sendColorChange change an array color in the lighting system
     * \param index index of array color
     * \param color the color being sent for the given index
     */
    void sendArrayColorChange(int index, QColor color);

    /*!
     * \brief sendRoutineChange change the mode of the lights. The mode changes
     *        how the lights look. some modes are a single color, some modes are random colors
     *        and some use a saved array.
     * \param routine the mode being sent to the LED system
     * \param colorGroupUsed -1 if single color routine, otherwise a EColorGroup.
     */
    void sendRoutineChange(ELightingRoutine routine, int colorGroupUsed = -1);

    /*!
     * \brief sendCustomArrayCount sends a new custom array count to the LED array. This count
     *        determines how many colors from the custom array should be used. It is different
     *        from the size of the custom array, which provides a maximum possible amount
     *        of colors.
     * \param count a value less than the size of the custom color array.
     */
    void sendCustomArrayCount(int count);

    /*!
     * \brief sendBrightness sends a brightness value between 0 and 100, with 100 being full brightness.
     * \param brightness a value between 0 and 100
     */
    void sendBrightness(int brightness);

    /*!
     * \brief sendSpeed sends a desired FPS for light updates. This value is the FPS * 100,
     *        for example if you want a FPS of 5, send the value 500.
     * \param speed the FPS multiplied by 100.
     */
    void sendSpeed(int speed);

    /*!
     * \brief sendTimeOut the amount of minutes that it takes for the LEDs to turn themselves off from
     *        inactivity. Perfect for bedtime!
     * \param timeOut a number greater than 0
     */
    void sendTimeOut(int timeOut);

    /*!
     * \brief sendReset resets the board to its default settings.
     */
    void sendReset();

    /*!
     * \brief comm returns a pointer to the current connection
     * \return a pointer to the current connection
     */
    CommType *comm() { return mComm; }

    /*!
     * \brief sendPacket sends the string over the currently
     *        active connection
     * \param packet a string that will be sent over the currently
     *        active connection.
     */
    void sendPacket(QString packet);

    /*!
     * \brief currentCommType sets the current comm type
     * \param commType the desired comm type
     */
    void currentCommType(ECommType commType);

    /*!
     * \brief closeCurrentConnection required only for serial connections, closes
     *        the current connectio before trying to open a new one.
     */
    void closeCurrentConnection();

    /*!
     * \brief currentCommType getting for the current comm type.
     * \return
     */
    ECommType currentCommType();

    /*!
     * \brief mData pointer to the data layer
     */
    DataLayer *mData;

    /*!
     * \brief startDiscovery start the discovery methods of the current ECommType
     */
    void startDiscovery();

    /*!
     * \brief isInDiscoveryMode returns true if in discovery mode, false otherwise.
     * \return true if in discovery mode, false otherwise.
     */
    bool isInDiscoveryMode() { return mCurrentlyDiscovering; }

    /*!
     * \brief stopDiscovery stop the discovery methods of the current ECommType
     */
    void stopDiscovery();

    /*!
     * \brief isConnected returns true if a connection has been estbalished
     *        with the given parameters for the current communication type.
     *        If the communication type is connectionless, this returns
     *        true.
     * \return true if a conenction has been established for the current
     *         communication type or if the communication type is connectionaless,
     *         false otherwise.
     */
    bool isConnected();

    // --------------------------
    // Hardware specific functions
    // --------------------------
    //TODO: most of these should be turned into generalized systems that
    //      work for whatever comm types they apply to...

#ifndef MOBILE_BUILD
    /*!
     * \brief changeSerialPort attempts to change to the serial port with
     *        the same name as the argument given.
     * \param serialPort the name of the serial port to connect to.
     */
    void changeSerialPort(QString serialPort);
#endif //MOBILE_BUILD

    /*!
     * \brief selectHueLight selects a hue light from the set of connected lights
     * \param i the index of the light you want to select.
     */
    void selectHueLight(int i);

    /*!
     * \brief connectedHues returns a vector of structs that contain all relevant
     *        states of all Hue lights connected to the Bridge. These values are
     *        updated every few seconds by a timer.
     * \return a vector of SHueLight structs which contain info on all the connected lights.
     */
    std::vector<SHueLight> connectedHues() { return mHue->connectedHues(); }

signals:
    /*!
    * \brief hueDiscoveryStateChange the state of the Hue discovery methods,
    *        forwarded from a private HueBridgeDiscovery object.
    */
   void hueDiscoveryStateChange(int);

   /*!
    * \brief hueLightStateChange sent out whenever there is a change in the state
    *        of the LEDs.
    */
   void hueLightStateChange();

private slots:
   /*!
    * \brief hueStateUpdate forwards the hue discovery state changes
    *        from a private HueBridgeDiscovery object.
    */
   void hueDiscoveryUpdate(int newDiscoveryState) { emit hueDiscoveryStateChange(newDiscoveryState); }

   /*!
    * \brief hueLightStateUpdate forwards the hue light state changes
    *        from a prviate HueBridgeDiscovery object.
    */
   void hueLightStateUpdate() { emit hueLightStateChange(); }

private:

#ifndef MOBILE_BUILD
    /*!
     * \brief mSerial Serial connection object
     */
    std::shared_ptr<CommSerial> mSerial;
#endif //MOBILE_BUILD
    /*!
     * \brief mHTTP HTTP connection object
     */
    std::shared_ptr<CommHTTP> mHTTP;
    /*!
     * \brief mUDP UDP connection object
     */
    std::shared_ptr<CommUDP>  mUDP;

    /*!
     * \brief mHue Phillip's Hue connection object
     */
    std::shared_ptr<CommHue> mHue;

    /*!
     * \brief mCommType The currently active
     *        connection type.
     */
    ECommType mCommType;

    /*!
     * \brief mComm pointer to the current connection
     *        being used.
     */
    CommType *mComm;

    /*!
     * \brief mSettings object used to access persistent app memory
     */
    QSettings *mSettings;

    /*!
     * \brief mCurrentlyDiscovering set to true by startDiscovery() and set to false
     *        by stopDiscovery().
     */
    bool mCurrentlyDiscovering;

    /*!
     * \brief hueBrightness converts the brightness from the applications protocols
     *        to a brightness message that the Hue can use.
     * \param brightness a brightness value between 0 and 100.
     */
    void hueBrightness(int brightness);

    /*!
     * \brief hueRoutineChange converts a routine change from the applications protocols
     *        to a color change packet that the Hue can use.
     * \param routine the new routine for the Hue
     * \param colorGroupUsed the color group used for the routine.
     */
    void hueRoutineChange(ELightingRoutine routine, int colorGroupUsed = -1);

    /*!
     * \brief KCommDefaultType Settings key for default type of communication.
     *        This is saved whenever the user changes it and is restored at the
     *        start of each application session.
     */
    const static QString KCommDefaultType;

    /*!
     * \brief KCommDefaultName Settings key for default name of communication.
     *        This is saved whenever the user changes it and is restored at the
     *        start of each application session.
     */
    const static QString kCommDefaultName;

};

#endif // COMMLAYER_H
