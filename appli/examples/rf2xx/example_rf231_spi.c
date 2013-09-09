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

/**
 * \example example_rf231_spi.c
 *
 * This example shows how to use the asynchronous features for read/write
 * accesses to the FIFO of the RF231.
 *
 * \date Oct 1, 2012
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include <stdlib.h>
#include "platform.h"
#include "printf.h"

#include "rf2xx.h"
#include "rf2xx/rf2xx_regs.h"

extern rf2xx_t rf231;

#define TEST_LENGTH 126

static void app_task(void *);
static void transfer_done(handler_arg_t arg);

static uint8_t tx_fifo[128], rx_fifo[128];
static xSemaphoreHandle radio_sem;

int main()
{
    // Initialize the platform
    platform_init();

    // Create a task for the application
    xTaskCreate(app_task, (const signed char * const) "rf231", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Create a task for the synchronization
    vSemaphoreCreateBinary(radio_sem);

    // Run
    platform_run();
    return 0;
}

static void app_task(void *param)
{
    printf("Testing the RF231 asynchronous SPI\n");

    // Initialize the radio
    rf2xx_reset(rf231);
    rf2xx_wakeup(rf231);

    while (1)
    {
        uint32_t i;

        // Set TX buffer to random data, RX buffer to all zeros
        for (i = 0; i < TEST_LENGTH; i++)
        {
            tx_fifo[i] = rand();
            rx_fifo[i] = 0;
        }

        printf("Writing  to FIFO...");
        // Write to FIFO
        xSemaphoreTake(radio_sem, 0);
        rf2xx_fifo_write_async(rf231, tx_fifo, TEST_LENGTH, transfer_done, NULL);

        // Wait for end of transfer
        xSemaphoreTake(radio_sem, portMAX_DELAY);

        printf("\tReading from FIFO...");
        // Read from FIFO
        xSemaphoreTake(radio_sem, 0);
        rf2xx_fifo_read_async(rf231, rx_fifo, TEST_LENGTH, transfer_done, NULL);

        // Wait for end of transfer
        xSemaphoreTake(radio_sem, portMAX_DELAY);

        printf("\tComparing...");
        // Compare both buffers
        uint32_t err = 0;
        for (i = 0; i < TEST_LENGTH - 1; i++)
        {
            err += (tx_fifo[i] != rx_fifo[1 + i]);
        }

        if (err == 0)
        {
            printf("\t\x1b[32mNo error\x1b[0m\n");
        }
        else
        {
            printf("\t\x1b[31m%u errors!\x1b[0m\n", err);
            printf("Diff:\nid\tTX\tRX\n");

            for (i = 0; i < TEST_LENGTH - 1; i++)
            {
                if (tx_fifo[i] != rx_fifo[1 + i])
                {
                    printf("\x1b[31m%u\t%x\t%x\x1b[0m\n", i, tx_fifo[i], rx_fifo[i + 1]);
                }
            }
        }

        // Wait a little and start over
        vTaskDelay(configTICK_RATE_HZ / 2);
    }
}
static void transfer_done(handler_arg_t arg)
{
    portBASE_TYPE yield;
    xSemaphoreGiveFromISR(radio_sem, &yield);
    if (yield)
    {
        portYIELD();
    }
}

