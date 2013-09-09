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
 * adg759_.h
 *
 *  Created on: Dec 21, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef ADG759__H_
#define ADG759__H_

#include "adg759.h"
#include "gpio.h"

typedef struct
{
    gpio_t enable_gpio;
    gpio_pin_t enable_pin;
    gpio_t select_gpio_A0;
    gpio_pin_t select_pin_A0;
    gpio_t select_gpio_A1;
    gpio_pin_t select_pin_A1;
} _adg759_t;

/**
 * Configure the ADG759 Multiplexer.
 *
 * This configures the mutliplexer pins.
 * \param _adg a pointer to the ADG759 structure to initialize
 * \param type the output type to set to the pins, either open drain or push pull
 */
void adg759_config(const _adg759_t *_adg, gpio_type_t type);

#endif /* ADG759__H_ */
