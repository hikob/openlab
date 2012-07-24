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
 * buttons.c
 *
 *  Created on: Dec 28, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"

void button_handler(void *dummy)
{
    leds_toggle(LED_1);
}

int main()
{
    // Initialize the platform
    platform_init();

    // Set initial values
    leds_off(LED_0);
    leds_off(LED_1);

    // Register handler
    button_set_handler(button_handler, NULL);

    while (1)
    {
        int i;

        for (i = 0; i < 0x80000; i++)
        {
            __asm__("nop");
        }

        if (button_state())
        {
            leds_toggle(LED_0);
        }
    }

    return 0;
}
