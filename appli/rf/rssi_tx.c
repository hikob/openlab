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
#include "packet.h"
#include "packer.h"
#include "random.h"

#include "printf.h"
#include "debug.h"

#define PKT_PERIOD soft_timer_ms_to_ticks(10)
#define PKT_LENGTH 80

static void char_rx(handler_arg_t arg, uint8_t c);

static void start(handler_arg_t arg);
static void tx_time(handler_arg_t arg);
static void tx_done(phy_status_t status);

static soft_timer_t tx_timer;
static phy_packet_t pkt;
static uint32_t global_counter = 0;

static volatile int tx_pow_ix = 0;
static volatile int tx_channel = -1;

static void uart_putc(char c, void* arg)
{
    uart_transfer(uart_external, (const uint8_t*) &c, 1);
}
static struct fprintf_stream uart_out =
{
        .putc = uart_putc
};

const struct
{
    char* txt;
    phy_power_t value;
} powers[] =
{
{ .txt = "PMax", .value = PHY_POWER_3dBm },
{ .txt = "PMax - 3dBm", .value = PHY_POWER_0dBm },
{ .txt = "PMax - 5dBm", .value = PHY_POWER_m2dBm },
{ .txt = "PMax - 10dBm", .value = PHY_POWER_m7dBm },
{ .txt = "PMax - 15dBm", .value = PHY_POWER_m12dBm },
{ .txt = "PMax - 20dBm", .value = PHY_POWER_m17dBm }, };

static void show_help(handler_arg_t arg)
{
    fprintf(&uart_out, "\n\nSelect the TX power with the following values:\r\n");
    int i;
    for (i = 0; i < 6; i++)
    {
        fprintf(&uart_out, "\t'%c': \t%s\r\n", '1' + i, powers[i].txt);
    }
    fprintf(&uart_out, "Select the channel to use with the following values:\r\n");
    for (i = 0; i < 16; i++)
    {
        fprintf(&uart_out, "\t'%c': \tchannel %u\r\n", 'a' + i, 11 + i);
    }
    fprintf(&uart_out, "\t' ': \tOFF mode\r\n");
}

static void show_tx_power(handler_arg_t arg)
{
    fprintf(&uart_out, "TX power set to: %s\r\n", powers[tx_pow_ix].txt);
}
static void show_tx_channel(handler_arg_t arg)
{
    if (tx_channel < 0)
    {
        fprintf(&uart_out, "TX OFF mode\r\n");
        return;
    }
    fprintf(&uart_out, "TX channel set to: %u\r\n", tx_channel);
}
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

    soft_timer_init();
    event_post(EVENT_QUEUE_APPLI, start, NULL );

    uart_set_rx_handler(uart_external, char_rx, NULL );
    show_help(NULL );

    platform_run();

    return 0;
}

static void start(handler_arg_t arg)
{
    // Reset the PHY
    phy_reset(phy);

    // Set IDLE
    phy_idle(phy);

    // Setup the timer
    soft_timer_set_handler(&tx_timer, tx_time, NULL );
    soft_timer_start(&tx_timer, PKT_PERIOD, 1);

    // Clear packet flag
    pkt.length = 0;
}

static void tx_time(handler_arg_t arg)
{
    // Check if last packet sent
    if (pkt.length)
    {
        log_error("previous packet not yet sent");
        return;
    }

    if (tx_channel < 0)
    {
        phy_sleep(phy);
        return;
    }

    // Compute channel
    uint8_t chan = tx_channel;
    phy_set_channel(phy, chan);

    // Set power
    phy_set_power(phy, powers[tx_pow_ix].value);

    // Set data pointer
    pkt.data = pkt.raw_data + 1;

    int i;

    // Fill with random data
    for (i = 3; i < PKT_LENGTH; i++)
    {
        pkt.data[i] = random_rand16();
    }

    // First byte is a FLAG
    pkt.data[0] = 0xB0;

    // 2nd ant 3rd bytes are the counter
    packer_uint16_pack(pkt.data + 1, global_counter);

    // Set length
    pkt.length = PKT_LENGTH;

    // Send now
    if (phy_tx(phy, 0, &pkt, tx_done) != PHY_SUCCESS)
    {
        log_error("Failed to send packet");
        pkt.length = 0;
    }

    // Increment counters
    global_counter++;

    static uint32_t last_print = 0;
    if (last_print == 0)
    {
        last_print = soft_timer_time();
    }
    if (soft_timer_time() - last_print > soft_timer_s_to_ticks(2))
    {
        fprintf(&uart_out, "[Chan. %u] %u packets sent\r\n", tx_channel, global_counter);
        last_print = soft_timer_time();
    }
}

static void tx_done(phy_status_t status)
{
    if (status != PHY_SUCCESS)
    {
        log_error("Error while sending packet");
    }

    // Clear the flag
    pkt.length = 0;
}

static void char_rx(handler_arg_t arg, uint8_t c)
{
    switch (c)
    {
        case '1' ... '6':
            tx_pow_ix = c - '1';
            event_post_from_isr(EVENT_QUEUE_APPLI, show_tx_power, NULL );
            break;

        case ' ':
            tx_channel = -1;
            global_counter = 0;
            event_post_from_isr(EVENT_QUEUE_APPLI, show_tx_channel, NULL );
            break;

        case 'a' ... 'p':
            tx_channel = 11 + (c - 'a');
            global_counter = 0;
            event_post_from_isr(EVENT_QUEUE_APPLI, show_tx_channel, NULL );
            break;

        case '?':
            event_post_from_isr(EVENT_QUEUE_APPLI, show_help, NULL );
            break;
        default:
            // Unknown code
            break;
    }
}
