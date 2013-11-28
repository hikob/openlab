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
 * soft_timer_.h
 *
 *  Created on: Feb 12, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef SOFT_TIMER__H_
#define SOFT_TIMER__H_

#if !defined(PLATFORM_OS) || (PLATFORM_OS == FREERTOS)
#include "FreeRTOS.h"
#include "semphr.h"
#else
typedef void* xSemaphoreHandle;
#endif 

#include "soft_timer.h"
#include "timer.h"


typedef struct
{
    /** Mutex for protection */
    xSemaphoreHandle mutex;

    /** Event priority on which to run the soft timer process */
    event_queue_t priority;

    /** Linked list of scheduled soft_timers */
    soft_timer_t *first;

    /** Timer information */
    openlab_timer_t timer;
    timer_channel_t channel;

    /** Flag indicating if the timer alarm is set */
    int alarm_scheduled;

    /** Flag indicating the timer alarm has posted an event */
    int alarm_posted;

    /** Flag indicating the process method is posted */
    int process_posted;

    /** Number of timer updates before first alarm is scheduled */
    int32_t remainder;

    /** The number of timer updates */
    uint32_t update_count;
} softtim_t;

extern softtim_t softtim;

/**
 * Configure the software timer, specifying its timer and channel to use
 */
void soft_timer_config(openlab_timer_t timer, timer_channel_t channel);

/**
 * Specify the priority on which to run the soft_timer process
 */
static inline void soft_timer_config_priority(event_queue_t priority)
{
    softtim.priority = priority;
}

/**
 * Function to call by the platform at each timer update.
 */
void soft_timer_update(handler_arg_t arg, uint16_t count);

#endif /* SOFT_TIMER__H_ */
