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
 * imu_mouse.c
 *
 *  Created on: Aug 30, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdbool.h>
#include "platform.h"
#include "FreeRTOS.h"
#include "task.h"
#include "libusb.h"

#include "lsm303dlhc.h"
#include "l3g4200d.h"

#define NO_DEBUG_HEADER
#define LOG_LEVEL LOG_LEVEL_DEBUG
#include "printf.h"
#include "debug.h"

static xTaskHandle vMouseTaskHandle;

static void vMouseTask(void *pvParameters);

int main()
{
    signed portBASE_TYPE ret;

    // Initialize the platform
    platform_init();

    log_info("USB IMU Mouse\r\n================\r\n");

    // Initialize USB
    usb_init(USB_HID);

    leds_on(LED_1);

    log_debug("Initializing LSM303DLHC");
    lsm303dlhc_powerdown();

    log_debug("Setting LSM303DLHC accelerometer datarate and scale");
    lsm303dlhc_acc_config(LSM303DLHC_ACC_RATE_400HZ, LSM303DLHC_ACC_SCALE_2G, LSM303DLHC_ACC_UPDATE_ON_READ);

    ret = xTaskCreate(vMouseTask, (signed char *)"Mouse", configMINIMAL_STACK_SIZE, NULL, 1, &vMouseTaskHandle);

    switch (ret)
    {
        case errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY:
            log_error("Could not allocate required memory");

            while (1)
            {
                ;
            }

            return 0;

        default:
            log_debug("Mouse task created successfully");
    }

    // Start the scheduler
    platform_run();

    return 0;
}

#define MIN 100

static int8_t clamp(int16_t x)
{
    if (x > MIN)
    {
        return 1;
    }
    else if (x < -MIN)
    {
        return -1;
    }

    return 0;
}

static void vMouseTask(void *pvParameters)
{
    int16_t a[3];
    int8_t x, y;

    while (1)
    {
        vTaskDelay(configTICK_RATE_HZ / 100);

        lsm303dlhc_read_acc(a);

        x = clamp(a[0]);
        y = clamp(a[1]);

        log_printf("a0: %d a1: %d\n", a[0], a[1]);
        log_printf("x: %d y: %d\n", x, y);

        hid_send(x, y, false, false, false);
    }
}
