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
 * Copyright (C) 2011 HiKoB.
 */

/*
 * event.c
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

#ifndef EVENT_QUEUE_LENGTH
#define EVENT_QUEUE_LENGTH 4
#endif

// typedef
typedef struct
{
    handler_t event;
    handler_arg_t event_arg;
} queue_entry_t;

// prototypes
static void event_task(void *param);
static void event_debug();

// data
static xTaskHandle tasks[3];
static xQueueHandle queues[3];
static queue_entry_t current_entries[3];

void event_init(void)
{
    uint32_t i;

    for(i = EVENT_QUEUE_APPLI; i <= EVENT_QUEUE_NETWORK_HIGH; i++)
    {
        // Create the ith Queue
        queues[i] = xQueueCreate(EVENT_QUEUE_LENGTH, sizeof(queue_entry_t));

        if(queues[i] == NULL)
        {
            log_error("Failed to create the event queue #%d!", i);
            HALT();
        }

        signed char task_name[] = "evt0";
        task_name[3] = '0' + i;
        // Create the ith task, plate its number in the param variable
        xTaskCreate(event_task, task_name, 5 * configMINIMAL_STACK_SIZE, (void *) i,
                    configMAX_PRIORITIES - 3 + i, tasks + i);

        if(tasks[i] == NULL)
        {
            log_error("Failed to create the event task #%d!", i);
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
    if(xQueueSendToBack(queues[queue], &entry, 0) == pdTRUE)
    {
        return EVENT_OK;
    }
    else
    {
        log_error("Failed to post to queue #%u, current event: %x", queue,
                  entry.event);
        event_debug();
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
    if(xQueueSendToBackFromISR(queues[queue], &entry, &yield) == pdTRUE)
    {
        if(yield)
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
        event_debug();
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
    while(1)
    {
        // Get next event
        entry->event = NULL;

        if(xQueueReceive(queue, entry, portMAX_DELAY) == pdTRUE)
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

static void event_debug()
{
    uint32_t i;
#if RELEASE == 0
    log_printf("Debugging locked queues...");

    for(i = EVENT_QUEUE_APPLI; i <= EVENT_QUEUE_NETWORK_HIGH; i++)
    {
        log_printf("\tQueue #%u Current event: %x", i, current_entries[i].event);
    }

    signed char buf[256];
    vTaskList(buf);
    log_printf("\n%s\n", buf);
#endif
}
