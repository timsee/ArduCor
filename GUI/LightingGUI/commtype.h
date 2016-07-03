#ifndef COMMTYPE_H
#define COMMTYPE_H

#include <QString>
#include <QList>
#include <QSettings>
#include <QDebug>
#include <QWidget>
#include <memory>


/*!
 * \brief The ECommType enum The connection types
 *        supported by the GUI. For mobile builds,
 *        serial is not supported.
 */
enum class ECommType {
#ifndef MOBILE_BUILD
    eSerial,
#endif //MOBILE_BUILD
    eHTTP,
    eUDP,
    eHue
};

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief inherited by comm types, provides a general interface that can
 * be used to do connections and sending packets. Each CommType also has its
 * own conenctionList(), which lists up to 5 of the previous connections. This
 * list persists in the application's memory after the application closes.
 */
class CommType : public QWidget {
    Q_OBJECT
public:
    /*!
     * \brief ~CommType Deconstructor
     */
    virtual ~CommType(){}

    /*!
     * \brief sendPacket Sends the provided string over the
     *        connection stream.
     * \param packet the packet that is going to be sent
     */
    void sendPacket(QString packet);

    /*!
     * \brief currentConnection returns the "name" of the connection. This is
     *        the IP address or the serial port, depending on the
     *        connection type
     * \return the name of the connection.
     */
    QString currentConnection() { return (*mList.get())[mSelectedIndex]; }

    /*!
     * \brief connectionList
     * \return
     */
    std::shared_ptr<std::vector<QString> > connectionList() { return mList; }

    /*!
     * \brief numberOfConnections count of connections stored in the
     *        connections list
     * \return count of connections stored in the connections list
     */
    int numberOfConnections() { return mListSize; }

    /*!
     * \brief selectedIndex the currently selected index of the connection list. Will always
     *        be smaller than numberOfConnections()
     * \return
     */
    int selectedIndex() { return mSelectedIndex; }

    // ----------------------------
    // Connection List Management
    // ----------------------------
    // Each CommType stores its own list of up to 5 possible connections
    // in its mList object. This is saved into persistent data and will
    // reload every time the program is started up.

    /*!
     * \brief setupConnectionList initializes the connection list and reloads
     *        it from system memory, if needed
     * \param type the ECommType of this specific connection.
     */
    void setupConnectionList(ECommType type);

    /*!
     * \brief addConnection attempts to add the connection to the connection list
     * \param connection the name of the new connection
     * \return true if the conenction is added, false otherwise
     */
    bool addConnection(QString connection);

    /*!
     * \brief selectConnection attempts to set the connction as the current conneciton in use.
     * \param connection the name of the connect that you want to use
     * \return true if the connection exists and setup was successful, false otherwise
     */
    bool selectConnection(QString connection);

    /*!
     * \brief selectConnection attempts to the use the connection from the connection list
     *        at the given index.
     * \param connectionIndex the index of the connection name that you want you want to connect to.
     * \return true if the index is valid, false otherwise.
     */
    bool selectConnection(int connectionIndex);

    /*!
     * \brief removeConnection attempts to remove the connection from the connection list
     * \param connection the connection you want to remove
     * \return true if the connection exists and was removed, false if it wasn't there in the first place
     */
    bool removeConnection(QString connection);

    /*!
     * \brief saveConnectionList must be called by deconstructors, saves the connection list to the app's
     *        persistent memory.
     */
    void saveConnectionList();

private:
    // ----------------------------
    // Connection List Helpers
    // ----------------------------

    /*!
     * \brief settingsIndexKey returns a settings key based on the index
     * \param index the index for the key
     * \return a QString of a key that represents the comm type and index
     */
    QString settingsIndexKey(int index);

    /*!
     * \brief settingsListSizeKey a key for saving and accessing the size of the array
     *        the array of saved values in the saved data that persists between sessions.
     * \return a Qstring of a key that contains the comm type.
     */
    QString settingsListSizeKey();

    /*!
     * \brief checkIfConnectionExistsInList checks if theres a string
     *        that exists in the saved data that is exactly the
     *        same as the input string
     * \param connection the string that is getting searched for in the
     *        saved data
     * \return true if the connection exists already, false otherwise.
     */
    bool checkIfConnectionExistsInList(QString connection);

    /*!
     * \brief checkIfConnectionIsValid based on the comm type, it checks if the
     *        new connection name is a valid connection name for that platform.
     * \param connection the name of the connection that you want to check for validity
     * \return  true if the connection has a valid name, false otherwise.
     */
    bool checkIfConnectionIsValid(QString connection);

    // ----------------------------
    // Connection List Variables
    // ----------------------------

    /*!
     * \brief mSettings Device independent persistent application memory access
     */
    QSettings mSettings;
    /*!
     * \brief mType the type CommType this is, meaning UDP, Serial, HTTP, etc.
     */
    ECommType mType;
    /*!
     * \brief mList the list of possible connections.
     */
    std::shared_ptr<std::vector<QString> > mList;
    /*!
     * \brief mSelectedIndex the index of the connection that is currently getting used.
     */
    int mSelectedIndex;
    /*!
     * \brief mListSize the current size of the list
     */
    int mListSize;
    /*!
     * \brief mListMaxSize the maximum possible size of the list
     */
    int mListMaxSize;

};

#endif // COMMTYPE_H
