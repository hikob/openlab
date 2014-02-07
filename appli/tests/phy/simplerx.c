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
 * simplerx.c
 *
 *  Created on: Jul 5, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "printf.h"

#include "phy.h"
#include "soft_timer.h"

#include "event.h"

// Task function
static void enter_rx(handler_arg_t arg);
static void process_packet(handler_arg_t arg);
static void rx_done(phy_status_t status);

static phy_packet_t pkt;

#if 1
#define PHY platform_phy
#else
extern phy_t phy_868;
#define PHY phy_868
#endif

int main()
{
    // Initialize the platform
    platform_init();

    printf("PHY SimpleRX test\n");
    event_post(EVENT_QUEUE_APPLI, enter_rx, NULL);

    platform_run();
    return 0;
}

static void enter_rx(handler_arg_t arg)
{
    phy_idle(PHY);

    // Set channel
    phy_set_channel(PHY, 15);

    // Enter RX now
    pkt.data = pkt.raw_data;
    pkt.length = 0;
    phy_rx(PHY, 0, 0, &pkt, rx_done);
}

static void process_packet(handler_arg_t arg)
{
    // Go to sleep
    phy_sleep(PHY);

    if (pkt.length > 0)
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

    // Back to RX
    enter_rx(NULL);
}

static void rx_done(phy_status_t status)
{
    // Check status
    switch (status)
    {
        case PHY_RX_TIMEOUT_ERROR:
            printf("PHY RX timeout\n");
            break;
        case PHY_RX_CRC_ERROR:
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

    // Post packet processing
    event_post(EVENT_QUEUE_APPLI, process_packet, NULL);
}
