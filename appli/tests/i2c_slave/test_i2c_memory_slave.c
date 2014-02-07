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
 * \file test_i2c_slave_mem.c
 * \date Nov 12, 2013
 * \author Damien Hedde <damien.hedde.at.hikob.com>
 */

/**
 * This test application implements an I2C slave as a 256 bytes memory.
 * It uses classical I2C addressing mecanism,
 * first received byte of a transaction contains the target memory address.
 */

#include <stdbool.h>

#include "platform.h"
#include "FreeRTOS.h"
#include "debug.h"

#include "drivers.h"
#include "i2c_slave.h"

#define DEV_ADDR 0x42

/* memory array */
static uint8_t mem[256] = {0};
/* current offset pointer */
static uint8_t offset = 0;

static void i2c_slave_fsm(i2c_slave_event_t ev, uint8_t *data);

int main()
{
    // Initialize the platform
    platform_init();

    leds_off(0xff);

    i2c_slave_set_address(i2c_external, DEV_ADDR);
    i2c_slave_configure(i2c_external, i2c_slave_fsm);

    // Run
    platform_run();
    return 0;
}

static void i2c_slave_fsm(i2c_slave_event_t ev, uint8_t *data)
{
    /* called from interrupt context */
    static bool next_byte_is_address = false;
    switch(ev)
    {
        case I2C_SLAVE_EV_RX_START:
            leds_off(LED_0 | LED_1);
            // indicate we will receive a new offset
            next_byte_is_address = true;
            log_printf("start rx\n");
            break;

        case I2C_SLAVE_EV_TX_START:
            leds_off(LED_0 | LED_1);
            log_printf("start tx\n");
            break;

        case I2C_SLAVE_EV_STOP:
            leds_off(LED_0 | LED_1);
            log_printf("stop\n\n");
            break;

        case I2C_SLAVE_EV_RX_BYTE:
            if (next_byte_is_address)
            {
                // store the new offset
                next_byte_is_address = false;
                offset = *data;
                log_printf("Set offset to @%u\n", offset);
            }
            else
            {
                // store data byte and increment offset
                log_printf("Received %u for @%u\n", *data, offset);
                mem[offset++] = *data;
                leds_on(LED_1);
            }
            break;

        case I2C_SLAVE_EV_TX_BYTE:
            // load data byte and increment offset
            log_printf("Sent %u from @%u\n", mem[offset], offset);
            *data = mem[offset++];
            leds_on(LED_0);
            break;

        default:
            log_error("I2C slave Unexpected event %d", ev);
    }
}
