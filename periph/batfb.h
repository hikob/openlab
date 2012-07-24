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

extern uint16_t batfb_vbat;

/**
 * Initialize the battery feedback , and put every thing in off mode.
 */
void batfb_init();

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

#endif /* BATFB_H_ */
