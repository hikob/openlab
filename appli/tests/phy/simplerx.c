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
 * Copyright (C) 2011 HiKoB.
 */

/*
 * blink.c
 *
 *  Created on: Jul 15, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "printf.h"

#include "phy.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

// Task function
static void test_task(void *);
static void rx_done(phy_status_t status);

static phy_packet_t pkt;
static xSemaphoreHandle rx_sem;

int main()
{
    // Initialize the platform
    platform_init();

    // Create a test task
    xTaskCreate(test_task, (const signed char * const)"test",
                4 * configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Create a semaphore and take it
    vSemaphoreCreateBinary(rx_sem);
    xSemaphoreTake(rx_sem, 0);

    platform_run();
    return 0;
}

void test_task(void *arg)
{
    (void) arg;

    // Debug
    printf("PHY SimpleRX test\n");

    // Reset the PHY
    phy_reset(phy);

    // Set channel
    phy_set_channel(phy, 15);

    // Go to sleep
    phy_sleep(phy);

    while(1)
    {
        // Enter RX now
        pkt.data = pkt.raw_data;
        pkt.length = 0;
        phy_rx(phy, 0, 0, &pkt, rx_done);

        // Take the semaphore, to wait until taken
        while(xSemaphoreTake(rx_sem, configTICK_RATE_HZ) != pdTRUE)
        {
            leds_toggle(LED_0);
        }

        // Go to sleep
        phy_sleep(phy);

        if(pkt.length > 0)
        {
            leds_toggle(LED_1);
            printf("Packet Received !!\n");
            printf("\tlength = %u\n", pkt.length);
            printf("\ttimestamp = %u\n", pkt.timestamp);
            printf("\trssi = %d\n", pkt.rssi);
            printf("\tlqi = %d\n", pkt.lqi);

            pkt.data[pkt.length] = 0;
            printf("Data: %s\n", pkt.data);
        }

        // Wait a little
        vTaskDelay(configTICK_RATE_HZ / 4);
    }

}

static void rx_done(phy_status_t status)
{
    // Check status
    switch(status)
    {
        case PHY_RX_TIMEOUT_ERROR:
            printf("PHY RX timeout\n");
            break;
        case PHY_RX_CRC_ERROR:
        case PHY_RX_ERROR:
        case PHY_RX_LENGTH_ERROR:
            printf("PHY RX error %x\n", status);
            break;
        case PHY_SUCCESS:
            // Valid frame
            break;
        default:
            // Weird status
            break;
    }

    // Give semaphore
    xSemaphoreGive(rx_sem);
}
