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
 * n25xxx.h
 *
 *  Created on: May 18, 2012
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "printf.h"
#include "debug.h"
#include "n25xxx.h"
#include "random.h"

static void app_task(void *);

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
    static uint8_t buf[256];
    static uint16_t i;
    static uint32_t page;

    log_printf("# Testing N25XXX\n");

    /** Read the chip ID */
    log_printf("# ID: ");

    n25xxx_read_id(buf, 20);

    for (i = 0; i < 20; i++)
    {
        log_printf("%02X", buf[i]);
    }

    log_printf("\n");

    /** Bulk erase */
    log_printf("# Bulk erase this operation can take up to 250s... be patient!\n");

    n25xxx_write_enable();
    n25xxx_bulk_erase();

    /** Read content of a page */
    page = random_rand16();
    
    log_printf("# Reading page: %d\n", page);

    n25xxx_read(page << 8, buf, 256);

    for(i = 0; i < 256; i++)
    {
        if(buf[i] != 0xFF)
        {
            log_error("Page should be blank but its not...");
            while(1);
        }
    }

    /** Writing a page */
    log_printf("# Writing page: %d\n", page);
    for(i = 0; i < 256; i++)
    {
        buf[i] = 3 * i;
    }

    n25xxx_write_enable();
    n25xxx_write_page(page << 8, buf);

    /** Reading a page */
    log_printf("# Re-reading page: %d\n", page);
    
    n25xxx_read(page << 8, buf, 256);

    for(i = 0; i < 256; i++)
    {
        if(buf[i] != (uint8_t)(3 * i))
        {
            log_error("Page has not the right content...");
            while(1);
        }
    }

    log_printf("Test successfull\n");

    while (1)
    {
        ;
    }
}
