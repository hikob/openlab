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
 * coord.c
 *
 * \date Jan 09, 2013
 * \author: Damien Hedde <damien.hedde.at.hikob.com>
 */

#include <string.h>

#include "soft_timer.h"

#include "tdma_internal.h"
#include "tdma_types.h"
#include "tdma_packet.h"
#include "tdma_config.h"

#define LOG_LEVEL LOG_LEVEL_INFO
#include "debug.h"

static soft_timer_t beacon_timer;

static tdma_frame_t beacon_frame;
static uint32_t beacon_time;

static int slotsframe_is_static;

static void coord_rx_handler(tdma_frame_t *frame);
static void coord_tx_handler(tdma_frame_t *frame);
static void beacon_tick(handler_arg_t arg);
static void coord_assoc(handler_arg_t arg);

void mac_tdma_start_coord(const mac_tdma_coord_config_t *cfg)
{
    tdma_packet_t *packet;
    tdma_get();

    if (tdma_data.state != TDMA_IDLE)
    {
        log_error("TDMA is not IDLE");
        tdma_release();
        return;
    }
    if (cfg == NULL)
    {
        log_error("TDMA configuration is not present");
        tdma_release();
        return;
    }

    /* register config */
    tdma_data.pan.coord = tdma_data.addr;
    tdma_data.pan.panid = cfg->panid;
    tdma_data.pan.slot_duration = cfg->slot_duration;
    tdma_data.pan.slot_count = cfg->slot_count;
    tdma_data.pan.channel = cfg->channel;

    tdma_data.rx_handler = &coord_rx_handler;
    tdma_data.tx_handler = &coord_tx_handler;

    if (tdma_data.pan.slot_count > TDMA_MAX_SLOTS)
    {
        log_error("Too many slots");
        tdma_release();
        return;
    }

    /* start slots-frame */
    if (tdma_slot_start(0))
    {
        log_error("Can't start slots-frame");
        tdma_release();
        return;
    }

    /* init beacon frame */
    phy_prepare_packet(&beacon_frame.pkt);
    tdma_frame_prepare(&beacon_frame);
    packet = (tdma_packet_t *) &beacon_frame.pkt.raw_data[0];
    tdma_packet_header_prepare(packet, TDMA_PKT_BEACON, tdma_data.pan.panid, tdma_data.addr, 0xffff);
    packet->payload.beacon.slot_duration = tdma_data.pan.slot_duration;
    packet->payload.beacon.slot_count = tdma_data.pan.slot_count;
    packet->payload.beacon.slot_desc_off = 0;
    beacon_frame.pkt.length = TDMA_PKT_SIZE_HEADER + TDMA_PKT_SIZE_BEACON;


    if (!cfg->slotsframe)
    {
        /* dynamic slotsframe => default slots */
        slotsframe_is_static = 0;
        tdma_slot_configure(0, tdma_data.addr); // Tx
        tdma_slot_configure(tdma_data.pan.slot_count / 2, 0xffff); // Rx
    }
    else
    {
        unsigned i,bdcast = 0;
        /* static slotsframe */
        slotsframe_is_static = 1;

        /* check first slot */
        if (cfg->slotsframe[0] != tdma_data.addr)
        {
            log_error("Invalid slots-frame: slot0's owner isn't coordinator");
            tdma_slot_stop();
            tdma_release();
            return;
        }
        tdma_slot_configure(0, tdma_data.addr); // Tx

        /* add other slots */
        for (i= 1; i < tdma_data.pan.slot_count; i += 1)
        {
            uint16_t owner = cfg->slotsframe[i];
            if (owner == 0xffff)
            {
                bdcast += 1;
            }
            tdma_slot_configure(i, owner);
        }

        /* check broadcast slots */
        if (bdcast != 1)
        {
            log_error("Invalid slots-frame: there's %u broadcast RX slot(s)", bdcast);
            tdma_slot_stop();
            tdma_release();
            return;
        }
    }
    tdma_slot_print();

    /* start beacons */
    beacon_time = soft_timer_time();
    soft_timer_set_handler(&beacon_timer, beacon_tick, NULL);
    event_post(EVENT_QUEUE_APPLI, beacon_tick, NULL);
    soft_timer_start(&beacon_timer, TDMA_BEACON_PERIOD_S * SOFT_TIMER_FREQUENCY, 1);

    /* change state */
    tdma_data.tx_frames = NULL;
    tdma_data.beacon_frame = NULL;
    tdma_data.state = TDMA_COORD;

    tdma_release();
}

/*
 * Prepare a beacon
 */
static void beacon_prepare(void)
{
    /* TODO: handle big slotframes size */
    uint8_t i;
    tdma_packet_t *pkt = (tdma_packet_t *) &beacon_frame.pkt.raw_data[0];
    for (i = 0; i < tdma_data.pan.slot_count && i < 50; i++)
    {
        pkt->payload.beacon.slot_desc_own[i] = packer_uint16_hton(tdma_data.slots[i]);
    }
    pkt->payload.beacon.slot_desc_off = 0;
    pkt->payload.beacon.slot_desc_cnt = i;
    beacon_frame.pkt.length = TDMA_PKT_SIZE_HEADER + TDMA_PKT_SIZE_BEACON + 2U * i;
    tdma_data.beacon_frame = &beacon_frame;
}

/*
 * Called periodically to generate beacons
 */
static void beacon_tick(handler_arg_t arg)
{
    // unused
    (void) arg;

    tdma_get();

    log_debug("Beacon time");

    /* get a frame */
    if (beacon_frame.status != TDMA_STATUS_IDLE)
    {
        log_error("Beacon frame is not ready");
        tdma_release();
        return;
    }

    /* add beacon */
    beacon_prepare();

    tdma_release();
}

/*
 * Handler for tx packet
 */
static void coord_tx_handler(tdma_frame_t *frame)
{
    tdma_packet_t *pkt = (tdma_packet_t *) frame->pkt.data;
    switch (tdma_packet_header_type(pkt))
    {
        case TDMA_PKT_BEACON:
            /* reset the beacon frame */
            phy_prepare_packet(&beacon_frame.pkt);
            tdma_frame_prepare(&beacon_frame);
            tdma_packet_header_prepare(pkt, TDMA_PKT_BEACON, tdma_data.pan.panid, tdma_data.addr, 0xffff);
            pkt->payload.beacon.slot_duration = tdma_data.pan.slot_duration;
            pkt->payload.beacon.slot_count = tdma_data.pan.slot_count;
            pkt->payload.beacon.slot_desc_cnt = 0;
            beacon_frame.pkt.length = TDMA_PKT_SIZE_HEADER + TDMA_PKT_SIZE_BEACON;
            break;
        case TDMA_PKT_DATA:
            event_post(EVENT_QUEUE_APPLI, (handler_t) tdma_data_tx_handler, frame);
            break;
        default:
            log_error("Unexpected tx frame type %d", tdma_packet_header_type(pkt));
            break;
    }
}

/*
 * Handler for rx packet
 */
static void coord_rx_handler(tdma_frame_t *frame)
{
    tdma_packet_t *pkt = (tdma_packet_t *) frame->pkt.data;
    //tdma_frame_print(frame);
    switch (tdma_packet_header_type(pkt))
    {
        case TDMA_PKT_DATA:
            event_post(EVENT_QUEUE_APPLI, (handler_t) tdma_data_rx_handler, frame);
            return;
        case TDMA_PKT_ASSOC:
            if (frame->pkt.length != TDMA_PKT_SIZE_HEADER + TDMA_PKT_SIZE_ASSOC)
            {
                log_error("Bad length packet for type %d", tdma_packet_header_type(pkt));
                break;
            }
            event_post(EVENT_QUEUE_APPLI, coord_assoc, frame);
            return;
        default:
            log_warning("Received frame of unexpected type %u", tdma_packet_header_type(pkt));
            break;
    }

    // Droping frame
    tdma_frame_free(frame);
}

/*
 * Handle association request
 */
static void coord_assoc(handler_arg_t arg)
{
    tdma_frame_t *frame = (tdma_frame_t *) arg;
    tdma_packet_t *pkt = (tdma_packet_t *) frame->pkt.data;
    uint16_t addr = pkt->header.src;
    uint8_t request = pkt->payload.assoc.slots;
    uint8_t delta_slot, i;
    unsigned id;

    tdma_get();
    tdma_frame_free(frame);

    if (slotsframe_is_static)
    {
        log_info("Ignoring associate request because slots-frame is static");
        tdma_release();
        return;
    }

    /* check address */
    if (addr == 0x0000 ||
            addr == 0xffff ||
            addr == tdma_data.addr)
    {
        tdma_release();
        log_error("Can't associate 0x%04x", addr);
        return;
    }

    /* check slots */
    if (!request)
    {
        tdma_release();
        log_error("Bad associate request with 0 slots");
        return;
    }

    /* beacon frame not idle, should bot happen */
    if (beacon_frame.status != TDMA_STATUS_IDLE)
    {
        tdma_release();
        log_error("Dropping assoc request due to beacon in progress");
        return;
    }

    /* estimate delat_slot to allocate slot 'not in group' */
    delta_slot = tdma_data.pan.slot_count / request;

    /* first count the current slot of this node and register the last one index */
    id = 0;
    for (i = 0; i < tdma_data.pan.slot_count; i++)
    {
        if (tdma_data.slots[i] == addr)
        {
            log_warning("0x%x has already slot %d", addr, i);
            id = i;
            if (!--request)
            {
                break;
            }
        }
    }

    while (request)
    {
        // compute next slot id base on last one
        uint8_t base = (id + delta_slot) % tdma_data.pan.slot_count;
        id = base;
        while (tdma_data.slots[id])
        {
            id = (id + 1) % tdma_data.pan.slot_count;
            if (id == base)
            {
                // no more slot available
                break;
            }
        }
        if (tdma_data.slots[id])
        {
            break;
        }
        tdma_slot_configure(id, addr);
        request -= 1;
    }

    if (request)
    {
        log_warning("slotframe is full, %u slots remains unallocated", request);
    }

    /* program beacon to be sent */
    beacon_prepare();

    tdma_release();
}
