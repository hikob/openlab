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
 * usb_cdc_acm.c
 *
 *  Created on: Aug 30, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdbool.h>
#include "platform.h"
#include "FreeRTOS.h"
#include "task.h"
#include "libusb.h"
#define NO_DEBUG_HEADER
//#define LOG_LEVEL LOG_LEVEL_DEBUG
#include "printf.h"
#include "debug.h"

static xTaskHandle vLEDTaskHandle;

static void vLEDTask(void *pvParameters);

static void data_received(uint8_t *buf, uint16_t len)
{
    uint16_t i;

    for (i = 0; i < len; i++)
    {
        if ((buf[i] != '\n') && (buf[i] != '\r'))
        {
            if (cdc_acm_send(&(buf[i]), 1) != 1)
            {
                log_error("Character lost: %c", buf[i]);
            }
        }
        else
        {
            if (cdc_acm_send((uint8_t *)"\r\n", 2) != 2)
            {
                log_error("Character lost: \\n");
            }
        }
    }
}

int main()
{
    signed portBASE_TYPE ret;

    // Initialize the platform
    platform_init();

    log_info("USB CDC-ACM test\r\n================\r\n");

    // Initialize USB
    cdc_acm_register_rx_callback(data_received);
    usb_init(USB_CDC_ACM);

    // Create a blinking LED task that is initially suspended
    ret = xTaskCreate(vLEDTask, (signed char *)"LED", configMINIMAL_STACK_SIZE, NULL, 1, &vLEDTaskHandle);

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
            log_debug("LED task created successfully");
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
        vTaskDelay(configTICK_RATE_HZ);

        leds_toggle(LED_0);
    }
}
