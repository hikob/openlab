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
 * timer.c
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include <stddef.h>
#include "platform.h"
#include "timer.h"

#include "printf.h"

#define PERIOD 32768

static void alarm(handler_arg_t arg, uint16_t counter);

int main()
{
    // Initialize the platform
    platform_init();

    // Set initial values
    leds_on(LED_0);
    leds_off(LED_1);

    // Configure Timer 3
    // Select 32kHz tick
    timer_enable(tim3);
    timer_select_internal_clock(tim3, (timer_get_frequency(tim3) / 32768) - 1);

    // Set alarm on channel 0 @1Hz
    timer_set_channel_compare(tim3, 0, PERIOD, alarm, NULL);

    // Start the timer
    // tim3 counts up to 65536 (2s period)
    timer_start(tim3, 0xFFFF, NULL, NULL);

    while(1)
    {
        __asm__("nop");
    }

    return 0;
}

static void alarm(handler_arg_t arg, uint16_t counter)
{
    // arg and counter are not used
    (void) arg;
    (void) counter;

    printf("LED0\n");
    leds_toggle(LED_0);
}
