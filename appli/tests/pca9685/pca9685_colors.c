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
 * pca9685_chaser.c
 *
 *  Created on: Mar 30, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "printf.h"

#include "soft_timer.h"
#include "pca9685.h"

#define PAUSE 1
#define STEPS 30

static void set_triled(uint32_t led, float r, float g, float b);
static void set_all_trileds(float r, float g, float b);

int main()
{
    // Initialize the platform
    platform_init();

    printf("### Testing PCA9685 ###\n");

    float x;

    // Start the ADC
    while (1)
    {
        // Keep red and add green
        for (x = 0; x < 1; x += 1. / STEPS)
        {
            set_all_trileds(1, x, 0);
            soft_timer_delay_ms(PAUSE);
        }

        // Keep green and remove red
        for (x = 0; x < 1; x += 1. / STEPS)
        {
            set_all_trileds(1 - x, 1, 0);
            soft_timer_delay_ms(PAUSE);
        }

        // Keep green and add blue
        for (x = 0; x < 1; x += 1. / STEPS)
        {
            set_all_trileds(0, 1, x);
            soft_timer_delay_ms(PAUSE);
        }

        // Keep blue and remove green
        for (x = 0; x < 1; x += 1. / STEPS)
        {
            set_all_trileds(0, 1 - x, 1);
            soft_timer_delay_ms(PAUSE);
        }

        // Keep blue and add red
        for (x = 0; x < 1; x += 1. / STEPS)
        {
            set_all_trileds(x, 0, 1);
            soft_timer_delay_ms(PAUSE);
        }

        // Bonus: add green
        for (x = 0; x < 1; x += 1. / STEPS)
        {
            set_all_trileds(1, x, 1);
            soft_timer_delay_ms(PAUSE);
        }

        // Bonus: remove green
        for (x = 0; x < 1; x += 1. / STEPS)
        {
            set_all_trileds(1, 1 - x, 1);
            soft_timer_delay_ms(PAUSE);
        }

        // Keep red and remove blue
        for (x = 0; x < 1; x += 1. / STEPS)
        {
            set_all_trileds(1, 0, 1 - x);
            soft_timer_delay_ms(PAUSE);
        }
    }

    return 0;
}

static void set_triled(uint32_t led, float r, float g, float b)
{
    pca9685_t pca = pca9685_get(led / 4);

    // Set the LEDs
    uint32_t led_r = 3 * (led % 4);
    pca9685_set_led(pca, led_r, 4096 * r);
    pca9685_set_led(pca, led_r + 1, 4096 * g);
    pca9685_set_led(pca, led_r + 2, 4096 * b);
}
static void set_all_trileds(float r, float g, float b)
{
    uint32_t led;

    for (led = 0; led < 16; led++)
    {
        set_triled(led, r, g, b);
    }
}
