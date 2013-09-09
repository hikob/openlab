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
 * _adg.c
 *
 *  Created on: Dec 21, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "adg759_.h"

static void inline pin_A0_set(const _adg759_t *_adg);
static void inline pin_A0_clear(const _adg759_t *_adg);
static void inline pin_A1_set(const _adg759_t *_adg);
static void inline pin_A1_clear(const _adg759_t *_adg);

void adg759_config(const _adg759_t *_adg, gpio_type_t type)
{
    // Configure the pins
    if (_adg->enable_gpio)
    {
        gpio_set_output(_adg->enable_gpio, _adg->enable_pin);
        gpio_config_output_type(_adg->enable_gpio, _adg->enable_pin, type);
    }

    gpio_set_output(_adg->select_gpio_A0, _adg->select_pin_A0);
    gpio_config_output_type(_adg->select_gpio_A0, _adg->select_pin_A0, type);
    gpio_set_output(_adg->select_gpio_A1, _adg->select_pin_A1);
    gpio_config_output_type(_adg->select_gpio_A1, _adg->select_pin_A1, type);

    // Start with disabled, channel 1
    adg759_disable(_adg);
    adg759_select(_adg, ADG759_INPUT_1);
}

void adg759_enable(adg759_t adg)
{
    const _adg759_t *_adg = (const _adg759_t*) adg;

    // Set the enable PIN
    if (_adg->enable_gpio)
    {
        gpio_pin_set(_adg->enable_gpio, _adg->enable_pin);
    }
}
void adg759_disable(adg759_t adg)
{
    const _adg759_t *_adg = (const _adg759_t*) adg;

    // Clear the enable PIN
    if (_adg->enable_gpio)
    {
        gpio_pin_clear(_adg->enable_gpio, _adg->enable_pin);
    }

    // Clear the Ax pins to prevent power loss
    pin_A0_clear(_adg);
    pin_A1_clear(_adg);
}
void adg759_select(adg759_t adg, adg759_input_t input)
{
    const _adg759_t *_adg = (const _adg759_t*) adg;

    switch (input)
    {
        case ADG759_INPUT_1:
            pin_A0_clear(_adg);
            pin_A1_clear(_adg);
            break;
        case ADG759_INPUT_2:
            pin_A0_set(_adg);
            pin_A1_clear(_adg);
            break;
        case ADG759_INPUT_3:
            pin_A0_clear(_adg);
            pin_A1_set(_adg);
            break;
        case ADG759_INPUT_4:
            pin_A0_set(_adg);
            pin_A1_set(_adg);
            break;
        default:
            // Invalid
            return;
    }
}

static void inline pin_A0_set(const _adg759_t *_adg)
{
    gpio_pin_set(_adg->select_gpio_A0, _adg->select_pin_A0);
}
static void inline pin_A0_clear(const _adg759_t *_adg)
{
    gpio_pin_clear(_adg->select_gpio_A0, _adg->select_pin_A0);
}
static void inline pin_A1_set(const _adg759_t *_adg)
{
    gpio_pin_set(_adg->select_gpio_A1, _adg->select_pin_A1);
}
static void inline pin_A1_clear(const _adg759_t *_adg)
{
    gpio_pin_clear(_adg->select_gpio_A1, _adg->select_pin_A1);
}
