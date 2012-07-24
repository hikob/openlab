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
 * simplemulti.c
 *
 *  Created on: Jul 5, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "printf.h"

#include "FreeRTOS.h"
#include "task.h"

void blink_task(void *arg);
void hello_task(void *arg);

int main()
{
    xTaskHandle blink_handle, hello_handle;
    signed portBASE_TYPE ret;

    // Initialize the platform
    platform_init();

    printf("FreeRTOS Test Program\n");

    // Set initial values
    leds_on(LED_0);
    leds_off(LED_1);

    // Create a task to blink
    ret = xTaskCreate(blink_task, (const signed char * const) "blink",
                      configMINIMAL_STACK_SIZE, NULL, 1, &blink_handle);

    switch (ret)
    {
        case errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY:
            printf("Error: Could not allocate required memory\n");

            while (1)
            {
                ;
            }

            return 0;

        default:
            printf("Blink task created successfully\n");
    }

    // Create a task to print hello world
    xTaskCreate(hello_task, (const signed char * const) "hello",
                configMINIMAL_STACK_SIZE, NULL, 1, &hello_handle);

    switch (ret)
    {
        case errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY:
            printf("Error: Could not allocate required memory\n");

            while (1)
            {
                ;
            }

            return 0;

        default:
            printf("Hello task created successfully\n");
    }

    // Start the scheduler
    platform_run();

    return 0;
}

void blink_task(void *arg)
{
    uint32_t count = 0;

    // arg is not used
    (void) arg;

    while (1)
    {
        count++;

        if (count % 2 == 0)
        {
            leds_toggle(LED_0);
        }

        if (count % 4 == 0)
        {
            leds_toggle(LED_1);
        }

        vTaskDelay(configTICK_RATE_HZ);
    }
}

void hello_task(void *arg)
{
    uint32_t count = 0;

    // arg is not used
    (void) arg;

    while (1)
    {
        count ++;
        printf("Hello World %u\n", count);
        vTaskDelay(configTICK_RATE_HZ);
    }
}
