/*!
 * @copyright
 * Copyright (C) 2015 - 2016. All Rights MIT Licensed.
 *
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
