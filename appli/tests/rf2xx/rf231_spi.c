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
 * rf231_spi.c
 *
 *  Created on: Oct 12, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include <stdlib.h>
#include "platform.h"
#include "rf2xx.h"
#include "rf2xx/rf2xx_regs.h"
#include "printf.h"
#include "debug.h"

extern rf2xx_t rf231;

#define TEST_LENGTH 126

static void transfer_done(handler_arg_t arg);

static uint8_t tx_fifo[128], rx_fifo[128];
static volatile uint32_t done;

int main()
{
    // Initialize the platform
    platform_init();

    printf("Testing the RF231 SPI\n");

    // Set initial values
    leds_on(LED_0);
    leds_off(LED_1);

    // Init. the radio
    rf2xx_wakeup(rf231);
    rf2xx_set_state(rf231, RF2XX_TRX_STATE__FORCE_TRX_OFF);

    while (1)
    {
        uint32_t i;

        for (i = 0; i < TEST_LENGTH; i++)
        {
            tx_fifo[i] = rand();
            rx_fifo[i] = 0;
        }

        printf("Writing  to FIFO...");

        done = 0;
        rf2xx_fifo_write_async(rf231, tx_fifo, TEST_LENGTH, transfer_done, NULL);

        while (!done)
        {
            // Wait
        }

        printf("\tReading from FIFO...");
        done = 0;
        rf2xx_fifo_read_async(rf231, rx_fifo, TEST_LENGTH, transfer_done, NULL);

        while (!done)
        {
            // Wait
        }

        printf("\tComparing...");
        // Compare
        uint32_t err = 0;

        for (i = 0; i < TEST_LENGTH - 1; i++)
        {
            err += (tx_fifo[i] != rx_fifo[1 + i]);
        }

        if (err == 0)
        {
            printf("\tNo error\n");
        }
        else
        {
            printf("\t%u errors!\n", err);
            printf("Diff:\nid\tTX\tRX\n");

            for (i = 0; i < TEST_LENGTH - 1; i++)
            {
                if (tx_fifo[i] != rx_fifo[1 + i])
                {
                    printf("\x1b[31m%u\t%x\t%x\x1b[0m\n", i, tx_fifo[i], rx_fifo[i + 1]);
                }
            }

            HALT();
        }
    }

    return 0;
}

static void transfer_done(handler_arg_t arg)
{
    done = 1;
}
