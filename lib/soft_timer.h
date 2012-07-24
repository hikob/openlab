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
 * soft_timer.h
 *
 *  Created on: Jan 13, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef SOFT_TIMER_H_
#define SOFT_TIMER_H_

#include <stdint.h>
#include "handler.h"

#include "soft_timer_delay.h"

typedef struct soft_timer_alarm
{
    /** Internally used pointer DO TO MODIFY */
    struct soft_timer_alarm *next;

    /** Next scheduled alarm, DO NOT MODIFY */
    uint32_t alarm;

    /** Periodicity, DO NOT MODIFY */
    uint32_t period;

    handler_t handler;
    handler_arg_t handler_arg;
} soft_timer_t;

/**
 * Initialize and start the soft timer library, if not already started.
 */
void soft_timer_init();

/**
 * Set the handler and argument to this alarm.
 *
 * \param timer the timer to configure
 * \param handler the handler function to call when alarm expires;
 * \param handler_arg_t the handler argument to provide;
 */
static inline void soft_timer_set_handler(soft_timer_t *timer,
        handler_t handler, handler_arg_t arg)
{
    timer->handler = handler;
    timer->handler_arg = arg;
}

/**
 * Schedule an alarm;
 *
 * \param timer the timer to schedule;
 * \param ticks the delay before the alarm;
 * \param periodic indicates it the timer is to be repeated or not
 */
void soft_timer_start(soft_timer_t *timer, uint32_t ticks, uint32_t periodic);

/**
 * Schedule an alarm to trigger at a given time. It can't be periodic.
 *
 * \param timer the timer to schedule;
 * \param alarm_time the time at which the handler should be called.
 */
void soft_timer_start_at(soft_timer_t *timer, uint32_t alarm_time);
/**
 * Stop a scheduled alarm.
 *
 * \param timer the timer to stop.
 */
void soft_timer_stop(soft_timer_t *timer);

/**
 * Reset a timer by re-scheduling it from now with the period set initially.
 *
 * \param timer the timer to reset
 */
void soft_timer_reset(soft_timer_t *timer);

/**
 * Check if an alarm is scheduled.
 *
 * \param timer the timer to check
 * \return 1 if alarm is scheduled, 0 if not
 */
int32_t soft_timer_is_active(soft_timer_t *timer);

void soft_timer_debug();

#endif /* SOFT_TIMER_H_ */
