#ifndef COMMTYPE_H
#define COMMTYPE_H

#include <QString>
#include <QList>
/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief inherited by comm types, provides a general
 * interface that can be used to do the basic
 * connecting, disconnecting, and sending of packets.
 */
class CommType {
public:
    /*!
     * \brief ~CommType Deconstructor
     */
    virtual ~CommType(){}

    /*!
     * \brief setup set up the connection of a specific type.
     *        The parameter varies from connection to connection,
     *        but is explained in the specific connection's documentation.
     * \param param1 The parameter used to configure the connection.
     */
    void setup(QString param1);

    /*!
     * \brief closeConnection closes the connection, if applicable.
     *        connections like UDP won't need this, whereas connections
     *        like serial will absolutely require it.
     */
    void closeConnection();

    /*!
     * \brief sendPacket Sends the provided string over the
     *        connection stream.
     * \param packet the packet that is going to be sent
     */
    void sendPacket(QString packet);

    /*!
     * \brief name returns the "name" of the connection. This is
     *        the IP address or the serial port, depending on the
     *        connection
     * \return the name of the connection.
     */
    QString name() { return mName; }

    /*!
     * \brief name sets the name of the connection type. This varies
     *        from connection to connection.
     * \param name the new name of the connection.
     */
    void name(QString name) { mName = name; }

private:

    /*!
     * \brief mName the "name" of the connection, which is the unique
     *        identifier that would show up in a list of connections.
     *        For example, for a serial connection it is the serial port.
     */
    QString mName;

};

#endif // COMMTYPE_H
