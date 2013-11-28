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
 * timer_.h
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef OPENLAB_TIMER__H_
#define OPENLAB_TIMER__H_

#include <stdint.h>
#include <stddef.h>

#include "timer.h"
#include "rcc.h"
#include "nvic.h"

typedef struct
{
    // The estimated frequency
    uint32_t frequency;

    timer_handler_t update_handler;
    handler_arg_t update_handler_arg;
} _timer_data_t;

typedef struct
{
    // Base Address of the timer
    uint32_t base_address;
    // APB bus number
    rcc_apb_bus_t apb_bus;
    // APB bit
    rcc_apb_bit_t apb_bit;
    // IRQ line in the NVIC
    nvic_irq_line_t irq_line;

    uint8_t number_of_channels;

    timer_handler_t *channel_handlers;
    handler_arg_t *channel_handler_args;
    _timer_data_t *data;
} _openlab_timer_t;

#define TIMER_INIT(name, addr, bus, bit, line, num) \
    static _timer_data_t name##_data; \
    static timer_handler_t name##_handlers[num]; \
    static handler_arg_t name##_args[num]; \
    const _openlab_timer_t name = { \
    .base_address = addr, \
    .apb_bus = bus, \
    .apb_bit = bit, \
    .irq_line = line, \
    .number_of_channels = num, \
    .channel_handlers = name##_handlers, \
    .channel_handler_args = name##_args, \
    .data = &name##_data \
}

#define TIMER_INIT_BASIC(name, addr, bus, bit, line) \
    static _timer_data_t name##_data; \
    const _openlab_timer_t name = { \
    .base_address = addr, \
    .apb_bus = bus, \
    .apb_bit = bit, \
    .irq_line = line, \
    .number_of_channels = num, \
    .channel_handlers = NULL, \
    .channel_handler_args = NULL, \
    .data = &name##_data \
}
void timer_handle_interrupt(const _openlab_timer_t *timer);

#include "timer_registers.h"
static inline void timer_restart(const _openlab_timer_t *_timer)
{
    // Set the UG bit to generate an update event (reset)
    *timer_get_EGR(_timer) = TIMER_EGR__UG;
};

#endif /* TIMER__H_ */
