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
#include "phy.h"

#include "soft_timer.h"
#include "debug.h"

// Task function
static void tx(handler_arg_t arg);
static void tx_done(phy_status_t status);

static const uint8_t msg[] = "Hello World 000";
static phy_packet_t pkt;
static soft_timer_t tim;

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

    // Configure periodic sending
    soft_timer_init();
    soft_timer_set_handler(&tim, tx, NULL);
    soft_timer_start(&tim, soft_timer_s_to_ticks(1), 1);

    printf("PHY SimpleTX test\n");
    phy_reset(PHY);
    phy_set_channel(PHY, 15);

    platform_run();
    return 0;
}

static void tx(handler_arg_t arg)
{
    static uint8_t count = 0;

    phy_idle(PHY);

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
    phy_tx(PHY, t, &pkt, tx_done);
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
}
