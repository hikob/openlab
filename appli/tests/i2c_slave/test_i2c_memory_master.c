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
 * Copyright (C) 2013 HiKoB.
 */

/**
 * \file test_i2c_master_mem.c
 * \date Nov 12, 2013
 * \author Damien Hedde <damien.hedde.at.hikob.com>
 */

/**
 * This test application implements an application with uses
 * a 256 bytes memory on I2C slaves.
 * It can be used to test an i2c link to a slave running
 * the test_i2c_slave_mem.c application.
 */

#include "platform.h"
#include "FreeRTOS.h"

#include "drivers.h"

#include "debug.h"
#include "ansi.h"

#define DEV_ADDR 0x42

static void i2c_master(void *arg);

int main()
{
    // Initialize the platform
    platform_init();

    leds_off(0xff);

    // Create a task for the application
    if (xTaskCreate(i2c_master, (const signed char * const) "i2c_master", configMINIMAL_STACK_SIZE, NULL, 1, NULL) != pdPASS)
    {
        log_error("Failed to create task");
    }

    // Run
    platform_run();
    return 0;
}

static void i2c_master(void *param)
{
    // unused
    (void) param;

    static uint8_t mem[256];
    static uint8_t buf[129];
    int i;
    log_info("i2c master task");

    // init memory
    for (i = 0; i < 256; i++)
    {
        mem[i] = i;
    }

    while (1)
    {
        uint8_t size;
        // write memory
        // we write [1,1] , [2,3] ,[4,7], ... [128,255]
        size = 1;
        while (size)
        {
            uint8_t idx = size;
            // put offset first
            buf[0] = idx;
            // then put data
            for (i = 0; i < size; i++)
            {
                buf[i+1] = mem[i + idx];
            }

            log_printf("** write [%u;%u] ...", idx, idx + size - 1);
            leds_on(LED_0);
            if (i2c_tx(i2c_external, DEV_ADDR, buf, 1 + size))
            {
                ansi_set_text_color(ANSI_RED);
                log_printf("  failed\n");
            }
            else
            {
                ansi_set_text_color(ANSI_GREEN);
                log_printf("  ok\n");
            }
            leds_off(LED_0);
            ansi_set_text_color(ANSI_WHITE);

            vTaskDelay(configTICK_RATE_HZ);
            size *= 2;
        }

        // read memory
        // we read [255,255] , [253,254] , [249,252], ... [1,128]
        size = 1;
        while (size)
        {
            uint8_t idx = 256 - 2 * size + 1;
            for (i = 0; i < size; i++)
            {
                buf[i] = 0;
            }

            log_printf("** read [%u;%u] ...", idx, idx + size - 1);
            leds_on(LED_1);
            if (i2c_tx_rx(i2c_external, DEV_ADDR, &idx, 1, buf, size))
            {
                ansi_set_text_color(ANSI_RED);
                log_printf("  failed\n");
            }
            else
            {
                ansi_set_text_color(ANSI_GREEN);
                log_printf("  ok\n");
                for (i = 0; i < size; i++)
                {
                    if (buf[i] != mem[idx + i])
                    {
                        ansi_set_text_color(ANSI_RED);
                        log_printf(" -> corrupted byte %u: received %u vs%u expected\n", idx + i, buf[i], mem[idx+i]);
                        break;
                    }
                }
            }
            leds_off(LED_1);
            ansi_set_text_color(ANSI_WHITE);

            vTaskDelay(configTICK_RATE_HZ);
            size *= 2;
        }

        vTaskDelay(5 * configTICK_RATE_HZ);

        // modify memory content
        for (i = 0; i < 256; i++)
        {
            mem[i] = mem[i] * 3 + 1;
        }
        log_printf("\n");
    }
}

