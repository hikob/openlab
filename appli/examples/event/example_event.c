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
 * \example example_event.c
 *
 * This file is an example of the event handler library.
 *
 * \date Oct 1, 2012
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "event.h"

#include "printf.h"
#include "debug.h"

//#define CHANGE_PRIORITY
#ifdef CHANGE_PRIORITY
#include "event/event_priorities.h"

const event_priorities_t event_priorities =
{
    /* priority of EVENT_APPLI */
    configMAX_PRIORITIES - 3,
    /* priority of EVENT_NETWORK */
    configMAX_PRIORITIES - 2,
};
#endif

static void uart_rx(handler_arg_t arg, uint8_t c);

static void start_event(handler_arg_t arg);
static void uart_event(handler_arg_t arg);
static void second_event(handler_arg_t arg);

static char message[] = "Hello, World!";

int main()
{
    // Initialize the platform
    platform_init();

    // Initialize the event library
    event_init();

    // Register a handler for UART rx characters.
    uart_set_rx_handler(uart_print, uart_rx, NULL);

    // Post a first event
    event_post(EVENT_QUEUE_APPLI, start_event, (handler_arg_t) 0x12345678);

    // Run
    platform_run();
    return 0;
}

static void start_event(handler_arg_t arg)
{
    printf("Start event, argument is %x\n", arg);
}

static void uart_rx(handler_arg_t arg, uint8_t c)
{
    // This is called from an Interrupt Service Routing
    event_post_from_isr(EVENT_QUEUE_APPLI, uart_event,
            (handler_arg_t) (uint32_t) c);
}
static void uart_event(handler_arg_t arg)
{
    printf("UART event, received character is %c\n", (char) (uint32_t) arg);

    // Post a second event
    event_post(EVENT_QUEUE_APPLI, second_event, (handler_arg_t) message);
}

static void second_event(handler_arg_t arg)
{
    printf("Second event, received string is %s\n", (const char*) arg);
}

