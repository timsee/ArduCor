#ifndef LIGHTINGPAGE_H
#define LIGHTINGPAGE_H

#include "commlayer.h"
#include "datalayer.h"
#include <memory>

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief Inherited by Lighting Pages, provides a link to the backend.
 */
class LightingPage {

public:
    virtual ~LightingPage(){}

    void setup(std::shared_ptr<CommLayer> commLayer,std::shared_ptr<DataLayer> dataLayer) {
        mComm = commLayer;
        mData = dataLayer;
    }

protected:
    /*!
     * \brief communication pointer to communication object
     * for sending comannds to the lights
     */
    std::shared_ptr<CommLayer> mComm;

    /*!
     * \brief data layer that maintains and tracks the states of the lights
     *  and the saved data of the GUI
     */
    std::shared_ptr<DataLayer> mData;

};


#endif // LIGHTINGPAGE_H
