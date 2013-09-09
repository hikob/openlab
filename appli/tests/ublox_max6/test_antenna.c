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
 * test_antenna.c
 *
 *  Created on: Jun 28, 2013
 *      Author: Damien Hedde <damien.hedde.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"

#include "ublox_max6/max6x.h"
#include "ublox_max6/packet.h"
#include "ublox_max6/max6x_.h"

#include "printf.h"
#include "debug.h"

static max6x_antenna_t max6x_ant;

static void button_handler(handler_arg_t arg)
{
    switch (max6x_ant)
    {
        case MAX6X_ANTENNA_PATCH:
            log_printf("Switching to CONNECTOR\n");
            max6x_ant = MAX6X_ANTENNA_CONNECTOR;
            break;
        case MAX6X_ANTENNA_CONNECTOR:
            log_printf("Switching to PATCH\n");
            max6x_ant = MAX6X_ANTENNA_PATCH;
            break;
    }
    max6x_select_antenna(max6x_ant);
}

static void uart_char_recv (handler_arg_t arg, uint8_t c)
{
    if (c == 'a' || c == 'A')
    {
        switch (max6x_ant)
        {
            case MAX6X_ANTENNA_PATCH:
                log_printf("Switching to CONNECTOR\n");
                max6x_ant = MAX6X_ANTENNA_CONNECTOR;
                break;
            case MAX6X_ANTENNA_CONNECTOR:
                log_printf("Switching to PATCH\n");
                max6x_ant = MAX6X_ANTENNA_PATCH;
                break;
        }
        max6x_select_antenna(max6x_ant);
    }
}

int main()
{
    // Initialize the platform
    platform_init();
    
    soft_timer_init();

    max6x_ant = MAX6X_ANTENNA_PATCH;
    max6x_select_antenna(max6x_ant);
    button_set_handler(button_handler, NULL);
    uart_set_rx_handler(uart_print, uart_char_recv, NULL);

    platform_run();

    return 0;
}

