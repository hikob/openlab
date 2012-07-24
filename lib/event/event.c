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
 * event_rtos.c
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "task.h"

#include "event.h"
#include "printf.h"

#include "soft_timer.h"

#ifndef EVENT_QUEUE_LENGTH
#define EVENT_QUEUE_LENGTH 12
#endif

#ifndef TRACE_EVENT
#define TRACE_EVENT 0
#endif // TRACE_EVENT
#define TRACE_LENGTH 16

// typedef
typedef struct
{
    handler_t event;
    handler_arg_t event_arg;
#if TRACE_EVENT
    uint32_t t_in, t_out;
#endif
} queue_entry_t;

#if TRACE_EVENT
typedef struct
{
    uint32_t queue;
    handler_t event;
    handler_arg_t arg;
    uint32_t t_in, t_out;
} trace_entry_t;
static trace_entry_t trace[TRACE_LENGTH];
static uint32_t trace_next = 0;
#endif

// prototypes
static void event_task(void *param);

// data
static xTaskHandle tasks[2];
static xQueueHandle queues[2];
static queue_entry_t current_entries[2];

void event_init(void)
{
    // Create the 1st Queue
    queues[0] = xQueueCreate(EVENT_QUEUE_LENGTH, sizeof(queue_entry_t));

    if (queues[0] == NULL)
    {
        log_error("Failed to create the event queue #0!");
        HALT();
    }

    // Create the 1st task, plate its number in the param variable
    xTaskCreate(event_task, (const signed char *) "evt0",
                configMINIMAL_STACK_SIZE, (void *) 0,
                configMAX_PRIORITIES - 2, tasks);

    if (tasks[0] == NULL)
    {
        log_error("Failed to create the event task #0!");
        HALT();
    }

    // Create the 2nd Queue
    queues[1] = xQueueCreate(EVENT_QUEUE_LENGTH, sizeof(queue_entry_t));

    if (queues[1] == NULL)
    {
        log_error("Failed to create the event queue #1!");
        HALT();
    }

    // Create the 2nd task, plate its number in the param variable
    xTaskCreate(event_task, (const signed char *) "evt1",
                configMINIMAL_STACK_SIZE, (void *) 1,
                configMAX_PRIORITIES - 1, tasks + 1);

    if (tasks[1] == NULL)
    {
        log_error("Failed to create the event task #1!");
        HALT();
    }
}

event_status_t event_post(event_queue_t queue, handler_t event,
                          handler_arg_t arg)
{
    queue_entry_t entry;

    // Fill the entry
    entry.event = event;
    entry.event_arg = arg;

    // Send to Queue
    if (xQueueSendToBack(queues[queue], &entry, 0) == pdTRUE)
    {
        return EVENT_OK;
    }
    else
    {
        log_error("Failed to post to queue #%u, current event: %x", queue,
                  entry.event);
        HALT();
        return EVENT_FULL;
    }
}

event_status_t event_post_from_isr(event_queue_t queue, handler_t event,
                                   handler_arg_t arg)
{
    queue_entry_t entry;
    portBASE_TYPE yield = pdFALSE;

    // Fill the entry
    entry.event = event;
    entry.event_arg = arg;

    // Send to Queue
    if (xQueueSendToBackFromISR(queues[queue], &entry, &yield) == pdTRUE)
    {
        if (yield)
        {
            // The event task should yield!
            vPortYieldFromISR();
        }
        else
        {
        }

        return EVENT_OK;
    }
    else
    {
        log_error("Failed to post to queue #%u from ISR, current event: %x",
                  queue, entry.event);
        HALT();
        return EVENT_FULL;
    }
}

static void event_task(void *param)
{
    uint32_t num = (uint32_t) param;
    xQueueHandle queue = queues[num];
    queue_entry_t *entry = current_entries + num;

    // Infinite loop
    while (1)
    {
        // Get next event
        entry->event = NULL;

        if (xQueueReceive(queue, entry, portMAX_DELAY) == pdTRUE)
        {
            // Call the event
#if TRACE_EVENT
            entry->t_in = soft_timer_time();
#endif // TRACE_EVENT
            entry->event(entry->event_arg);
#if TRACE_EVENT
            entry->t_out = soft_timer_time();

            asm volatile("cpsid i");
            trace[trace_next].queue = num;
            trace[trace_next].event = entry->event;
            trace[trace_next].arg = entry->event_arg;
            trace[trace_next].t_in = entry->t_in;
            trace[trace_next].t_out = entry->t_out;

            trace_next = (trace_next + 1) % TRACE_LENGTH;
            asm volatile("cpsie i");
#endif // TRACE_EVENT
        }
        else
        {
            log_error("Failed to receive from queue #%d", num);
            HALT();
        }
    }
}

void event_debug()
{
    uint32_t i;
    log_printf("Debugging Queues...\n");

    for (i = EVENT_QUEUE_APPLI; i <= EVENT_QUEUE_NETWORK; i++)
    {
        log_printf("\tQueue #%u Current event:  %x", i,
                   current_entries[i].event);

        if (current_entries[i].event)
        {
            log_printf("(arg: %x", current_entries[i].event_arg);
#if TRACE_EVENT
            log_printf(", t_in: %u", current_entries[i].t_in);
#endif // TRACE_EVENT
            log_printf(")");
        }

        log_printf(", %u waiting\n", uxQueueMessagesWaiting(queues[i]));
    }

#if TRACE_EVENT
    log_printf("Event Backtrace:\n");
    log_printf("ID\tQueue\tEvent\t\tArg\t\tT_in\tT_out\tDuration\n");

    for (i = 1; i <= TRACE_LENGTH; i++)
    {
        trace_entry_t *entry = trace + ((trace_next + TRACE_LENGTH - i)
                                        % TRACE_LENGTH);
        log_printf("-%u\t%u\t%8x\t%8x\t%5u\t%5u\t%u\n", i, entry->queue,
                   entry->event, entry->arg, entry->t_in, entry->t_out,
                   entry->t_out - entry->t_in);
    }

#endif // TRACE_EVENT
}
