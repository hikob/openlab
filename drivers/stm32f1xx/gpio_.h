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

#define GPIO_INIT(name, ba, ab) \
        const _gpio_t name = { .base_address = (ba), .apb2_bit = (ab) }

void gpio_set_alternate_function(const _gpio_t *_gpio, gpio_pin_t pin);

#endif /* GPIO__H_ */
