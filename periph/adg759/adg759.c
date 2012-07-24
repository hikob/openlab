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
 * adg759.c
 *
 *  Created on: Dec 21, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "adg759_.h"

static struct
{
    gpio_t enable_gpio;
    gpio_pin_t enable_pin;
    gpio_t select_gpio;
    gpio_pin_t select_pin_0;
    gpio_pin_t select_pin_1;
} adg759;

static void inline pin0_set();
static void inline pin0_clear();
static void inline pin1_set();
static void inline pin1_clear();

void adg759_config(gpio_t enable_gpio, gpio_pin_t enable_pin,
                   gpio_t select_gpio, gpio_pin_t select_pin_0, gpio_pin_t select_pin_1)
{
    // Store all
    adg759.enable_gpio = enable_gpio;
    adg759.enable_pin = enable_pin;
    adg759.select_gpio = select_gpio;
    adg759.select_pin_0 = select_pin_0;
    adg759.select_pin_1 = select_pin_1;

    // Configure the pins
    if (adg759.enable_gpio)
    {
        gpio_set_output(adg759.enable_gpio, adg759.enable_pin);
        gpio_config_output_type(adg759.enable_gpio, adg759.enable_pin,
                                GPIO_TYPE_OPEN_DRAIN);
    }

    gpio_set_output(adg759.select_gpio, adg759.select_pin_0);
    gpio_config_output_type(adg759.select_gpio, adg759.select_pin_0,
                            GPIO_TYPE_OPEN_DRAIN);
    gpio_set_output(adg759.select_gpio, adg759.select_pin_1);
    gpio_config_output_type(adg759.select_gpio, adg759.select_pin_1,
                            GPIO_TYPE_OPEN_DRAIN);

    // Start with disabled, channel 1
    adg759_disable();
    adg759_select(ADG759_INPUT_1);
}

void adg759_enable()
{
    // Set the enable PIN
    if (adg759.enable_gpio)
    {
        gpio_pin_set(adg759.enable_gpio, adg759.enable_pin);
    }
}
void adg759_disable()
{
    // Clear the enable PIN
    if (adg759.enable_gpio)
    {
        gpio_pin_clear(adg759.enable_gpio, adg759.enable_pin);
    }

    // Clear the Ax pins to prevent power loss
    pin0_clear();
    pin1_clear();
}
void adg759_select(adg759_input_t input)
{
    switch (input)
    {
        case ADG759_INPUT_1:
            pin0_clear();
            pin1_clear();
            break;
        case ADG759_INPUT_2:
            pin0_set();
            pin1_clear();
            break;
        case ADG759_INPUT_3:
            pin0_clear();
            pin1_set();
            break;
        case ADG759_INPUT_4:
            pin0_set();
            pin1_set();
            break;
        default:
            // Invalid
            return;
    }
}

static void inline pin0_set()
{
    gpio_pin_set(adg759.select_gpio, adg759.select_pin_0);
}
static void inline pin0_clear()
{
    gpio_pin_clear(adg759.select_gpio, adg759.select_pin_0);
}
static void inline pin1_set()
{
    gpio_pin_set(adg759.select_gpio, adg759.select_pin_1);
}
static void inline pin1_clear()
{
    gpio_pin_clear(adg759.select_gpio, adg759.select_pin_1);
}
