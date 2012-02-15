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
 * net_timer.c
 *
 *  Created on: Jul 20, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "platform.h"
#include "net_timer.h"

#include "printf.h"

// Private prototypes
/** Handler for the timer update interrupt (every 2 seconds) */
static void timer_update_2s(handler_arg_t arg, uint16_t timer_value);

// Private variables
/** The timer we're using */
static timer_t timer;

/** The time in 2 seconds unit */
static volatile uint32_t time_2s;

void net_timer_init(timer_t tim)
{
    // Store timer and channel
    timer = tim;

    // Set time to 0
    time_2s = 0;

    // Start timer, with our function as update handler
    timer_start(timer, 0xFFFF, timer_update_2s, NULL);
}

uint32_t net_timer_time()
{
    // Return the time in second plus the timer time
    return (time_2s << 16) + timer_time(timer);
}

uint32_t net_timer_convert_time(uint16_t time16)
{
    // Return the time in second plus the given 16bit time
    return (time_2s << 16) + time16;
}

/* --------------------------------------------------------------------------*/
/** Warning: interrupt service routine !! */
static void timer_update_2s(handler_arg_t arg, uint16_t timer_value)
{
    // Increment time_2s by 1
    time_2s++;
}
