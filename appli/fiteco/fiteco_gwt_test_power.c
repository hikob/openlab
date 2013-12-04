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
 * fiteco_lib_gwt_test_power.c
 *
 *  Created on: Nov 30, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "debug.h"

#include "fiteco_lib_gwt.h"
#include "ina226.h"

static void char_rx(handler_arg_t arg, uint8_t c);
static void process_char(handler_arg_t arg);
static void sample_done(handler_arg_t arg, float u, float i, float p, float sv,
        uint32_t timestamp);

int main()
{
    // Initialize the platform
    platform_init();

    // Set handler for RX of char
    uart_set_rx_handler(uart_print, char_rx, NULL );

    log_printf(
            "\r\n\r\nOpenNode Power Selection:\r\n\t0: off\r\n\t1: main\r\n\t2: battery\r\n");
    log_printf("OpenNode Battery Charge:\r\n\t3: off\r\n\t4: on\r\n");
    log_printf(
            "Current measurement:\r\n\t5: off\r\n\t6: 3V\r\n\t7: 5V\r\n\t8: battery\r\n");

    // Launch everything
    platform_run();

    return 0;
}

static void char_rx(handler_arg_t arg, uint8_t c)
{
    event_post_from_isr(EVENT_QUEUE_APPLI, process_char,
            (handler_arg_t) (int) c);
}

static void process_char(handler_arg_t arg)
{
    int c = (int) arg;

    switch (c)
    {
        // Power Control
        case '0':
            // Power OFF
            log_debug("Power OFF\r");
            fiteco_lib_gwt_opennode_power_select(
                    FITECO_GWT_OPENNODE_POWER__OFF);
            break;
        case '1':
            // Power MAIN
            log_debug("Power MAIN\r");
            fiteco_lib_gwt_opennode_power_select(
                    FITECO_GWT_OPENNODE_POWER__MAIN);
            break;
        case '2':
            // Power Battery
            log_debug("Power Battery\r");
            fiteco_lib_gwt_opennode_power_select(
                    FITECO_GWT_OPENNODE_POWER__BATTERY);
            break;

            // Battery Charge
        case '3':
            // Charge OFF
            log_debug("Charge OFF\r");
            fiteco_lib_gwt_battery_charge_disable();
            break;
        case '4':
            // Charge ON
            log_debug("Charge ON\r");
            fiteco_lib_gwt_battery_charge_enable();
            break;

            // Current Measure
        case '5':
            // Current OFF
            log_debug("Current OFF\r");
            fiteco_lib_gwt_current_monitor_select(
                    FITECO_GWT_CURRENT_MONITOR__OFF, NULL, NULL );
            break;
        case '6':
            // Current 3V
            log_debug("Current 3V\r");
            fiteco_lib_gwt_current_monitor_configure(INA226_PERIOD_2116us, INA226_AVERAGE_64);
            fiteco_lib_gwt_current_monitor_select(
                    FITECO_GWT_CURRENT_MONITOR__OPEN_3V, sample_done, NULL );
            break;
        case '7':
            // Current 5V
            log_debug("Current 5V\r");
            fiteco_lib_gwt_current_monitor_configure(INA226_PERIOD_2116us, INA226_AVERAGE_64);
            fiteco_lib_gwt_current_monitor_select(
                    FITECO_GWT_CURRENT_MONITOR__OPEN_5V, sample_done, NULL );
            break;
        case '8':
            // Current Battery
            log_debug("Current Battery\r");
            fiteco_lib_gwt_current_monitor_configure(INA226_PERIOD_2116us, INA226_AVERAGE_64);
            fiteco_lib_gwt_current_monitor_select(
                    FITECO_GWT_CURRENT_MONITOR__BATTERY, sample_done, NULL );
            break;
        case 'r':
            nvic_reset();
            break;
    }
}

static void sample_done(handler_arg_t arg, float u, float i, float p, float sv,
        uint32_t timestamp)
{
    // Print values
    log_printf("[%u] \tu=%f, \t\ti=%f, \t\tp=%f\tsv: %f\r\n", timestamp, u, i,
            p, sv);
}
