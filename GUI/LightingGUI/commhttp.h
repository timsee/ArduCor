#ifndef COMMHTTP_H
#define COMMHTTP_H

#include "commtype.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief provides a HTTP communication stream, which is
 * similar to an IP Camera. Packets are sent as HTTP
 * requests by appending them to the end of the HTTP
 * request's link.
 */

class CommHTTP : public CommType
{
public:
    /*!
     * \brief CommHTTP Constructor
     */
    CommHTTP();

    /*!
     * \brief setup sets up the connection to send to
     *        IP Address provided as param1
     * \param param1 the IP Address to make a connection
     *        to.
     */
    void setup(QString param1);

    /*!
     * \brief sendPacket sends a packet in a way similar to am
     *        IP Camera: The packet is added to the end of the
     *        web address, and sent as an HTTP request
     * \param packet the string to be sent over HTTP.
     */
    void sendPacket(QString packet);

    /*!
     * \brief closeConnection Doesn't do anything for HTTP
     *        "connections" as they don't actually have a
     *        consistent connection.
     */
    void closeConnection();

private:
    /*!
     * \brief mNetworkManager Qt's HTTP connection object
     */
    QNetworkAccessManager *mNetworkManager;

};

#endif // COMMHTTP_H
