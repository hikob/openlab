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
 * usb_hid_kbd.c
 *
 *  Created on: 5 juillet 2013
 *      Author: Liam Beguin
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

#define NO_CHAR		-1

static void kbd_task(void *pvParameters);

int main()
{
    // Initialize the platform
    platform_init();

    log_info("USB Keyboard test\r\n================\r\n");

    // Initialize USB
    usb_init(USB_HID_KBD);

    xTaskCreate(kbd_task, (signed char *)"KBD", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    leds_on(LED_1);

    // Start the scheduler
    platform_run();

    return 0;
}

static void kbd_task(void *pvParameters)
{
    uint8_t idx = 0;
    bool last_state = false;
    char hikob[] = "-- HiKoB --";

    while (1)
    {
        /*  LED used to indicate if the device is set up  */
        if(kbd_get_state())
            leds_off(LED_1);

        last_state = false;
        leds_toggle(LED_0);
        vTaskDelay(configTICK_RATE_HZ/10);

        if(!button_state() && !last_state){	//if the button is pushed
            kbd_send(hikob[idx]);
            idx ++;
            last_state = true;
        }
        else{
            kbd_send(NO_CHAR);
            last_state = false;
        }

        if(idx > sizeof(hikob))
            idx = 0;
    }
}
