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
 * event.h
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "handler.h"

typedef enum
{
    EVENT_QUEUE_APPLI = 0,
    EVENT_QUEUE_NETWORK = 1,
    EVENT_QUEUE_NETWORK_HIGH = 2,
} event_queue_t;

typedef enum
{
    EVENT_OK,
    EVENT_FULL
} event_status_t;

/**
 * Initialize the event mechanism. Should be called first.
 */
void event_init(void);

/**
 * Post an event to an event queue.
 *
 * \param queue the queue to post the event to;
 * \param event the event handler to call;
 * \param param the parameter pointer.
 * \return EVENT_OK if post successful, EVENT_FULL if not possible.
 */
event_status_t event_post(event_queue_t queue, handler_t event,
                          handler_arg_t arg);
/**
 * Post an event to an event queue, from an ISR.
 *
 * There will be a task yield if required during the call.
 *
 * \param queue the queue to post the event to;
 * \param event the event handler to call.
 * \param param the parameter pointer.
 * \return EVENT_OK if post successful, EVENT_FULL if not possible.
 */
event_status_t event_post_from_isr(event_queue_t queue, handler_t event,
                                   handler_arg_t arg);

#endif /* EVENT_H_ */
