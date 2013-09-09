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
 * azurelion_io.c
 *
 *  Created on: Nov 7, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "azurelion.h"

#ifndef PRINT_UART
#define PRINT_UART 2
#endif

static inline void pin_cfg(gpio_t gpio, gpio_pin_t pin)
{
    gpio_set_analog(gpio, pin);
}

void platform_io_setup()
{
    gpio_pin_t *pin;

    /* ****** GPIO A ****** */
    gpio_pin_t gpioA_pins[] =
    {
        GPIO_PIN_4, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14,
        GPIO_PIN_15
    };

    for (pin = gpioA_pins; pin < gpioA_pins + sizeof(gpioA_pins)
            / sizeof(gpio_pin_t); pin++)
    {
        pin_cfg(GPIO_A, *pin);
    }

    /* ****** GPIO B ****** */
    gpio_pin_t gpioB_pins[] =
    {
        GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_7, GPIO_PIN_8, GPIO_PIN_9,
        GPIO_PIN_12
    };

    for (pin = gpioB_pins; pin < gpioB_pins + sizeof(gpioB_pins)
            / sizeof(gpio_pin_t); pin++)
    {
        pin_cfg(GPIO_B, *pin);
    }

    /* ****** GPIO C ****** */
    gpio_pin_t gpioC_pins[] =
    { GPIO_PIN_7, GPIO_PIN_8, GPIO_PIN_12, GPIO_PIN_13 };

    for (pin = gpioC_pins; pin < gpioC_pins + sizeof(gpioC_pins)
            / sizeof(gpio_pin_t); pin++)
    {
        pin_cfg(GPIO_C, *pin);
    }

    /* ****** GPIO D ****** */

    /* ****** GPIO H ****** */
    gpio_pin_t gpioH_pins[] =
    { GPIO_PIN_0, GPIO_PIN_1 };
    gpio_enable(GPIO_H);

    for (pin = gpioH_pins; pin < gpioH_pins + sizeof(gpioH_pins)
            / sizeof(gpio_pin_t); pin++)
    {
        pin_cfg(GPIO_H, *pin);
    }

    // Disable GPIOs in sleep
    gpio_lp_disable(GPIO_A);
    gpio_lp_disable(GPIO_B);
    gpio_lp_disable(GPIO_C);

    // Disable Unused GPIOs
    gpio_disable(GPIO_D);
    gpio_disable(GPIO_H);

    // Disable unused SLP_TR pin for RF231
    if (al04_has_slptr_on_timer)
    {
        pin_cfg(GPIO_C, GPIO_PIN_10);
    }
    else
    {
        pin_cfg(GPIO_C, GPIO_PIN_6);
    }
}
