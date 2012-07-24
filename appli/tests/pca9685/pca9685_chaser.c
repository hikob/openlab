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

#define RATIO 1

typedef struct
{
    float r, g, b;
} color_t;

static const color_t BLACK =
{ 0, 0, 0 };
static const color_t RED =
{ 1, 0, 0 };
static const color_t GREEN =
{ 0, 1, 0 };
static const color_t BLUE =
{ 0, 0, 1 };
static const color_t YELLOW =
{ 1, 1, 0 };
static const color_t CYAN =
{ 0, 1, 1 };
static const color_t PINK =
{ 1, 0, 1 };
static const color_t WHITE =
{ 1, 1, 1 };

static void set_triled(uint32_t led, color_t color);
static void set_all_trileds(color_t color);

static void turn(color_t back_color, color_t front_colot);
int main()
{
    // Initialize the platform
    platform_init();

    printf("### Testing PCA9685 ###\n");

    // Start the ADC
    while (1)
    {
        // Display all colors in turn
        turn(BLACK, WHITE);
        turn(RED, CYAN);
        turn(CYAN, GREEN);
        turn(GREEN, PINK);
        turn(PINK, YELLOW);
        turn(YELLOW, BLUE);
        turn(BLUE, WHITE);
    }

    return 0;
}

static void set_triled(uint32_t led, color_t color)
{
    pca9685_t pca = pca9685_get(led / 4);

    // Set the LEDs
    uint32_t led_r = 3 * (led % 4);
    pca9685_set_led(pca, led_r, 4096 * color.r * RATIO);
    pca9685_set_led(pca, led_r + 1, 4096 * color.g * RATIO);
    pca9685_set_led(pca, led_r + 2, 4096 * color.b * RATIO);
}
static void set_all_trileds(color_t color)
{
    uint32_t led;

    for (led = 0; led < 16; led++)
    {
        set_triled(led, color);
    }
}

static void turn(color_t back_color, color_t front_color)
{
    uint32_t led;

    for (led = 0; led < 16; led++)
    {
        // Set all back
        set_all_trileds(back_color);
        // Set front
        set_triled(led, front_color);

        soft_timer_delay_ms(30);
    }
}

