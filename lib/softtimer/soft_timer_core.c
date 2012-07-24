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
 * soft_timer.c
 *
 *  Created on: Jan 13, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "FreeRTOS.h"
#include "semphr.h"

#include "soft_timer_.h"
#include "timer.h"
#include "event.h"
#include "soft_timer.h"

#define LOG_LEVEL 3
#include "printf.h"

enum
{
    SOFT_TIMER_PERIOD_MASK = 0x7FFFFFFF,
    SOFT_TIMER_PERIODIC = 0x80000000,
};

/**
 * Insert a timer in the list and indicate if process is required.
 *
 * \param timer the timer to insert
 * \return 1 if a call to process is required, 0 otherwise
 **/
static int32_t insert(soft_timer_t *timer);

/**
 * Remove a atimer from the list and indicate if process is required.
 *
 * \param timer the timer to remove
 * \return 1 if a call to process is required, 0 otherwise
 **/
static int32_t remove(soft_timer_t *timer);
/** Call alarm handlers while expired */
static void process(handler_arg_t arg);
/** Handler for timer alarm */
static void timer_alarm(handler_arg_t arg, uint16_t count);

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
static void print_list_();
#define print_list() print_list_()
#else // LOG_LEVEL <= LOG_LEVEL_DEBUG
#define print_list()
#endif // LOG_LEVEL <= LOG_LEVEL_DEBUG

xSemaphoreHandle softtim_mutex = NULL;

void soft_timer_init()
{
    // Create mutex if required
    if (softtim_mutex == NULL)
    {
        softtim_mutex = xSemaphoreCreateMutex();
    }
}

void soft_timer_update(handler_arg_t arg, uint16_t count)
{
    // Increment the update counter
    softtim.update_count += 1;

    // Check if mutex is created, i.e. lib is started
    if (softtim_mutex == NULL)
    {
        return;
    }

    // Check remainder to process call
    if (softtim.remainder)
    {
        softtim.remainder--;
    }

    if (softtim.remainder == 0)
    {
        event_post_from_isr(EVENT_QUEUE_APPLI, process, NULL);
    }
}

void soft_timer_start(soft_timer_t *timer, uint32_t ticks, uint32_t periodic)
{
    uint32_t now = soft_timer_time();

    if (xSemaphoreTake(softtim_mutex, configTICK_RATE_HZ) != pdTRUE)
    {
        log_error("Failed to get soft timer mutex");
        HALT();
    }

    // Remove alarm if already scheduled
    remove(timer);

    // Fill data
    timer->next = NULL;
    timer->alarm = now + ticks;
    timer->period = ticks | (periodic ? SOFT_TIMER_PERIODIC : 0);

    // Insert in list
    if (insert(timer))
    {
        event_post(EVENT_QUEUE_APPLI, process, NULL);
    }

    // Release mutex
    xSemaphoreGive(softtim_mutex);
}

void soft_timer_start_at(soft_timer_t *timer, uint32_t alarm_time)
{
    if (xSemaphoreTake(softtim_mutex, configTICK_RATE_HZ) != pdTRUE)
    {
        log_error("Failed to get soft timer mutex");
        HALT();
    }

    // Remove alarm if already scheduled
    remove(timer);

    // Fill data
    timer->next = NULL;
    timer->alarm = alarm_time;
    timer->period = (alarm_time - soft_timer_time());

    // Insert in list
    if (insert(timer))
    {
        // Process
        event_post(EVENT_QUEUE_APPLI, process, NULL);
    }

    // Release mutex
    xSemaphoreGive(softtim_mutex);
}

void soft_timer_stop(soft_timer_t *timer)
{
    if (xSemaphoreTake(softtim_mutex, configTICK_RATE_HZ) != pdTRUE)
    {
        log_error("Failed to get soft timer mutex");
        HALT();
    }

    // Use remove algorithm and process if required
    if (remove(timer))
    {
        // Process
        event_post(EVENT_QUEUE_APPLI, process, NULL);
    }

    // Release mutex
    xSemaphoreGive(softtim_mutex);
}

void soft_timer_reset(soft_timer_t *timer)
{
    if (xSemaphoreTake(softtim_mutex, configTICK_RATE_HZ) != pdTRUE)
    {
        log_error("Failed to get soft timer mutex");
        HALT();
    }

    // Remove alarm if already scheduled
    remove(timer);

    // Fill data
    timer->next = NULL;
    timer->alarm = soft_timer_time() + (timer->period & SOFT_TIMER_PERIOD_MASK);

    // Insert in list
    if (insert(timer))
    {
        // Process
        event_post(EVENT_QUEUE_APPLI, process, NULL);
    }

    // Release mutex
    xSemaphoreGive(softtim_mutex);
}

int32_t soft_timer_is_active(soft_timer_t *timer)
{
    if (xSemaphoreTake(softtim_mutex, configTICK_RATE_HZ) != pdTRUE)
    {
        log_error("Failed to get soft timer mutex");
        HALT();
    }

    soft_timer_t *x;
    int32_t found = 0;

    // Loop to find the timer
    for (x = softtim.first; x != NULL; x = x->next)
    {
        if (x == timer)
        {
            // Found
            found = 1;
            break;
        }
    }

    // Release mutex
    xSemaphoreGive(softtim_mutex);

    return found;
}

void soft_timer_debug()
{
    log_printf("Debugging soft timer:\n");
    log_printf("\tFIRST->\n");
    soft_timer_t *x = softtim.first;

    while (x != NULL)
    {
        log_printf("\t%x(%u)->\n", x, x->alarm);
        x = x->next;
    }

    log_printf("\tNULL\n");
}

/* ************************************************************************ */

static int32_t insert(soft_timer_t *timer)
{
    /*
     * Check if first is empty.
     * Then, check if before first
     * Otherwise loop for the timer whose next is after
     */
    log_debug("*** Inserting %x(%u) ***", timer, timer->alarm);
    print_list();

    if (softtim.first == NULL)
    {
        log_debug("Inserting %x(%u) first", timer, timer->alarm);
        softtim.first = timer;
        timer->next = NULL;
        // Process required
        print_list();
        return 1;
    }

    if (!soft_timer_a_is_before_b(softtim.first->alarm, timer->alarm))
    {
        log_debug("Inserting %x(%u) before first", timer, timer->alarm);
        // Insert before first
        timer->next = softtim.first;
        softtim.first = timer;
        // Process required
        print_list();
        return 1;
    }

    soft_timer_t *x;

    log_debug("Inserting %x(%u) later", timer, timer->alarm);

    // Loop while timer is before x->next
    for (x = softtim.first; (x->next != NULL) && soft_timer_a_is_before_b(
                x->next->alarm, timer->alarm); x = x->next)
    {
    }

    // x is the timer before the new one
    timer->next = x->next;
    x->next = timer;

    print_list();
    // Process not required
    return 0;
}

static int32_t remove(soft_timer_t *timer)
{
    /*
     * Check if first
     * Check in the list
     */
    if (!softtim.first || !timer)
    {
        // Invalid
        return 0;
    }

    log_debug("*** Removing %x(%u) ***", timer, timer->alarm);
    print_list();

    if (softtim.first == timer)
    {
        log_debug("Removing first: %x(%u)", softtim.first, softtim.first->alarm);
        softtim.first = timer->next;
        // Process required
        print_list();
        return 1;
    }

    soft_timer_t *x;

    // Loop to find the previous one
    for (x = softtim.first; (x->next != NULL) && (x->next != timer); x
            = x->next)
    {
    }

    // Check if found
    if (x->next == timer)
    {
        log_debug("Removing other: %x(%u)", x, x->alarm);
        // Remove
        x->next = timer->next;
        print_list();
    }

    // Process not required
    return 0;
}

static void process(handler_arg_t arg)
{
    // If first is null, nothing to do
    if (softtim.first == NULL)
    {
        timer_set_channel_compare(softtim.timer, softtim.channel, 0, NULL, NULL);
        return;
    }

    if (xSemaphoreTake(softtim_mutex, configTICK_RATE_HZ) != pdTRUE)
    {
        log_error("Failed to get soft timer mutex");
        HALT();
    }

    // Loop while first event has triggered (first is before now)
    while (softtim.first && !soft_timer_a_is_before_b(soft_timer_time(),
            softtim.first->alarm))
    {
        // Store event that has triggered, and time
        soft_timer_t *x = softtim.first;

        log_debug("*** Processing %x(%u) ***", x, x->alarm);
        print_list();

        // Increment first
        softtim.first = softtim.first->next;

        // Re-insert event if it is periodic
        if (x->period & SOFT_TIMER_PERIODIC)
        {

            // Compute next timer
            x->alarm += x->period & SOFT_TIMER_PERIOD_MASK;

            log_debug("Rescheduling %x(%u/%x)", x, x->alarm, x->period);

            // Call insert
            insert(x);
        }

        print_list();

        // Call timer handler
        if (x->handler)
        {
            event_post(EVENT_QUEUE_APPLI, x->handler, x->handler_arg);
        }
    }

    // Check if there is a timer to schedule
    if (softtim.first)
    {
        vPortEnterCritical();

        // Check if next timer is in less than 2 seconds
        uint32_t delta = softtim.first->alarm - soft_timer_time();
        // Set remainder
        softtim.remainder = delta / 0x10000;

        vPortExitCritical();

        if (delta < 0x10000)
        {
            // Set timer for first event
            timer_set_channel_compare(softtim.timer, softtim.channel,
                                      (softtim.first->alarm & 0xFFFF), timer_alarm, NULL);

            // Check if we didn't miss a tick
            if (soft_timer_a_is_before_b(softtim.first->alarm,
                                         soft_timer_time()))
            {
                // Timer missed, request process
                event_post(EVENT_QUEUE_APPLI, process, NULL);
            }
        }
        else
        {
            timer_set_channel_compare(softtim.timer, softtim.channel, 0, NULL,
                                      NULL);
        }
    }
    else
    {
        timer_set_channel_compare(softtim.timer, softtim.channel, 0, NULL, NULL);
    }

    log_debug("After process");
    print_list();

    // Release mutex
    xSemaphoreGive(softtim_mutex);
}

static void timer_alarm(handler_arg_t arg, uint16_t count)
{
    // Post an event to process this timer
    event_post_from_isr(EVENT_QUEUE_APPLI, process, NULL);
}

/********** DEBUG ******************/

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
static void print_list_()
{
    log_printf("\t\tFIRST->");
    soft_timer_t *x = softtim.first;

    while (x != NULL)
    {
        log_printf("%x(%u)->", x, x->alarm);
        x = x->next;
    }

    log_printf("NULL\n");
}
#endif // LOG_LEVEL <= LOG_LEVEL_DEBUG
