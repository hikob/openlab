/*
 * This file is part of HiKoB Openlab.
 *
 * HiKoB Openlab is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, version 3.
 *
 * HiKoB Openlab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with HiKoB Openlab. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2012 HiKoB.
 */

/*
 * \file fiteco_lib_gwt.h
 * \date Nov 30, 2012
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef FITECO_LIB_GWT__H_
#define FITECO_LIB_GWT__H_

#include "gpio.h"
#include "adg759.h"

typedef struct
{
    gpio_t main_power_gpio; gpio_pin_t main_power_pin;
    gpio_t battery_power_gpio; gpio_pin_t battery_power_pin;
    gpio_t battery_charge_gpio; gpio_pin_t battery_charge_pin;

    adg759_t current_mux;
} fiteco_lib_gwt_config_t;

/** Set the configuration structure of the GWT */
void fiteco_lib_gwt_set_config(const fiteco_lib_gwt_config_t* config);

#endif /* FITECO_LIB_GWT__H_ */
