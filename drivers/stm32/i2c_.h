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
 * i2c_.h
 *
 *  Created on: Jul 27, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef I2C__H_
#define I2C__H_

#include "i2c.h"
#include "memmap.h"
#include "rcc.h"
#include "nvic.h"
#include "gpio.h"
#include "handler.h"

typedef struct
{
    uint32_t base_address;
    rcc_apb_bus_t apb_bus;
    rcc_apb_bit_t apb_bit;
    nvic_irq_line_t irq_line_ev, irq_line_er;

    handler_t transfer_handler;
    handler_arg_t transfer_handler_arg;
} _i2c_t;

static inline void i2c_init(_i2c_t *_i2c, uint32_t base_address,
                            rcc_apb_bus_t apb_bus, rcc_apb_bit_t apb_bit, nvic_irq_line_t irq_line_ev,
                            nvic_irq_line_t irq_line_er, gpio_t gpio, gpio_pin_t pin_sda, gpio_pin_t pin_scl)
{
    _i2c->base_address = base_address;
    _i2c->apb_bus = apb_bus;
    _i2c->apb_bit = apb_bit;
    _i2c->irq_line_ev = irq_line_ev;
    _i2c->irq_line_er = irq_line_er;

    // Configure SDA pin
    gpio_set_i2c_sda(gpio, pin_sda);
    // Configure SCL pin
    gpio_set_i2c_scl(gpio, pin_scl);
}

void i2c_handle_ev_interrupt(_i2c_t *_i2c);
void i2c_handle_er_interrupt(_i2c_t *_i2c);

#endif
