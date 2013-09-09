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

/**
 * \example example_soft_timer_alarm.c
 *
 * This examples shows how alarm timers may be used in an application
 * to wait for a given time.
 *
 * \date Oct 2, 2012
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "event.h"
#include "printf.h"

#include "soft_timer.h"

/**
 * Handler for soft timer alarms
 *
 * \param arg the index of the alarm
 */
static void event(handler_arg_t arg);

#define NUM_ALARMS 6
#define BASE_PERIOD soft_timer_s_to_ticks(5)
static soft_timer_t alarms[NUM_ALARMS];

static uint32_t t_ref;

int main()
{
    // Initialize the platform
    platform_init();

    // Initialize the event and software timer
    event_init();
    soft_timer_init();

    // Set all handlers
    int i;
    for (i = 0; i < NUM_ALARMS; i++)
    {
        soft_timer_set_handler(alarms + i, event, (handler_arg_t) i);
    }

    // Store time reference
    t_ref = soft_timer_time();

    // Schedule first alarm, periodic
    soft_timer_start(alarms, BASE_PERIOD, 1);

    // Run
    platform_run();
    return 0;
}

static void event(handler_arg_t arg)
{
    int alarm = (int) arg;
    uint32_t t = soft_timer_time();
    uint32_t dt = t - t_ref;
    printf("[%u ms] Alarm %u\n", soft_timer_ticks_to_ms(dt), alarm);

    if (alarm == 0)
    {
        // Start all alarms, starting from 1
        int i;
        for (i = 1; i < NUM_ALARMS; i++)
        {
            soft_timer_start_at(alarms + i, t + BASE_PERIOD / (1 + i));
        }

        // Update t_ref
        t_ref = t;
    }
}
