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
#include "event_priorities.h"
#include "printf.h"
#include "debug.h"

#include "soft_timer.h"

#ifndef EVENT_QUEUE_LENGTH
#define EVENT_QUEUE_LENGTH 12
#endif

// typedef
typedef struct
{
    handler_t event;
    handler_arg_t event_arg;
} queue_entry_t;


// prototypes
static void event_task(void *param);

// data
static xTaskHandle tasks[2] = {NULL, NULL};
static xQueueHandle queues[2] = {NULL, NULL};
static queue_entry_t current_entries[2];

void event_init(void)
{
    if (queues[0] == NULL)
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
                    event_priorities[0], tasks);
        log_info("Priority of event task #0: %u/%u", event_priorities[0], configMAX_PRIORITIES - 1);

        if (tasks[0] == NULL)
        {
            log_error("Failed to create the event task #0!");
            HALT();
        }
    }
    if (queues[1] == NULL)
    {
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
                    event_priorities[1], tasks + 1);
        log_info("Priority of event task #1: %u/%u", event_priorities[1], configMAX_PRIORITIES - 1);

        if (tasks[1] == NULL)
        {
            log_error("Failed to create the event task #1!");
            HALT();
        }
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
            entry->event(entry->event_arg);
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
#if RELEASE == 0
    uint32_t i;
    log_printf("Debugging Queues...\n");

    for (i = EVENT_QUEUE_APPLI; i <= EVENT_QUEUE_NETWORK; i++)
    {
        log_printf("Queue #%u Current event:  %08x (%08x)", i,
                current_entries[i].event, current_entries[i].event_arg);
        int count = uxQueueMessagesWaiting(queues[i]);
        log_printf(", %u waiting:\n", count);
        queue_entry_t e;
        while ((xQueueReceive(queues[i], &e, 0) == pdTRUE))
        {
            log_printf("\tevt: %08x (%08x)\n", e.event, e.event_arg);
        }
    }
#endif
}
