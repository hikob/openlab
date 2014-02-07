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
 * simplesniff.c
 *
 *  Created on: Jul 17, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "printf.h"
#include "uart.h"

#include "phy.h"

#define CHAN_PERIOD (configTICK_RATE_HZ * 4)
static uint8_t channels[] =
{ 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26 };
static uint32_t channels_len = sizeof(channels);

// Task function
static void test_task(void *);
static void rx_done(phy_status_t status);

// Private variables
static xQueueHandle rx_queue;
static phy_packet_t rx_pkt, pkt;

int main()
{
    // Initialize the platform
    platform_init();

    // Create a test task
    xTaskCreate(test_task, (signed char *)"test", configMINIMAL_STACK_SIZE / 2,
            NULL, 1, NULL);

    // Create a Queue for received packets
    rx_queue = xQueueCreate(1, 128);

    // Run
    platform_run();
    return 0;
}

void test_task(void *arg)
{
    // Debug
    printf("PHY Simple Sniffer\n");

    // Reset
    phy_reset(platform_phy);

    // Set channel
    phy_set_channel(platform_phy, 12);

    portTickType next_switch = xTaskGetTickCount();
    int current_channel_ix = 0;
    while (1)
    {
        next_switch += CHAN_PERIOD;
        // Reset RX
        phy_idle(platform_phy);

        // Set current channel
        phy_set_channel(platform_phy, channels[current_channel_ix]);

        // Send message
        // Send startup sequence
        uint8_t msg_chan[3] =
        { 0xab, 0xcc, channels[current_channel_ix] };
        printf("Channel: %u\n", channels[current_channel_ix]);
        uart_transfer(uart_print, msg_chan, 3);

        // Send packet
        uart_transfer(uart_print, pkt.data, pkt.length + 1);

        // Set RX
        rx_pkt.data = rx_pkt.raw_data + 1;
        phy_rx_now(platform_phy, &rx_pkt, rx_done);

        // Get a packet from the queue
        while (xQueueReceive(rx_queue, &pkt, next_switch - xTaskGetTickCount())
                == pdTRUE)
        {
            // Send startup sequence
            uint8_t startup[2] =
            { 0xab, 0xcd };
            uart_transfer(uart_print, startup, 2);

            // Send packet
            uart_transfer(uart_print, pkt.data, pkt.length + 1);

            // Set RX
            rx_pkt.data = rx_pkt.raw_data + 1;
            phy_rx_now(platform_phy, &rx_pkt, rx_done);
        }

        // Increment channel
        current_channel_ix += 1;
        current_channel_ix %= channels_len;
    }

}

static void rx_done(phy_status_t status)
{
    if (status != PHY_SUCCESS)
    {
        // Reset RX
        phy_rx_now(platform_phy, &rx_pkt, rx_done);
        return;
    }

    // Toggle
    leds_toggle(LED_1);

    // Store length
    rx_pkt.data--;
    *rx_pkt.data = rx_pkt.length;

    // Send packet to queue
    xQueueSend(rx_queue, &rx_pkt, 0);

}
