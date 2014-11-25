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

/*
 * example_tdma_coord_static.c
 *
 * \brief Example of TDMA coordinator with static slotframe
 *
 * \date Jan 09, 2013
 * \author: Damien Hedde <damien.hedde.at.hikob.com>
 */

#include "platform.h"
#include "packet.h"
#include "soft_timer.h"

#include "mac_tdma.h"

#include "debug.h"

/*
 * coordinator configuration:
 * + network ID 0x6666
 * + channel 21
 * + 25 slots of 25ms each
 * + following slotframe definition
 */
static const uint16_t cfg_sf[25] = {
    0x8001, // our address
    0xffff, // the bcast slot
    0x0000, // empty slot
    0x0001, // a node with addr 0x1
    0x0002, // a node with addr 0x2
    0x0003, // a node with addr 0x3
    0x0004, // a node with addr 0x4
    0x0005, // a node with addr 0x5
    0x0000, // empty slot
    0x0006, // a node with addr 0x6
    0x0007, // a node with addr 0x7
    0x0008, // a node with addr 0x8
    0x0009, // a node with addr 0x9
    0x000a, // a node with addr 0xa
    0x0000, // empty slot
    0x000b, // a node with addr 0xb
    0x000c, // a node with addr 0xc
    0x000d, // a node with addr 0xd
    0x000e, // a node with addr 0xe
    0x000f, // a node with addr 0xf
    // remaining slots will be empty
};
static const mac_tdma_coord_config_t cfg = {
    /* network id */
    .panid = 0x6666,
    /* phy channel */
    .channel = 21,
    /* slot duration in tdma time unit (default 1unit = 100us) */
    .slot_duration = 250,
    /* number of slots */
    .slot_count = 25,
    /* slots-frame description */
    .slotsframe = cfg_sf,
};

static soft_timer_t timer;
static uint8_t index;

static void pkt_tick(handler_arg_t arg);
static void pkt_sent(void *arg, enum tdma_result res);
static void pkt_received(packet_t *packet, uint16_t src);

int main()
{
    platform_init();

    /*
     * init tdma with our address
     * (must correspond to the one put the slotframe)
     */
    mac_tdma_init(0x8001);

    /* start coordinator */
    mac_tdma_start_coord(&cfg);

    /* register data packet handler */
    mac_tdma_set_recv_handler(pkt_received);

    /*
     * programm periodic timer to send packet
     */
    soft_timer_set_handler(&timer, pkt_tick, NULL);
    soft_timer_start(&timer, SOFT_TIMER_FREQUENCY, 1);

    /* shutdown leds */
    leds_off(0xf);

    platform_run();
    return 0;
}

static void pkt_tick(handler_arg_t arg)
{
    enum tdma_result res;

    /* allocate a packet */
    packet_t *packet = packet_alloc(0);
    if (!packet)
    {
        log_error("Can't allocate a packet");
        return;
    }

    /* fill it */
    log_printf("Send packet %u\n", index);
    *(packet->data) = index++;
    packet->length = 1;

    /* broadcast it */
    if ((res = mac_tdma_send(packet, 0xffff, pkt_sent, packet)) != TDMA_OK)
    {
        packet_free((packet_t *) arg);
        log_printf("Packet sending failed %d\n", res);
    }
}

static void pkt_sent(void *arg, enum tdma_result res)
{
    // unused
    (void) res;

    packet_t *packet = (packet_t *) arg;

    log_printf("Packet sending result %d\n", res);

    /* blink led */
    if (*(packet->data) % 2)
    {
        leds_on(LED_1);
    }
    else
    {
        leds_off(LED_1);
    }

    /* free packet */
    packet_free((packet_t *) arg);
}

static void pkt_received(packet_t *packet, uint16_t src)
{
    // unused
    (void) src;

    if (packet->length == 1)
    {
        log_printf("Packet received from 0x%04x : %u\n", src, *(packet->data));
    }
    else
    {
        log_printf("Unknown Packet received from 0x%04x\n", src);
    }
    packet_free(packet);
}
