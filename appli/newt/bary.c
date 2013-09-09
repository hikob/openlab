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
 * bary.c
 *
 *  Created on: Apr 16, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "math.h"
#include "platform.h"
#include "printf.h"

#include "soft_timer.h"

#include "adc.h"
#include "pga308.h"
#include "adg759.h"
#include "pca9685.h"

extern adg759_t pga308_mux;

static void app_task(void *);

static void adc_sample_done(handler_arg_t arg, uint16_t value);
static volatile int32_t adc_value;

static void gauges_read();
static void gauges_compute();
static void gauges_display();

static uint32_t gauges_empty_values[4] =
{ 0, 0, 0, 0 };
static uint32_t gauges_values[4];
static const float gauges_positions[4][2] =
{
    { 0.6, 0 },
    { 0, 0 },
    { 0.6, 0.6 },
    { 0, 0.6 }
};
static float gauges_barycentre[2];
static float gauges_weight;

#define D_BORDER 0.075
#define D_SPACING 0.150

static const float leds_position[16][2] =
{
    { D_BORDER + 3 * D_SPACING, D_BORDER + 1 * D_SPACING },
    { D_BORDER + 2 * D_SPACING, D_BORDER + 1 * D_SPACING },
    { D_BORDER + 3 * D_SPACING, D_BORDER + 0 * D_SPACING },
    { D_BORDER + 2 * D_SPACING, D_BORDER + 0 * D_SPACING },

    { D_BORDER + 1 * D_SPACING, D_BORDER + 0 * D_SPACING },
    { D_BORDER + 1 * D_SPACING, D_BORDER + 1 * D_SPACING },
    { D_BORDER + 0 * D_SPACING, D_BORDER + 0 * D_SPACING },
    { D_BORDER + 0 * D_SPACING, D_BORDER + 1 * D_SPACING },

    { D_BORDER + 2 * D_SPACING, D_BORDER + 3 * D_SPACING },
    { D_BORDER + 2 * D_SPACING, D_BORDER + 2 * D_SPACING },
    { D_BORDER + 3 * D_SPACING, D_BORDER + 3 * D_SPACING },
    { D_BORDER + 3 * D_SPACING, D_BORDER + 2 * D_SPACING },

    { D_BORDER + 0 * D_SPACING, D_BORDER + 2 * D_SPACING },
    { D_BORDER + 1 * D_SPACING, D_BORDER + 2 * D_SPACING },
    { D_BORDER + 0 * D_SPACING, D_BORDER + 3 * D_SPACING },
    { D_BORDER + 1 * D_SPACING, D_BORDER + 3 * D_SPACING }

};

int main()
{
    // Initialize the platform
    platform_init();

    // Create a task for the application
    xTaskCreate(app_task, (const signed char * const) "app",
                configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Run
    platform_run();
    return 0;
}

static void app_task(void *param)
{
    printf("Gauges Barycenter\n");

    // Enable the pga and all
    pga308_enable();
    adg759_enable(pga308_mux);

    // Configure the PGA for input gain
    pga308_configure_gain(PGA308_INPUT_GAIN__16, 0x4000, 0x4000,
                          PGA308_OUTPUT_GAIN__6);

    // Enable the ADC
    adc_enable(pga308_get_adc());

    // Set ADC handler
    adc_set_handler(pga308_get_adc(), adc_sample_done, NULL);

    // Prepare the ADC
    adc_prepare_single(pga308_get_adc(), pga308_get_adc_channel());

    // Read a few times for setup
    gauges_read();

    // Read empty values
    printf("Reading empty gauges values... ");

    uint32_t i, j;
#define AVG_LEN 16

    for (i = 0; i < AVG_LEN; i++)
    {
        gauges_read();

        for (j = 0; j < 4; j++)
        {
            gauges_empty_values[i % 4] += gauges_values[i % 4];
        }

        soft_timer_delay_ms(50);
    }

    for (i = 0; i < 4; i++)
    {
        gauges_empty_values[i] /= AVG_LEN;
    }

    printf("OK! (%u %u %u %u)\n", gauges_empty_values[0],
           gauges_empty_values[1], gauges_empty_values[2],
           gauges_empty_values[3]);

    while (1)
    {
        // Read the gauge values
        gauges_read();

        // Compute barycentre and total weight
        gauges_compute();

        // Update LEDs
        gauges_display();
    }
}

static void adc_sample_done(handler_arg_t arg, uint16_t value)
{
    // Store the value
    adc_value = value;
}

static void gauges_read()
{
    static float averaged_values[4] =
    { 0, 0, 0, 0 };

    uint32_t i;

    for (i = 0; i < 4; i++)
    {
        // Set the selected mux
        adg759_select(pga308_mux, ADG759_INPUT_1 + i);

        soft_timer_delay_ms(10);

        // Start new sample
        adc_value = -1;
        adc_sample_single(pga308_get_adc());

        // Wait until value updated
        while (adc_value == -1)
        {
            asm volatile("nop");
        }

        // Average if not null
        if (averaged_values[i] == 0)
        {
            averaged_values[i] = adc_value;
        }
        else
        {
#define COEF 0.7
            averaged_values[i] = (COEF * adc_value) + (1 - COEF)
                                 * averaged_values[i];
        }

        // Store result
        gauges_values[i] = (uint16_t) averaged_values[i];
    }
}

static void gauges_compute()
{
    gauges_barycentre[0] = 0;
    gauges_barycentre[1] = 0;
    gauges_weight = 0;

    uint32_t i;

    for (i = 0; i < 4; i++)
    {
        // Compute this gauge's weight
        float weight = (int32_t)(gauges_values[i] - gauges_empty_values[i]);

        // Update the barycenter contribution
        gauges_barycentre[0] += gauges_positions[i][0] * weight;
        gauges_barycentre[1] += gauges_positions[i][1] * weight;
        gauges_weight += weight;
    }

    if (gauges_weight < 50)
    {
        gauges_weight = 0;

        gauges_barycentre[0] = 0.3;
        gauges_barycentre[1] = 0.3;
    }
    else
    {
        gauges_barycentre[0] /= gauges_weight;
        gauges_barycentre[1] /= gauges_weight;
    }
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

static void gauges_display()
{
    // Loop over the LEDs
    uint32_t i;

    for (i = 0; i < 16; i++)
    {
        if (gauges_weight == 0)
        {
            set_triled(i, 0, 0, 0);
            continue;
        }

        // Compute distance to barycenter
        float d = hypot(gauges_barycentre[0] - leds_position[i][0],
                        gauges_barycentre[1] - leds_position[i][1]);

        // Compute scale
#define D_REF 0.30

        if (d > D_REF)
        {
            d = D_REF;
        }

        float c = 1 - (d / D_REF);

        // Update
        set_triled(i, c, 0, 0);
    }
}
