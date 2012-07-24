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
 * blinkrtc.c
 *
 *  Created on: Jul 7, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include <stddef.h>
#include "platform.h"
#include "nvic.h"
#include "printf.h"

void alarm(handler_arg_t arg);

int main()
{
    // Initialize the platform
    platform_init();

    printf("SysTick test program\n");

    // Set initial values
    leds_on(LED_0);
    leds_off(LED_1);

    // Enable the SysTick, 100Hz
    nvic_enable_systick(100, alarm, 0);

    while (1)
    {
        int i;

        for (i = 0; i < 0x80000; i++)
        {
            __asm__("nop");
        }

        leds_toggle(LED_1);
    }

    return 0;
}

void alarm(handler_arg_t arg)
{
    static uint8_t cpt = 0;
    // arg is not used
    (void) arg;

    if (cpt++ == 100)
    {
        cpt = 0;
        leds_toggle(LED_0);
    }
}
