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
 * example_tdma_coord.c
 *
 * \brief Example of TDMA coordinator
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
 * coordinator configuration
 */
static mac_tdma_pan_config_t cfg = {
    /* network id */
    .panid = 0x6666,
    /* phy channel */
    .channel = 21,
    /* slot duration in tdma time unit (default 1unit = 100us) */
    .slot_duration = 100,
    /* number of slots (the coordinator can have count-2 nodes) */
    .slot_count = 10,
};

static soft_timer_t timer;
static uint8_t index;

static void pkt_tick(handler_arg_t arg);
static void pkt_sent(void *arg, enum tdma_result res);
static void pkt_received(packet_t *packet, uint16_t src);

int main()
{
    platform_init();

    /* init tdma (0 means automatically generated address) */
    mac_tdma_init(0);

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
