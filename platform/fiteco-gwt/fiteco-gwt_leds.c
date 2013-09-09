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
 * fiteco-gwt_leds.c
 *
 *  Created on: Jul 10, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */


#include "platform.h"
#include "fiteco-gwt.h"

void platform_leds_setup()
{
    // Configure the LEDs
    gpio_set_output(GPIO_B, GPIO_PIN_5);
    gpio_set_output(GPIO_D, GPIO_PIN_2);

    // Set LEDs
    leds_on(LED_0 + LED_1);
}

void leds_off(uint8_t leds)
{
    if (leds & LED_0)
    {
        gpio_pin_set(GPIO_B, GPIO_PIN_5);
    }

    if (leds & LED_1)
    {
        gpio_pin_set(GPIO_D, GPIO_PIN_2);
    }
}
void leds_on(uint8_t leds)
{
    if (leds & LED_0)
    {
        gpio_pin_clear(GPIO_B, GPIO_PIN_5);
    }

    if (leds & LED_1)
    {
        gpio_pin_clear(GPIO_D, GPIO_PIN_2);
    }
}

void leds_toggle(uint8_t leds)
{
    if (leds & LED_0)
    {
        gpio_pin_toggle(GPIO_B, GPIO_PIN_5);
    }

    if (leds & LED_1)
    {
        gpio_pin_toggle(GPIO_D, GPIO_PIN_2);
    }
}

