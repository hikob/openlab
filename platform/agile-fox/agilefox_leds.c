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
 * agilefox_leds.c
 *
 *  Created on: Jul 13, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "platform.h"
#include "agilefox.h"

#include "gpio_.h"
#include "afio.h"
#include "exti.h"
#include "nvic_.h"


void platform_leds_setup()
{
    // Configure the LEDs
    // Enable port B
    gpio_enable(GPIO_B);

    // Set output pins B10 and B12
    gpio_set_output(GPIO_B, GPIO_PIN_10);
    gpio_set_output(GPIO_B, GPIO_PIN_12);

    // Clear LEDs
    leds_off(LED_0 + LED_1);
}

void leds_off(uint8_t leds)
{
    if (leds & LED_0)
    {
        gpio_pin_set(GPIO_B, GPIO_PIN_10);
    }

    if (leds & LED_1)
    {
        gpio_pin_set(GPIO_B, GPIO_PIN_12);
    }
}
void leds_on(uint8_t leds)
{
    if (leds & LED_0)
    {
        gpio_pin_clear(GPIO_B, GPIO_PIN_10);
    }

    if (leds & LED_1)
    {
        gpio_pin_clear(GPIO_B, GPIO_PIN_12);
    }
}

void leds_toggle(uint8_t leds)
{
    if (leds & LED_0)
    {
        gpio_pin_toggle(GPIO_B, GPIO_PIN_10);
    }

    if (leds & LED_1)
    {
        gpio_pin_toggle(GPIO_B, GPIO_PIN_12);
    }
}
