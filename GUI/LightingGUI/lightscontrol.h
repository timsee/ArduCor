
#ifndef LIGHTSCONTROL_H
#define LIGHTSCONTROL_H

#include "commlayer.h"
#include "datalayer.h"

/*!
 * \copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
 * \brief The LightsControl class is a simple object
 * that contains the two backend classes required
 * for communciation to the LED array.
 */
class LightsControl
{

public:
    LightsControl();
    ~LightsControl();

    /*!
     * \brief communication pointer to communication object
     * for sending comannds to the lights
     */
    CommLayer *comm;

    /*!
     * \brief data layer that maintains and tracks the states of the lights
     *  and the saved data of the GUI
     */
    DataLayer *data;
};

#endif // LIGHTSCONTROL_H
