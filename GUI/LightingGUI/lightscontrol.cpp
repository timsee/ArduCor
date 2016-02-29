/*!
 * RGB-LED-Routines
 * LightingGUI
 *
 * A Qt-based GUI for controlling LED arrays.
 *
 * Github repository: http://www.github.com/timsee/RGB-LED-Routines
 * License: MIT-License, LICENSE provided in root of git repo
 */

#include "lightscontrol.h"

LightsControl::LightsControl() {
    comm = new CommLayer();
    data = new DataLayer();
    data->resetToDefaults();
}

LightsControl::~LightsControl() {
    delete comm;
    delete data;
}
