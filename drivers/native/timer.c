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
 * timer.c
 *
 *  Created on: Sept, 2011
 *      Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#include "timer.h"
#include "timer_.h"
#include "printf.h"

void timer_enable(timer_t timer)
{
}

void timer_disable(timer_t timer)
{
}

void timer_select_internal_clock(timer_t timer, uint16_t prescaler)
{
}

void timer_select_external_clock(timer_t timer, uint16_t prescaler)
{
}

void timer_start(timer_t timer, uint16_t update_value,
                 timer_handler_t update_handler, handler_arg_t update_arg)
{
}

void timer_stop(timer_t timer)
{
}

uint16_t timer_time(timer_t timer)
{
    return 0;
}

uint32_t timer_get_frequency(timer_t timer)
{
    return 0;
}

uint16_t timer_get_number_of_channels(timer_t timer)
{
    return 0;
}

void timer_set_channel_compare(timer_t timer, timer_channel_t channel,
                               uint16_t compare_value, timer_handler_t handler, handler_arg_t arg)
{
}

void timer_update_channel_compare(timer_t timer, timer_channel_t channel,
                                  uint16_t value)
{
}

void timer_set_channel_capture(timer_t timer, timer_channel_t channel,
                               timer_capture_edge_t signal_edge, timer_handler_t handler,
                               handler_arg_t arg)
{
}

void timer_handle_interrupt(_timer_t *_timer)
{
}

