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

static void coord_rx_handler(tdma_frame_t *frame);
static void coord_tx_handler(tdma_frame_t *frame);
static void beacon_tick(handler_arg_t arg);
static void coord_assoc(handler_arg_t arg);

void mac_tdma_start_coord(const mac_tdma_pan_config_t *cfg)
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
    memcpy(&tdma_data.pan, cfg, sizeof(tdma_data.pan));
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
    packet->payload.beacon.assoc_count = 0;
    beacon_frame.pkt.length = TDMA_PKT_SIZE_HEADER + TDMA_PKT_SIZE_BEACON;

    /* add broadcast tx/rx slot */
    tdma_slot_configure(0, TDMA_SLOT_TX, 0xffff);
    tdma_slot_configure(1, TDMA_SLOT_RX, 0xffff);
    tdma_slot_print();

    /* start beacons */
    beacon_time = soft_timer_time();
    soft_timer_set_handler(&beacon_timer, beacon_tick, NULL);
    event_post(EVENT_QUEUE_APPLI, beacon_tick, NULL);
    soft_timer_start(&beacon_timer, TDMA_BEACON_PERIOD_S * SOFT_TIMER_FREQUENCY, 1);

    /* change state */
    tdma_data.coord = tdma_data.addr;
    tdma_data.tx_frames = NULL;
    tdma_data.beacon_frame = NULL;
    tdma_data.state = TDMA_COORD;

    tdma_release();
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
    tdma_data.beacon_frame = &beacon_frame;

    tdma_release();
}

/*
 * Handler for tx packet
 */
static void coord_tx_handler(tdma_frame_t *frame)
{
    tdma_packet_t *pkt = (tdma_packet_t *) frame->pkt.data;
    switch (pkt->header.type)
    {
        case TDMA_PKT_BEACON:
            /* reset the beacon frame */
            phy_prepare_packet(&beacon_frame.pkt);
            tdma_frame_prepare(&beacon_frame);
            tdma_packet_header_prepare(pkt, TDMA_PKT_BEACON, tdma_data.pan.panid, tdma_data.addr, 0xffff);
            pkt->payload.beacon.slot_duration = tdma_data.pan.slot_duration;
            pkt->payload.beacon.slot_count = tdma_data.pan.slot_count;
            pkt->payload.beacon.assoc_count = 0;
            beacon_frame.pkt.length = TDMA_PKT_SIZE_HEADER + TDMA_PKT_SIZE_BEACON;
            break;
        case TDMA_PKT_DATA:
            event_post(EVENT_QUEUE_APPLI, (handler_t) tdma_data_tx_handler, frame);
            break;
        default:
            log_error("Unexpected tx frame type %d", pkt->header.type);
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
    switch (pkt->header.type)
    {
        case TDMA_PKT_DATA:
            event_post(EVENT_QUEUE_APPLI, (handler_t) tdma_data_rx_handler, frame);
            return;
            break;
        case TDMA_PKT_ASSOC_REQ:
            if (frame->pkt.length != TDMA_PKT_SIZE_HEADER)
            {
                log_error("Bad length packet for type %d", pkt->header.type);
                break;
            }
            event_post(EVENT_QUEUE_APPLI, coord_assoc, frame);
            return;
            break;
        default:
            log_warning("Received frame of unexpected type %u", pkt->header.type);
            tdma_frame_free(frame);
            break;
    }

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
    int id, eid;

    tdma_get();
    tdma_frame_free(frame);

    /* check address */
    if (addr == 0x0000 ||
            addr == 0xffff ||
            addr == tdma_data.addr)
    {
        tdma_release();
        log_error("Can't associate 0x%04x", addr);
        return;
    }

    /* beacon frame not idle, should bot happen */
    if (beacon_frame.status != TDMA_STATUS_IDLE)
    {
        tdma_release();
        log_error("Dropping assoc request due to beacon in progress");
        return;
    }

    /* search for existing or free slot */
    eid = -1;
    for (id = 0; id < tdma_data.pan.slot_count; id++)
    {
        if (id == tdma_data.tx_slot)
        {
            continue;
        }
        if (tdma_data.slots[id] == addr)
        {
            break;
        }
        if (tdma_data.slots[id] == 0)
        {
            eid = id;
        }
    }

    /* configure slot */
    if (id >= tdma_data.pan.slot_count)
    {
        id = eid;
        if (id < 0)
        {
            log_error("Can't associate 0x%x, slots-frame is full", addr);
            tdma_release();
            return;
        }
        tdma_slot_configure(id, TDMA_SLOT_RX, addr);
        log_info("Added slot %d for 0x%x", id, addr);
    }
    else
    {
        log_warning("0x%x has already slot %d", addr, id);
    }

    /* add assoc indication to next beacon */
    pkt = (tdma_packet_t *) beacon_frame.pkt.data;
    pkt->payload.beacon.assoc_ind[pkt->payload.beacon.assoc_count].addr = packer_uint16_hton(addr);
    pkt->payload.beacon.assoc_ind[pkt->payload.beacon.assoc_count].slot = id;
    pkt->payload.beacon.assoc_count += 1;
    beacon_frame.pkt.length += TDMA_PKT_SIZE_ASSOC_IND;

    /* program beacon to be sent */
    tdma_data.beacon_frame = &beacon_frame;

    tdma_release();
}
