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
 * Copyright (C) 2011,2012 HiKoB.
 */

/*
 * batfb.h
 *
 *  Created on: Oct 26, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef BATFB_H_
#define BATFB_H_

#include <stdint.h>
#include "gpio.h"
#include "adc.h"

typedef enum
{
    BATFB_TYPE_NONE = 0,
    BATFB_TYPE_PRIMARY = 1,
    BATFB_TYPE_SECONDARY = 2,
} batfb_type_t;

typedef enum
{
    BATFB_SOLAR_OFF = 0,
    BATFB_SOLAR_LITTLE = 1,
    BATFB_SOLAR_POWERING = 2,
} batfb_solar_status_t;

/** Last measured value of Vcc, in volts */
extern float batfb_vcc;
/** Last measured value of Vbat, in volts */
extern float batfb_vbat;

/** Get the power source type */
batfb_type_t batfb_get_type();

/**
 * Sample the battery voltage.
 *
 * \param handler the handler function to be called after the conversion is done;
 * \param arg the argument to provide to the handler function call.
 */
void batfb_sample(handler_t handler, handler_arg_t arg);

/**
 * Sample the battery voltage, blocking.
 *
 * Once sampled, the voltage is available in batfb_vbat.
 *
 */
void batfb_sample_sync();

/**
 * Check if the battery feedback has a solar capability
 *
 * \return 1 if it has, 0 otherwise
 */
int32_t batfb_has_solar();

/**
 * Read the solar state
 */
batfb_solar_status_t batfb_get_solar_state();

#endif /* BATFB_H_ */
