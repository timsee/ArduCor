#ifndef SERIALCOMM_H
#define SERIALCOMM_H

#ifndef MOBILE_BUILD

#include "commtype.h"

#include <QSerialPort>
#include <QSerialPortInfo>

#include <memory>

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 *
 * \brief A serial conenction using QSerialPort. This connection
 * will not work in mobile devices since QSerialPort is
 * unimplemented (for pretty obvious reasons :P). It is the
 * fastest and most stable connection on PCs.
 */

class CommSerial : public CommType
{
public:
    /*!
     * \brief CommSerial Constructor
     */
    CommSerial();
    /*!
     * \brief CommSerial Deconstructor
     */
    ~CommSerial();

    /*!
     * \brief serialList list of possible serial ports
     *        for connection
     */
    QList<QSerialPortInfo> serialList;

    /*!
     * \brief discoverSerialPorts looks for new serial ports and adds it them to
     *        the connections page, if they are found.
     */
    void discoverSerialPorts();

    /*!
     * \brief connectSerialPort connect to a specific serial port, if possible.
     * \param serialPortName The name of the serial port that you want
     *        to connect to.
     * \return true if connection is successful, false otherwise.
     */
    bool connectSerialPort(QString serialPortName);

    /*!
     * \brief sendPacket sends a string over serial
     * \param packet the string that is going to be sent over
     *        serial.
     */
    void sendPacket(QString packet);

    /*!
     * \brief closeConnection cleans up after the serial port. This
     *        is required to be called during cleanup, in some
     *        environments not closing a serial port leads to it
     *        staying bound until the computer resets.
     */
    void closeConnection();

private:

    /*!
     * \brief serial the serial port currently in use
     */
    std::shared_ptr<QSerialPort> serial;

    /*!
     * \brief mIsConnected true if connected, false otherwise
     */
    bool mIsConnected;

};

#endif // MOBILE_BUILD
#endif // SERIALCOMM_H
