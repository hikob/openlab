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

/**
 * \example example_isl29020.c
 *
 * This file is an example of the use of ISL29020 light sensor.
 *
 *  \date Dec 3, 2012
 *  \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "platform.h"
#include "printf.h"

#include "isl29020.h"

/** Task handler */
static void app_task(void *);

int main()
{
    // Initialize the platform
    platform_init();

    // Create a task for the application
    xTaskCreate(app_task, (const signed char * const) "light",
            configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Run
    platform_run();
    return 0;
}

static void app_task(void *param)
{
    printf("# Testing ISL29020\n");

    printf("# Preparing the ISL29020 light sensor\n");
    isl29020_prepare(ISL29020_LIGHT__AMBIENT, ISL29020_RESOLUTION__16bit,
            ISL29020_RANGE__16000lux);

    printf("# Starting the ISL29020 light sensor\n");
    isl29020_sample_continuous();

    while (1)
    {
        // Wait for a sample to be rady (~100ms)
        vTaskDelay(configTICK_RATE_HZ / 10);

        // Read it
        float v = isl29020_read_sample();

        printf("Light: %f lux\n", v);
    }
}

