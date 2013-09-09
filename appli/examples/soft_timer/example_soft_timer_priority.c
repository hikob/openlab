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
 * \example example_soft_timer_priority.c
 *
 * This examples shows how alarm timers may be used with different priorities.
 *
 * \date Jan 15, 2013
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "event.h"
#include "printf.h"

#include "soft_timer.h"
#include "softtimer/soft_timer_.h"

/**
 * Handler for soft timer alarms
 *
 * \param arg the index of the alarm
 */
static void event(handler_arg_t arg);

static soft_timer_t low_alarm, high_alarm;

int main()
{
    // Initialize the platform
    platform_init();

    // Set high priority for soft_timer
    soft_timer_config_priority(EVENT_QUEUE_NETWORK);

    // Initialize the event and software timer
    event_init();
    soft_timer_init();

    // Set all handlers
    soft_timer_set_handler(&low_alarm, event, (handler_arg_t) 0);
    soft_timer_set_handler(&high_alarm, event, (handler_arg_t) 1);
    soft_timer_set_event_priority(&high_alarm, EVENT_QUEUE_NETWORK);

    // Schedule first alarm, periodic
    soft_timer_start(&low_alarm, soft_timer_s_to_ticks(1), 1);
    soft_timer_start(&high_alarm, soft_timer_ms_to_ticks(100), 1);

    // Run
    platform_run();
    return 0;
}

static void event(handler_arg_t arg)
{
    int alarm = (int) arg;

    if (alarm == 0)
    {
        // LOW
        printf("<");

        int i;
        for (i=0; i<50; i++)
        {
            printf("a");
            soft_timer_delay_ms(10);
        }

        printf(">\n");
    } else {
        printf(" HIGHP ");
    }
}
