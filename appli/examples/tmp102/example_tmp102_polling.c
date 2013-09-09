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

/**
 * \example example_tmp102_polling.c
 * This file is an example of the use of the TMP102 temperature sensor.
 *
 * \date Oct 1, 2012
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "printf.h"

#include "tmp102.h"

static void app_task(void *);

int main()
{
    // Initialize the platform
    platform_init();

    // Create a task for the application
    xTaskCreate(app_task, (const signed char * const) "tmp102", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Run
    platform_run();
    return 0;
}

static void app_task(void *param)
{
    printf("# Testing TMP102\n");

    printf("# Starting continuous sampling, 8Hz\n");
    tmp102_sample_continuous(0, TMP102_CONV_RATE_8HZ);

    uint32_t i = 0;
    while (1)
    {
        float t;
        // Read the temperature and apply its resolution factor
        t = tmp102_read(0);
        t *= TMP102_DEG_PER_BIT;

        printf("[%u] T=%f\n", i, t);

        // Wait for 1/8th of second
        vTaskDelay(configTICK_RATE_HZ / 8);
        i++;
    }
}

