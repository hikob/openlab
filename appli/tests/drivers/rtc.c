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
 * blinkrtc.c
 *
 *  Created on: Jul 7, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include <stddef.h>
#include "platform.h"
#include "stm32l1xx/rtc.h"
#include "printf.h"

void alarm(handler_arg_t arg);

int main()
{
    // Initialize the platform
    platform_init();

    printf("RTC test program\n");

    // Set initial values
    leds_on(LED_0);
    leds_off(LED_1);

    // Enable the RTC
    rtc_enable();
    rtc_set_periodic_wakeup(RTC_WAKEUP_CLOCK_16384Hz, 16384 - 1, alarm,
                            NULL);

    while (1)
    {
        int i;

        for (i = 0; i < 0x80000; i++)
        {
            __asm__("nop");
        }

        leds_toggle(LED_1);
        printf("Time: %u%u:%u%u:%u%u\n", rtc_get_hours_tens(),
               rtc_get_hours_units(), rtc_get_minutes_tens(),
               rtc_get_minutes_units(), rtc_get_seconds_tens(),
               rtc_get_seconds_units());
    }

    return 0;
}

void alarm(handler_arg_t arg)
{
    // arg is not used
    (void) arg;

    printf("\talarm\n");

    leds_toggle(LED_0);
}
