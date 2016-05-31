#ifndef COMMUDP_H
#define COMMUDP_H

#include "commtype.h"
#include <QUdpSocket>
/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief Provides a UDP communication stream which allows
 * the sending of UDP datagrams to a specific IP Address
 * and port.
 */

class CommUDP : public CommType
{
public:
    /*!
     * \brief CommUDP Constructor
     */
    CommUDP();
    /*!
     * \brief CommUDP Deconstructor
     */
    ~CommUDP();

    /*!
     * \brief sendPacket sends the packet over UDP to a specified
     *        IP addres and port. Returns immediately and buffers
     *        unsent packets.
     * \param packet the string that is going to get sent over UDP.
     */
    void sendPacket(QString packet);

    /*!
     * \brief closeConnection unnecesary, implemented
     *        only to comply with being a CommType, since
     *        UDP is connectionless.
     */
    void closeConnection();

private:
    /*!
     * \brief mSocket Qt's UDP object
     */
    QUdpSocket *mSocket;

};

#endif // COMMUDP_H
