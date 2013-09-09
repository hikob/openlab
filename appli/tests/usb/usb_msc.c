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
 * usb_msc.c
 *
 *  Created on: May 18, 2012
 *      Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */
#include <stdbool.h>
#include "platform.h"
#include "FreeRTOS.h"
#include "task.h"
#include "libusb.h"
#define NO_DEBUG_HEADER
#define LOG_LEVEL LOG_LEVEL_DEBUG
#include "printf.h"
#include "debug.h"

static void vLEDTask(void *pvParameters)
{
    while (1)
    {
        leds_toggle(LED_0);
        vTaskDelay(configTICK_RATE_HZ);
    }
}

int main()
{
    signed portBASE_TYPE ret;
    static xTaskHandle vLEDTaskHandle;

    // Initialize the platform
    platform_init();

    log_info("USB MSC test");
    log_info("============");

    // Initialize USB
    usb_init(USB_MSC);

    leds_off(LED_0 | LED_1);

    // Create a blinking LED task that is initially suspended
    ret = xTaskCreate(vLEDTask, (signed char *)"LED", configMINIMAL_STACK_SIZE, NULL, 1, &vLEDTaskHandle);
    if (ret == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
    {
	log_error("Could not allocate required memory");
	HALT();
    }

    log_info("heap remaining space: %d bytes", xPortGetFreeHeapSize());

    // Start the scheduler
    platform_run();

    return 0;
}

