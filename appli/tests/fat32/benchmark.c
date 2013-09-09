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
 * benchmark.c
 *
 *  Created on: Aug 17, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */
#include <stdbool.h>
#include "platform.h"
#include "printf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "fat32.h"

//#define WRITE_RATE 500 // Hz
//#define WRITE_RATE 333  // Hz
#define WRITE_RATE 200 // Hz
#define MAX_WRITE 6000
#define BUF_SIZE 10

xTaskHandle vLEDTaskHandle, vBenchTaskHandle;
file_t f;
uint8_t fb[512];
uint8_t *filename = (uint8_t *)"LOG.TXT";
int error = 0;

static void vLEDTask(void *pvParameters);
static void vBenchTask(void *pvParameters);

int main()
{
    signed portBASE_TYPE ret;

    // Initialize the platform
    platform_init();

    leds_off(LED_0 | LED_1);

    printf("\r\n\r\nFAT32 benchmark\r\n===============\r\n");

    // Create the writing task
    ret = xTaskCreate(vBenchTask, (signed char *)"Bench", configMINIMAL_STACK_SIZE, NULL, 3, &vBenchTaskHandle);

    switch (ret)
    {
        case errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY:
            printf("/!\\ Error: Could not allocate required memory\r\n");

            while (1)
            {
                ;
            }

            return 0;

        default:
            printf("-> Writing task created successfully\r\n");
    }

    // Create a blinking LED task that is initially suspended
    ret = xTaskCreate(vLEDTask, (signed char *)"LED", configMINIMAL_STACK_SIZE, NULL, 1, &vLEDTaskHandle);

    switch (ret)
    {
        case errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY:
            printf("/!\\ Error: Could not allocate required memory\r\n");

            while (1)
            {
                ;
            }

            return 0;

        default:
            printf("-> LED task created successfully\r\n");
    }

    // If an error occured suspend this task and make the LED blink
    if (fat32_init() != FAT32_OK)
    {
        error = 1;
        printf("/!\\ Error initializing FAT32\r\n");
        vTaskSuspend(vBenchTaskHandle);
    }
    else
    {
        printf("-> FAT32 initialized successfully\r\n");
        vTaskSuspend(vLEDTaskHandle);
    }

    // Start the scheduler
    platform_run();

    return 0;
}

// Task that blinks LED at 1Hz
static void vLEDTask(void *pvParameters)
{
    while (1)
    {
        vTaskDelay(configTICK_RATE_HZ >> (error - 1));

        leds_toggle(LED_0 | LED_1);
    }
}

// Task that writes to SD at WRITE_RATE Hz
static void vBenchTask(void *pvParameters)
{
    portTickType xLastWakeTime, t;
    static uint32_t buf[2 + BUF_SIZE];
    uint8_t volume[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    if ((error == 0) && (fat32_mount() != FAT32_OK))
    {
        error = 2;
    }

    if ((error == 0) && fat32_file_exists(filename))
    {
        fat32_delete(filename);
    }

    if ((error == 0) && (file_create(filename, fb, &f) != FAT32_OK))
    {
        error = 3;
    }

    if (error)
    {
        switch (error)
        {
            case 1:
                printf("/!\\ Unable to initialize FAT32\r\n");
                break;
            case 2:
                printf("/!\\ Unable to mount FAT32 file system\r\n");
                break;
            case 3:
                printf("/!\\ Unable create file\r\n");
                break;
        }

        vTaskResume(vLEDTaskHandle);
        vTaskSuspend(NULL);
    }

    fat32_get_volume_name(volume);
    printf("Volume name: %s\r\n", volume);

    buf[0] = 0;
    xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        // If the next wake up time is already passed, then
        // update the last wake up time with the current time
        t = xTaskGetTickCount();

        if (t > xLastWakeTime + configTICK_RATE_HZ / WRITE_RATE)
        {
            xLastWakeTime = t - configTICK_RATE_HZ / WRITE_RATE;
        }

        // Wait for the next cycle.
        vTaskDelayUntil(&xLastWakeTime, configTICK_RATE_HZ / WRITE_RATE);

        if (buf[0] < MAX_WRITE)
        {
            // Perform write.
            buf[0]++;
            buf[1] = xTaskGetTickCount();
            file_write(&f, (uint8_t *)buf, sizeof(buf));

            if (buf[0] % 16 == 0)
            {
                printf(".");
            }

            if (buf[0] % 256 == 0)
            {
                printf("\r\n");
            }
        }
        else
        {
            file_close(&f);

            printf("\r\nEND\r\n");

            fat32_get_volume_name(volume);
            printf("Volume name: %s\n", volume);

            leds_on(LED_0);

            // Suspend this task
            vTaskSuspend(NULL);
        }
    }
}
