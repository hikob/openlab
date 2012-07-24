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
 * tmp102.c
 *
 *  Created on: Oct 24, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "printf.h"

#include "triled.h"
#include "soft_timer.h"

#define DELAY 4
#define MAX 255

int main()
{
    int i;

    // Initialize the platform
    platform_init();

    // Enable the TRI LED
    triled_enable();

    // DARK to RED
    for (i = 0; i < MAX; i++)
    {
        triled_set(i, 0, 0);
        soft_timer_delay_ms(DELAY);
    }

    while (1)
    {
        // RED to RED-GREEN
        for (i = 0; i < MAX; i++)
        {
            triled_set(MAX, i, 0);
            soft_timer_delay_ms(DELAY);
        }

        // RED-GREEN to GREEN
        for (i = MAX; i >= 0; i--)
        {
            triled_set(i, MAX, 0);
            soft_timer_delay_ms(DELAY);
        }

        // GREEN to GREEN-BLUE
        for (i = 0; i < MAX; i++)
        {
            triled_set(0, MAX, i);
            soft_timer_delay_ms(DELAY);
        }

        // GREEN-BLUE to BLUE
        for (i = MAX; i >= 0; i--)
        {
            triled_set(0, i, MAX);
            soft_timer_delay_ms(DELAY);
        }

        // BLUE to BLUE-RED
        for (i = 0; i < MAX; i++)
        {
            triled_set(i, 0, MAX);
            soft_timer_delay_ms(DELAY);
        }

        // BLUE-RED to RED
        for (i = MAX; i >= 0; i--)
        {
            triled_set(MAX, 0, i);
            soft_timer_delay_ms(DELAY);
        }
    }

    return 0;
}
