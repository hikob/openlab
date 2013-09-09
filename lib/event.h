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
 * \file event.h
 *
 * \date Jul 6, 2011
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef EVENT_H_
#define EVENT_H_

/**
 * \addtogroup lib
 * @{
 */

/**
 * \defgroup event Event library
 *
 * Event posting for executing functions.
 *
 * This library provides an event mechanism allowing to post events, i.e. functions,
 * to be executed by the library tasks.
 *
 * The library creates two tasks with on each one a queue (FIFO) of events.
 * These tasks repeatedly take an event from their queue, extract the handler function
 * to call and call it.
 *
 * The difference between the different tasks of the library is their priority.
 * There two tasks run with different priority, allowing for example a network
 * stack to run with hard real time constraints and an application with less
 * real time requirements with a lower priority.
 *
 * Event posting allows independent libraries or applications to share a reduced
 * number of tasks, as those cost a lot of RAM on the processor. Furthermore,
 * an event mechanism is ideal to implement a finite state machine or equivalent
 * development models.
 *
 * It is possible to post events either from a task context, or from an interrupt
 * service routine context, in which case the method prefixed with _from_isr
 * MUST be used.
 *
 * The inside queues are FIFO, hence after posting an event, its corresponding
 * handler function will be called after all the previously posted events are
 * done.
 *
 * These tasks and queues directly rely on FreeRTOS tasks and queues.
 *
 * If an application creates other FreeRTOS tasks, note that the two created
 * tasks use priorities defined as (configMAX_PRIORITY - 1) and
 * (configMAX_PRIORITY - 2)
 *
 * @{
 */

#include "handler.h"

/**
 * Type defining the available queues for posting events. The \ref
 * EVENT_QUEUE_APPLI has a low priority, and the \ref EVENT_QUEUE_NETWORK has
 * a high priority.
 */
typedef enum
{
    EVENT_QUEUE_APPLI = 0,
    EVENT_QUEUE_NETWORK = 1,
} event_queue_t;

typedef enum
{
    EVENT_OK,
    EVENT_FULL
} event_status_t;

/**
 * Initialize the event mechanism.
 *
 * This creates the two tasks and start them.
 *
 * \note This MUST be called before any post is called at least once. It is a
 * good practice for the libraries and application that rely on it to call this.
 */
void event_init();

/**
 * Post an event to an event queue.
 *
 * This posts an event to one of the event queues, to execute its handler on the
 * corresponding task.
 *
 * The event handler will be called after all previously posted events on the
 * same queue have completed, and the task becomes available.
 *
 * \warning This method MUST NOT be called from an interrupt service routing, use
 * \ref event_post_from_isr
 *
 * \param queue the queue to post the event to
 * \param event the event handler to be called
 * \param param the parameter pointer
 * \return EVENT_OK if the post is successful, EVENT_FULL if otherwise
 */
event_status_t event_post(event_queue_t queue, handler_t event,
                          handler_arg_t arg);

/**
 * Post an event to an event queue, from an interrupt service routine.
 *
 * This posts an event to one of the event queues, to execute its handler on the
 * corresponding task.
 *
 * The event handler will be called after all previously posted events on the
 * same queue have completed, and the task becomes available.
 *
 * \note This method is reserved for interrupt service routing context, if in
 * a task, prefer the \ref event_post method.
 * \note There will be a task yield if required during the call.
 *
 * \param queue the queue to post the event to
 * \param event the event handler to be called
 * \param param the parameter pointer
 * \return EVENT_OK if the post is successful, EVENT_FULL if otherwise
 */
event_status_t event_post_from_isr(event_queue_t queue, handler_t event,
                                   handler_arg_t arg);

/**
 * Debug the events.
 *
 * This prints information about the different tasks and queues, but should be
 * used only for debugging when a system error occurred.
 */
void event_debug();

/**
 * @}
 * @}
 */

#endif /* EVENT_H_ */
