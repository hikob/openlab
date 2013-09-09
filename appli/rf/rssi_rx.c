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
 * rssi_tx.c
 *
 *  Created on: Dec 14, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include <stdint.h>
#include "platform.h"

#include "phy.h"
#include "event.h"
#include "soft_timer.h"
#include "packer.h"

#include "FreeRTOS.h"
#include "task.h"

#include "printf.h"
#include "debug.h"

#define PERIOD soft_timer_ms_to_ticks(2000)

static void show_help(handler_arg_t arg);
static void char_rx(handler_arg_t arg, uint8_t c);

static void start_rx(handler_arg_t arg);
static void rx_done(phy_status_t status);

static void check_packets(handler_arg_t arg);

static soft_timer_t check_timer;
static phy_packet_t pkt;

static int rx_channel = -1;

static void uart_putc(char c, void* arg)
{
    uart_transfer(uart_external, (const uint8_t*) &c, 1);
}
static struct fprintf_stream uart_out =
{
        .putc = uart_putc
};

typedef struct
{
    // Number of received packets
    uint32_t received;
    // Number of missed packets
    uint32_t missed;

    // Last ID received
    uint16_t last_id;

    // Total RSSI for averaging
    int32_t total_rssi;
    // Total LQI for averaging
    uint32_t total_lqi;
} stats_t;

static stats_t stats;

int main()
{
    // Initialize the platform
    platform_init();

    if (uart_external == NULL)
    {
        uart_external = uart_print;
    }
    else
    {
        uart_enable(uart_external, 500000);
    }

    // Initialize the soft timer library
    soft_timer_init();

    // Prepare the timer
    soft_timer_set_handler(&check_timer, check_packets, NULL );
    soft_timer_start(&check_timer, PERIOD, 1);

    uart_set_rx_handler(uart_external, char_rx, NULL );
    show_help(NULL );

    // Start RX
    event_post(EVENT_QUEUE_APPLI, start_rx, NULL );

    platform_run();
    return 0;
}

static void show_help(handler_arg_t arg)
{
    fprintf(&uart_out, "\n\nSelect the channel to use with the following values:\r\n");
    int i;
    for (i = 0; i < 16; i++)
    {
        fprintf(&uart_out, "\t'%c': \tchannel %u\r\n", 'a' + i, 11 + i);
    }
    fprintf(&uart_out, "\t' ': \tOFF mode\r\n");
}
static void start_rx(handler_arg_t arg)
{
    if (rx_channel < 0)
    {
        phy_reset(phy);
        phy_sleep(phy);
        return;
    }

    phy_reset(phy);
    phy_idle(phy);
    phy_set_channel(phy, rx_channel);

    // Clear all stats
    stats.missed = 0;
    stats.received = 0;
    stats.last_id = 0;
    stats.total_lqi = 0;
    stats.total_rssi = 0;

    fprintf(&uart_out, "RX on channel %u\r\n", rx_channel);

    // Start RX
    pkt.data = pkt.raw_data + 1;
    phy_rx(phy, 0, 0, &pkt, rx_done);
}

static void rx_done(phy_status_t status)
{
    // Check status
    if ((status == PHY_SUCCESS) && (pkt.data[0] == 0xB0))
    {
        uint16_t counter;
        packer_uint16_unpack(pkt.data + 1, &counter);

        // Compute missed
        if (stats.received)
        {

            uint32_t missed = (65535 + counter - stats.last_id) % 65536;
            stats.missed += missed;
        }

        // Update stats
        stats.received += 1;
        stats.last_id = counter;
        stats.total_lqi += pkt.lqi;
        stats.total_rssi += pkt.rssi;
    }

    // Return to RX
    pkt.data = pkt.raw_data + 1;
    phy_rx(phy, 0, 0, &pkt, rx_done);
}

static void check_packets(handler_arg_t arg)
{
    if (rx_channel < 0)
    {
        fprintf(&uart_out, "\t[OFF mode]\r\n");
        return;
    }
    fprintf(&uart_out, "\t[Chan: %d] RX: %u", rx_channel, stats.received);

    if (stats.received)
    {
        float per, rssi, lqi;

        per = stats.missed;
        per /= (stats.missed + stats.received);

        rssi = stats.total_rssi;
        rssi /= stats.received;

        lqi = stats.total_lqi;
        lqi /= stats.received;
        fprintf(&uart_out, "\tMissed: %u \tPER: %u%% \tRSSI: %d.%u \tLQI: %d.%u",
                stats.missed, (int32_t )(per * 100), (int32_t ) rssi,
                (int32_t )((((float )(int ) rssi - rssi) * 10)), (int32_t ) lqi,
                (uint32_t )((lqi - (int ) lqi) * 10));
    }

    fprintf(&uart_out, "\r\n");
}

static void char_rx(handler_arg_t arg, uint8_t c)
{
    switch (c)
    {
        case ' ':
            rx_channel = -1;
            event_post_from_isr(EVENT_QUEUE_APPLI, start_rx, NULL );
            break;
        case 'a' ... 'p':
            rx_channel = 11 + (c - 'a');
            event_post_from_isr(EVENT_QUEUE_APPLI, start_rx, NULL );
            break;

        case '?':
            event_post_from_isr(EVENT_QUEUE_APPLI, show_help, NULL );
            break;
        default:
            // Unknown code
            break;
    }
}
