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
 * stm32f4eval_leds.c
 *
 *  Created on: Sep 17, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "stm32f4eval.h"

static gpio_t leds_gpio[] =
{ GPIO_G, GPIO_G, GPIO_I, GPIO_C };
static gpio_pin_t leds_pin[] =
{ GPIO_PIN_6, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_7 };
static int leds_num = sizeof(leds_gpio) / sizeof(leds_gpio[0]);

void platform_leds_setup()
{
    // Configure the LEDs
    int i;
    for (i = 0; i < leds_num; i++)
    {
        gpio_set_output(leds_gpio[i], leds_pin[i]);
    }

    // Clear LEDs
    leds_off(0xFF);
}

void leds_off(uint8_t leds)
{
    int i;
    for (i = 0; i < leds_num; i++)
    {
        if (leds & (1 << i))
        {
            gpio_pin_clear(leds_gpio[i], leds_pin[i]);
        }
    }
}
void leds_on(uint8_t leds)
{
    int i;
    for (i = 0; i < leds_num; i++)
    {
        if (leds & (1 << i))
        {
            gpio_pin_set(leds_gpio[i], leds_pin[i]);
        }
    }
}

void leds_toggle(uint8_t leds)
{
    int i;
    for (i = 0; i < leds_num; i++)
    {
        if (leds & (1 << i))
        {
            gpio_pin_toggle(leds_gpio[i], leds_pin[i]);
        }
    }
}

