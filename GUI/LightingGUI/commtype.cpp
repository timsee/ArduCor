/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 */

#include "commtype.h"

#define DEFAULT_IP "192.168.0.125"

void CommType::setupConnectionList(ECommType type) {
    mType = type;
    mListMaxSize = 5;
    mSelectedIndex = 0;
    mListSize = mSettings.value(settingsListSizeKey()).toInt();
    // handles an edge case thats more likely to come up in debugging than typical use
    // but it would cause a crash either way...
    if (mListSize >= mListMaxSize) {
        qDebug() << "WARNING! You have too many saved settings. Reverting all.";
        mListSize = 0;
    }
    // create the mList object
    mList =  std::shared_ptr<std::vector<QString> > (new std::vector<QString>(mListMaxSize, nullptr));
    // add in a default for specific cases
    if (mListSize == 0) {
        if(mType == ECommType::eHTTP
             || mType == ECommType::eUDP ) {
            addConnection(DEFAULT_IP);
        }
    } else {
        // load preexisting settings, if they exist
        for (int i = 0; i < mListSize; ++i) {
           QString value = mSettings.value(settingsIndexKey(i)).toString();
           if (value.compare(QString("")) != 0) (*mList.get())[i] = value;
        }
    }
}

bool CommType::addConnection(QString connection) {
    // check both that the connection is valid and that it doesn't already exist in the list
    if (checkIfConnectionIsValid(connection) && !checkIfConnectionExistsInList(connection)) {
        // when less than the max size of connections have been saved, update these values
        if (mListSize < mListMaxSize) {
            mListSize++;
        }
        // copy the current vector
        std::vector<QString> tempCopy = *mList.get();
        // add the connection to the front of the list
        (*mList.get())[0] = connection;
        // move all values one right and throw out the last value of the list
        for (int i = 1; i < mListSize; ++i) {
           (*mList.get())[i] = tempCopy[i - 1];
        }
        mSelectedIndex = 0;
        return true;
    }
    return false;
}

bool CommType::selectConnection(QString connection) {
    // check if connection exists
    if (checkIfConnectionExistsInList(connection)) {
        // find the connection's index and save that as mSelectedIndex
        for (int i = 0; i < mListSize; ++i) {
            if (connection.compare((*mList.get())[i]) == 0) {
                mSelectedIndex = i;
                return true;
            }
        }
    }
    return false;
}

bool CommType::selectConnection(int connectionIndex) {
    if (connectionIndex < mListSize) {
        mSelectedIndex = connectionIndex;
        return true;
    }
    return false;
}


bool CommType::removeConnection(QString connection) {
    // check if connection exists in list and that its not the only connection
    if (checkIfConnectionExistsInList(connection) && (mListSize > 1)) {
        // get index of connection we're removing
        int tempIndex;
        for (int i = 0; i < mListSize; ++i) {
            if (connection.compare((*mList.get())[i]) == 0) tempIndex = i;
        }
        // handle edge case
        if (tempIndex == (mListSize - 1)) {
           (*mList.get())[tempIndex] = QString("");
        } else {
            // copy the vector
            std::vector<QString> tempCopy = *mList.get();
            // move all values to the right of the index one left
            for (int i = tempIndex; i < mListSize; ++i) {
               (*mList.get())[i] = tempCopy[i + 1];
            }
        }
        // reduce the overall size.
        if ((mListSize != 0)) mListSize--;
        return true;
    }
    return false;
}

void CommType::saveConnectionList() {
     // take currently selected and reorder it to the top of the list
     if (mSelectedIndex != 0) {
         // copy the vector
         std::vector<QString> tempCopy = *mList.get();
         // set the selected index at the front of the vector
         (*mList.get())[0] = (*mList.get())[mSelectedIndex];
         int j = 0;
         // move all values less than than mSelectedIndex one index right
         // then keep all other values in the same place.
         for (int i = 1; i < mListSize; ++i) {
            (*mList.get())[i] = tempCopy[j];
            j++;
            if (j == mSelectedIndex) j++;
         }
     }
    // save the current size
    mSettings.setValue(settingsListSizeKey(), mListSize);
    // save the values in the list
    for (int i = 0; i < mListSize; ++i) {
        mSettings.setValue(settingsIndexKey(i), (*mList.get())[i]);
    }
    // write settings to disk
    mSettings.sync();
}


QString CommType::settingsIndexKey(int index) {
    QString typeID;
    if (mType == ECommType::eHTTP) {
        typeID = QString("HTTP");
    } else if (mType == ECommType::eUDP) {
        typeID = QString("UDP");
    } else if (mType == ECommType::eHue) {
        typeID = QString("HUE");
    }
#ifndef MOBILE_BUILD
    else if (mType == ECommType::eSerial) {
        typeID = QString("SERIAL");
    }
#endif //MOBILE_BUILD
    return (QString("CommList_%1_%2_Key").arg(typeID,
                                              QString::number(index)));
}

QString CommType::settingsListSizeKey() {
    QString typeID;
    if (mType == ECommType::eHTTP) {
        typeID = QString("HTTP");
    } else if (mType == ECommType::eUDP) {
        typeID = QString("UDP");
    } else if (mType == ECommType::eHue) {
        typeID = QString("HUE");
    }
#ifndef MOBILE_BUILD
    else if (mType == ECommType::eSerial) {
        typeID = QString("SERIAL");
    }
#endif //MOBILE_BUILD
    return (QString("CommList_%1_Size_Key").arg(typeID));
}


bool CommType::checkIfConnectionExistsInList(QString connection) {
    for (int i = 0; i < mListSize; ++i) {
        if (connection.compare((*mList.get())[i]) == 0) return true;
    }
    return false;
}


bool CommType::checkIfConnectionIsValid(QString connection) {
    //TODO: write a stronger check...
    if (mType == ECommType::eHTTP || mType == ECommType::eUDP) {
        if (connection.count(QLatin1Char('.') != 3)) return false;
    }
    return true;
}
