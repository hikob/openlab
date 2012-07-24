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
 * soft_timer_delay.c
 *
 *  Created on: Jan 13, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "soft_timer_.h"
#include "timer.h"

#define LOG_LEVEL LOG_LEVEL_INFO
#include "printf.h"

softtim_t softtim;

void soft_timer_config(timer_t timer, timer_channel_t channel)
{
    // Store parameters
    softtim.timer = timer;
    softtim.channel = channel;
    softtim.first = NULL;
    softtim.remainder = 0;
    softtim.update_count = 0;
}

uint32_t soft_timer_time()
{
    /**
     * To prevent having the remainder being incremented while computing,
     * do the following:
     *
     *   * disable the interrupts
     *   * get the time (A)
     *   * check if an update has happened
     *   * get the time again (B)
     *   * enable the interrupts
     *
     *   * an update was missed
     *      * if the update flag is set or
     *      * if B is lower than A
     *
     *
     */
    uint16_t t_a, t_b;
    uint32_t t, update = 0;

    // Mask interrupts
    asm volatile("cpsid i");

    t = softtim.update_count << 16;
    t_a = timer_time(softtim.timer);
    update = timer_get_update_flag(softtim.timer);
    t_b = timer_time(softtim.timer);

    // Unmask interrupts
    asm volatile("cpsie i");

    // Compute result as the base time plus the measured time, plus an update
    return t + ((update || (t_b < t_a)) ? 0x10000 : 0) + t_b;
}

uint32_t soft_timer_convert_time(uint16_t t)
{
    // We assume t cannot be in the future
    uint32_t ref = soft_timer_time();
    uint16_t n = ref & 0xFFFF;
    uint16_t d = n - t;

    if (n >= t)
    {
        if (d < 32768)
        {
            // Nothing to do
        }
        else
        {
            // t is in advance
            ref += 0x10000;
        }
    }
    else
    {
        if (d < 32768)
        {
            // Adjust
            ref -= 0x10000;
        }
        else
        {
            // t is in advance
        }
    }

    return (ref & 0xFFFF0000) | t;
}

uint32_t soft_timer_time_s()
{
    return (softtim.update_count * 2) + (soft_timer_time() & 0x1000 ? 1 : 0);
}

int32_t soft_timer_a_is_before_b(uint32_t a, uint32_t b)
{
    int32_t delta = b - a;
    return delta > 0;
}

void soft_timer_delay_us(uint32_t us)
{
    uint32_t t_end = soft_timer_time() + soft_timer_us_to_ticks(us);

    // Loop until delay elapsed
    while (soft_timer_a_is_before_b(soft_timer_time(), t_end))
    {
    }
}

void soft_timer_delay_ms(uint32_t ms)
{
    uint32_t t_end = soft_timer_time() + soft_timer_ms_to_ticks(ms);

    // Loop until delay elapsed
    while (soft_timer_a_is_before_b(soft_timer_time(), t_end))
    {
    }
}

void soft_timer_delay_s(uint32_t s)
{
    uint32_t t_end = soft_timer_time() + soft_timer_s_to_ticks(s);

    // Loop until delay elapsed
    while (soft_timer_a_is_before_b(soft_timer_time(), t_end))
    {
    }
}


