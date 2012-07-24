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
 * triled.c
 *
 *  Created on: Dec 22, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "triled_.h"

static struct
{
    timer_t tim;
    timer_channel_t r, g, b;
    uint8_t enabled;
} triled;

void triled_init(timer_t tim, timer_channel_t channel_red,
                 timer_channel_t channel_green, timer_channel_t channel_blue)
{
    // Store the parameters
    triled.tim = tim;
    triled.r = channel_red;
    triled.g = channel_green;
    triled.b = channel_blue;

    // Save state
    triled.enabled = 0;
}
void triled_enable()
{
    // Start the timer (32kHz)
    timer_select_external_clock(triled.tim, 0);

    // Update at 256 s steps
    timer_start(triled.tim, 255, NULL, NULL);

    // Configure PWM output
    timer_set_channel_compare(triled.tim, triled.r, 0, NULL, NULL);
    timer_set_channel_compare(triled.tim, triled.g, 0, NULL, NULL);
    timer_set_channel_compare(triled.tim, triled.b, 0, NULL, NULL);

    timer_activate_channel_output(triled.tim, triled.r, TIMER_OUTPUT_MODE_PWM2);
    timer_activate_channel_output(triled.tim, triled.g, TIMER_OUTPUT_MODE_PWM2);
    timer_activate_channel_output(triled.tim, triled.b, TIMER_OUTPUT_MODE_PWM2);

    triled.enabled = 1;
}
void triled_disable()
{
    // Disable the timer
    timer_stop(triled.tim);

    triled.enabled = 0;
}
void triled_set(uint16_t r, uint16_t g, uint16_t b)
{
    if (triled.enabled == 0)
    {
        return;
    }

    timer_update_channel_compare(triled.tim, triled.r, r);
    timer_update_channel_compare(triled.tim, triled.g, g);
    timer_update_channel_compare(triled.tim, triled.b, b);
}
