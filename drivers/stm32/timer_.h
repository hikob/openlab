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
 * timer_.h
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef TIMER__H_
#define TIMER__H_

#include <stdint.h>
#include <stddef.h>

#include "timer.h"
#include "rcc.h"
#include "nvic.h"

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

    // The estimated frequency
    uint32_t frequency;

    uint8_t number_of_channels;

    timer_handler_t update_handler;
    handler_arg_t update_handler_arg;

    timer_handler_t *channel_handlers;
    handler_arg_t *channel_handler_args;
} _timer_t;

static inline void timer_init(_timer_t *timer, uint32_t base_address,
                              rcc_apb_bus_t apb_bus, rcc_apb_bit_t apb_bit, nvic_irq_line_t irq_line,
                              uint8_t number_of_channels, timer_handler_t *channel_handlers,
                              handler_arg_t *channel_handler_args)
{
    timer->base_address = base_address;

    timer->apb_bus = apb_bus;
    timer->apb_bit = apb_bit;
    timer->irq_line = irq_line;

    timer->number_of_channels = number_of_channels;

    timer->channel_handlers = channel_handlers;
    timer->channel_handler_args = channel_handler_args;

    // Clear frequency
    timer->frequency = 0;
}

static inline void timer_init_basic(_timer_t *timer, uint32_t base_address,
                                    rcc_apb_bus_t apb_bus, rcc_apb_bit_t apb_bit, nvic_irq_line_t irq_line)
{
    // Initialize the timer with zero channel
    timer_init(timer, base_address, apb_bus, apb_bit, irq_line, 0, NULL, NULL);
}

static inline void timer_init_general(_timer_t *timer, uint32_t base_address,
                                      rcc_apb_bus_t apb_bus, rcc_apb_bit_t apb_bit, nvic_irq_line_t irq_line,
                                      timer_handler_t *channel_handlers, handler_arg_t *channel_handler_args)
{
    // Initialize the timer with four channels
    timer_init(timer, base_address, apb_bus, apb_bit, irq_line, 4,
               channel_handlers, channel_handler_args);
}

void timer_handle_interrupt(_timer_t *timer);

#endif /* TIMER__H_ */
