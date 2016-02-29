/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#ifndef LIGHTSCONTROL_H
#define LIGHTSCONTROL_H

#include "commlayer.h"
#include "datalayer.h"

/*!
 * \brief The LightsControl class the beginning of a backend
 * class. This class will eventually contain all the systems
 * used to control the lights. For now, it contains two objects:
 * an object that controls communication to the lights, and an
 * object that stores data about the lights.
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
