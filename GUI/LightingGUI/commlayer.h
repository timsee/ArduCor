
#ifndef COMMLAYER_H
#define COMMLAYER_H

#include "lightingprotocols.h"
#include <QColor>
#include <memory>

#ifndef MOBILE_BUILD
#include "commserial.h"
#endif //MOBILE_BUILD
#include "commhttp.h"
#include "commudp.h"

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
class CommLayer
{
public:
    /*!
     * \brief Constructor
     */
    CommLayer();
    /*!
     * \brief Deconstructor
     */
    ~CommLayer();

#ifndef MOBILE_BUILD

    void changeSerialPort(QString serialPort);
#endif //MOBILE_BUILD

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
     * \brief mCommType The currently active
     *        connection type.
     */
    ECommType mCommType;

    /*!
     * \brief mComm pointer to the current connection
     *        being used.
     */
    CommType *mComm;
};

#endif // COMMLAYER_H
