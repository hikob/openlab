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
 * Copyright (C) 2013 HiKoB.
 */

/*
 * iotlab-serial-test.c
 *
 *  Created on: Oct 4, 2013
 *      Author: burindes
 */

#include <string.h>

#include "platform.h"
#include "debug.h"

#include "soft_timer.h"
#include "iotlab-serial.h"

static soft_timer_t tim;
static iotlab_serial_handler_t serial_handler;

static void test(handler_arg_t arg)
{
    packet_t *pkt = packet_alloc(IOTLAB_SERIAL_PACKET_OFFSET);
    if (pkt != NULL )
    {
        log_info("Sending notification");

        memcpy(pkt->data, "the indicati", 12);
        if (0 == iotlab_serial_send_frame(0x13, pkt))
        {
            packet_free(pkt);
        }
    }
}

static int32_t cmd_received(uint8_t cmd_type, packet_t *pkt)
{
    pkt->data[pkt->length] = 0;
    printf("CMD %x received, payload: %s", cmd_type, pkt->data);
    memcpy(pkt->data, "the response", 12);
    pkt->length = 12;
    return 1;
}

int main()
{
    // Initialize the platform
    platform_init();

    // Start the soft timer
    soft_timer_init();

    printf("Starting Serial (uart:%x, external:%x)\n", uart_print, uart_external);
    // Start the serial lib
    iotlab_serial_start(500000);

    serial_handler.cmd_type = 0x12;
    serial_handler.handler = cmd_received;

    printf("Registering Handler\n");
    iotlab_serial_register_handler(&serial_handler);

    printf("Starting timer\n");
    soft_timer_set_handler(&tim, test, NULL );
    soft_timer_start(&tim, soft_timer_s_to_ticks(5), 1);

    printf("GO\n");
    // Run
    platform_run();
    return 0;
}

