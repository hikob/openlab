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
 * Copyright (C) 2011 HiKoB.
 */

/*
 * gpio_.h
 *
 *  Created on: Jul 12, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef GPIO__H_
#define GPIO__H_

#include "gpio.h"
#include "rcc.h"

typedef struct
{
    uint32_t base_address;
    rcc_apb_bit_t apb2_bit;
} _gpio_t;

static inline void gpio_init(_gpio_t *_gpio, uint32_t base_address, rcc_apb_bit_t apb2_bit)
{
    _gpio->base_address = base_address;
    _gpio->apb2_bit = apb2_bit;
}

void gpio_set_alternate_function(_gpio_t *_gpio, gpio_pin_t pin);

#endif /* GPIO__H_ */
