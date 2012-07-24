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
 * simpletx.c
 *
 *  Created on: Jul 5, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include <string.h>
#include "platform.h"
#include "printf.h"

#include "phy.h"
#include "soft_timer.h"

// Task function
static void test_task(void *);
static void tx_done(phy_status_t status);

static const uint8_t msg[] = "Hello World From Azure Lion 000";
static phy_packet_t pkt;
static xSemaphoreHandle tx_sem;

int main()
{
    // Initialize the platform
    platform_init();

    // Create a test task
    xTaskCreate(test_task, (const signed char * const)"test",
                configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Create a semaphore and take it
    vSemaphoreCreateBinary(tx_sem);
    xSemaphoreTake(tx_sem, 0);

    platform_run();
    return 0;
}

void test_task(void *arg)
{
    // Debug
    printf("PHY SimpleTX test\n");

    // Reset PHY
    phy_reset(phy);

    // Set channel
    phy_set_channel(phy, 15);

    uint8_t count = 0;

    while (1)
    {
        // Prepare packet
        pkt.data = pkt.raw_data;
        pkt.length = sizeof(msg);
        memcpy(pkt.data, msg, pkt.length);

        pkt.data[pkt.length - 4] = '0' + ((count / 100) % 10);
        pkt.data[pkt.length - 3] = '0' + ((count / 10) % 10);
        pkt.data[pkt.length - 2] = '0' + ((count / 1) % 10);

        count ++;

        // Send in 10ms, no handler
        printf("Sending a Radio Packet, length: %u, data: %s\n", pkt.length,
               pkt.data);
        uint32_t t = soft_timer_time() + soft_timer_ms_to_ticks(10);
        phy_tx(phy, t, &pkt, tx_done);

        // Take the semaphore, to wait until taken
        while (xSemaphoreTake(tx_sem, configTICK_RATE_HZ) != pdTRUE)
        {
            leds_toggle(LED_1);
        }

        // Go to sleep
        phy_sleep(phy);

        // Wait a little
        vTaskDelay(configTICK_RATE_HZ * 4);

        // Toggle
        leds_toggle(LED_0);
    }

}

static void tx_done(phy_status_t status)
{
    if (status == PHY_SUCCESS)
    {
        log_debug("Frame sent at %u, length: %u", pkt.timestamp, pkt.length);
    }
    else
    {
        log_debug("Error while sending %x", status);
    }

    xSemaphoreGive(tx_sem);

}
