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
 * test_ina226.c
 *
 *  Created on: Aug 12, 2013
 *      Author: burindes
 */

#include "platform.h"
#include "debug.h"

#include "iotlab-control.h"
#include "iotlab-polling.h"
#include "iotlab-radio.h"
#include "iotlab-serial.h"

#include "softtimer/soft_timer_.h"

int main()
{
    // Initialize the platform
    platform_init();

    // Default to UART PRINT
    if (uart_external == NULL)
    {
        uart_external = uart_print;
    }

    // Start the soft timer with high priority
    soft_timer_init();
    soft_timer_config_priority(EVENT_QUEUE_NETWORK);

    // Start the serial lib
    iotlab_serial_start(500000);

    // Start the application libs
    iotlab_control_start();
    iotlab_polling_start();
    iotlab_radio_start();

    // Run
    platform_run();
    return 0;
}

