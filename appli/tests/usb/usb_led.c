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
 * usb_led.c
 *
 *  Created on: Jun 28, 2012
 *      Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#include <stdbool.h>
#include <string.h>

#include "platform.h"
#include "FreeRTOS.h"
#include "task.h"
#include "libusb.h"
#define NO_DEBUG_HEADER
#define LOG_LEVEL LOG_LEVEL_DEBUG
#include "printf.h"
#include "debug.h"

/* ************************************************** */
/* ************************************************** */

void led_write(void* arg, uint16_t lba, uint8_t *b)
{
    log_debug("led_write() -- %s",b);
    leds_toggle(LED_1);
}

void led_read(void* arg, uint16_t lba, uint8_t *b)
{
    memset(b,0,512);
    sprintf((char*)b,"HiKoB led control over USB\n\r");
}

const mmap_entry_t mmapfs_files[] = 
{
    {
	.arg     = NULL,
	.name    = "LED     ", //must be completed to 8characters with spaces
	.ext     = "TXT",
	.maxsize = 512, 
	.info    = NULL,
	.read    = led_read,
	.write   = led_write,
	.unlink  = NULL,
    },
};

const mmapfs_t _mmapfs = {
    // use of sizeof(mmapfs_files) / sizeof(struct mmap_entry_t)
    // discards const qualifier
    .size  = 1, 
    .files = mmapfs_files,
};

const uint8_t  scsi_max_lun = 0;
const scsi_lun_t scsi_lun[] = {
    { 
	.type          = USB_MSC_MMAPFS,
	.info          = & _mmapfs,
	.init          = scsi_mmapfs_init,
	.read_capacity = scsi_mmapfs_read_capacity,
	.read10        = scsi_mmapfs_read10,
	.write10       = scsi_mmapfs_write10 
    }
};

/* ************************************************** */
/* ************************************************** */


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

