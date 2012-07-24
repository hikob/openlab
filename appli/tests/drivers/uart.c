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
 * uart.c
 *
 *  Created on: Jul 7, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "platform.h"
#include "printf.h"
#include "uart.h"

static void char_rx(handler_arg_t arg, uint8_t c);

int main()
{
    uint32_t count = 0;

    // Initialize the platform
    platform_init();

    // Set initial values
    leds_on(LED_0);
    leds_off(LED_1);

    // Set the input char callback
    uart_set_rx_handler(uart_print, char_rx, NULL);

    while (1)
    {
        uint32_t i;

        // Wait a little
        for (i = 0; i < 0x80000; i++)
        {
            __asm__("nop");
        }

        // Toggle the LEDs
        leds_toggle(LED_0 + LED_1);

        // Print the message
        printf("Hello, World\t#%x\n", count--);
    }

    return 0;
}

static void char_rx(handler_arg_t arg, uint8_t c)
{
    static const uint8_t diff = 'a' - 'A';

    if ((c >= 'a') && (c <= 'z'))
    {
        c = (c - diff);
        uart_transfer(uart_print, (uint8_t *) &c, 1);
    }
    else if ((c >= 'A') && (c <= 'Z'))
    {
        c = (c + diff);
        uart_transfer(uart_print, (uint8_t *) &c, 1);
    }
    else
    {
        // Simple echo
        uart_transfer(uart_print, (uint8_t *) &c, 1);
    }
}
