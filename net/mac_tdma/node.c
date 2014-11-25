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
 * node.c
 *
 * \date Jan 09, 2013
 * \author: Damien Hedde <damien.hedde.at.hikob.com>
 */

#include "soft_timer.h"
#include "phy.h"
#include "random.h"

#include "tdma_internal.h"
#include "tdma_types.h"
#include "tdma_packet.h"
#include "tdma_config.h"

#define LOG_LEVEL LOG_LEVEL_INFO
#include "debug.h"


static soft_timer_t timeout_timer;

static tdma_frame_t node_frame;

static void node_rx_handler(tdma_frame_t *frame);
static void node_tx_handler(tdma_frame_t *frame);
static void node_beacon(handler_arg_t arg);
static void scan_start(handler_arg_t arg);
static void scan_handler(tdma_frame_t *frame);
static void node_timeout(handler_arg_t arg);
static void send_association_request ();

void mac_tdma_start_node(const mac_tdma_node_config_t *cfg)
{
    tdma_get();

    if (tdma_data.state != TDMA_IDLE)
    {
        log_error("TDMA is not IDLE");
        tdma_release();
        return;
    }

    /* register config */
    tdma_data.pan.coord = 0;
    tdma_data.pan.panid = cfg->panid;
    tdma_data.pan.channel = cfg->channel;
    tdma_data.bandwidth = cfg->bandwidth;

    tdma_data.beacon_frame = NULL;
    tdma_data.state = TDMA_SCAN;
    tdma_data.rx_handler = &node_rx_handler;
    tdma_data.tx_handler = &node_tx_handler;

    /* start beacons */
    soft_timer_set_handler(&timeout_timer, node_timeout, NULL);
    soft_timer_set_event_priority(&timeout_timer, EVENT_QUEUE_APPLI);

    /* start scan */
    event_post(EVENT_QUEUE_APPLI, scan_start, NULL);

    tdma_release();
}

/*
 * Timeout called when network is disconnected.
 */
static void node_timeout (handler_arg_t arg)
{
    // unused
    (void) arg;

    tdma_frame_t *f,*p;

    tdma_get();

    /* send association if needed */
    if (tdma_data.state == TDMA_WAIT)
    {
        send_association_request();
        tdma_release();
        return;
    }

    log_warning("Network disconnected");

    /* stop slots-frame */
    tdma_slot_stop();
    /* remove pending node_frame */
    f = tdma_data.tx_frames;
    p = NULL;
    while (f)
    {
        log_debug("unstacking %p->%p", p, f);
        if (f == &node_frame)
        {
            log_debug("Removing node_frame");
            f = f->next;
            if (p)
            {
                p->next = f;
            }
            else
            {
                tdma_data.tx_frames = f;
            }
            node_frame.next = NULL;
        }
        else
        {
            log_debug("Found frame %p", f);
            f->status = TDMA_STATUS_FAILED;
            p = f;
            f = f->next;
        }
    }

    /* handle remaining data frames */
    if (tdma_data.tx_frames)
    {
        event_post(EVENT_QUEUE_APPLI, (handler_t) tdma_data_tx_handler, tdma_data.tx_frames);
    }

    /* change state */
    tdma_data.tx_frames = NULL;
    tdma_data.pan.coord = 0;
    tdma_data.state = TDMA_SCAN;

    tdma_release();

    /* start a scan */
    event_post(EVENT_QUEUE_APPLI, scan_start, NULL);
}

/*
 * Start scanning
 */
static void scan_start(handler_arg_t arg)
{
    // unused
    (void) arg;

    tdma_get();

    log_info("Start to scan on channel %u", tdma_data.pan.channel);
    tdma_data.rx_handler = scan_handler;
    phy_prepare_packet(&node_frame.pkt);
    tdma_frame_prepare(&node_frame);
    slot_scan(&node_frame, tdma_data.pan.channel);

    tdma_release();
}

/*
 * Scan result handler
 */
static void scan_handler(tdma_frame_t *frame)
{
    if (frame->status != TDMA_STATUS_RECV)
    {
        log_error("Failed to scan");
    }
    else
    {
        //tdma_frame_print(frame);
        tdma_packet_t *pkt = (tdma_packet_t *) frame->pkt.data;
        tdma_packet_header_decode(pkt);
        if (tdma_packet_is_ok(pkt, tdma_data.pan.panid, 0xffff) && tdma_packet_header_type(pkt) == TDMA_PKT_BEACON
                && frame->pkt.length == TDMA_PKT_SIZE_HEADER + TDMA_PKT_SIZE_BEACON +
                    2 * pkt->payload.beacon.slot_desc_cnt)
        {
            /* compute start of frame time */
            uint32_t time = frame->pkt.timestamp;
            uint8_t i;

            /* start slots-frame */
            log_info("Scanned beacon from 0x%04x : %u*%uus",
                    pkt->header.src, pkt->payload.beacon.slot_count,
                    TDMA_SLOT_DURATION_FACTOR_US * pkt->payload.beacon.slot_duration);
            tdma_data.pan.coord = pkt->header.src;
            tdma_data.pan.slot_count = pkt->payload.beacon.slot_count;
            tdma_data.pan.slot_duration = pkt->payload.beacon.slot_duration;
            tdma_data.rx_handler = node_rx_handler;
            tdma_slot_start(time);

            /* add rx slot */
            tdma_slot_configure(0, tdma_data.pan.coord);

            /* add temporary tx slot to do association */
            for (i = 0; i < pkt->payload.beacon.slot_desc_cnt; i++)
            {
                if (pkt->payload.beacon.slot_desc_own[i] == 0xffff)
                {
                    tdma_slot_configure(pkt->payload.beacon.slot_desc_off + i, tdma_data.addr);
                    break;
                }
            }

            /* wait random slots-frames */
            time = ((soft_timer_time() ^ random_rand16()) % (1 + TDMA_ASSOC_BACKOFF_WINDOW));
            log_info("Wait %u slots-frame before associating", time);
            time *= soft_timer_us_to_ticks(TDMA_SLOT_DURATION_FACTOR_US);
            time *= (uint32_t) tdma_data.pan.slot_duration * tdma_data.pan.slot_count;
            soft_timer_start(&timeout_timer, time, 0);

            tdma_data.state = TDMA_WAIT;
            return;
        }
    }

    /* restart a scan */
    event_post(EVENT_QUEUE_APPLI, scan_start, NULL);
}

/*
 * Send an association request to the coordinator
 */
static void send_association_request ()
{
    uint32_t time = (TDMA_BEACON_BACKOFF_COUNT + 3) * tdma_data.pan.slot_duration * tdma_data.pan.slot_count;
    /* init frame */
    tdma_packet_t *pkt = (tdma_packet_t *) node_frame.pkt.data;
    tdma_frame_prepare(&node_frame);
    phy_prepare_packet(&node_frame.pkt);
    tdma_packet_header_prepare(pkt, TDMA_PKT_ASSOC, tdma_data.pan.panid, tdma_data.addr, tdma_data.pan.coord);

    /* compute number of slots to be requested */
    {
        unsigned n = tdma_data.pan.slot_duration; // n in 10e-4 sec
        n *= tdma_data.pan.slot_count;// n in 10e-4 sec/frame
        n *= tdma_data.bandwidth;// n in 10e-4 slot/frame
        n += (1000000 / TDMA_SLOT_DURATION_FACTOR_US) - 1;
        n /= (1000000 / TDMA_SLOT_DURATION_FACTOR_US); // n in slot/frame
        if (n > 255)
        {
            n = 255;
        }
        if (n == 0)
        {
            n = 1;
        }
        pkt->payload.assoc.slots = n;
    }

    node_frame.pkt.length = TDMA_PKT_SIZE_HEADER + TDMA_PKT_SIZE_ASSOC;

    log_debug("Sending association request to %04x for %u/%u slots",
            tdma_data.pan.coord, pkt->payload.assoc.slots, tdma_data.pan.slot_count);
    tdma_data.tx_frames = &node_frame;

    tdma_data.state = TDMA_ASSOC;
    soft_timer_start(&timeout_timer, soft_timer_us_to_ticks(TDMA_SLOT_DURATION_FACTOR_US) * time, 0);
}

/*
 * Tx packet handler
 */
static void node_tx_handler (tdma_frame_t *frame)
{
    if (frame == &node_frame)
    {
        ;
    }
    else
    {
        event_post(EVENT_QUEUE_APPLI, (handler_t) tdma_data_tx_handler, frame);
    }
}

/*
 * Rx packet handler
 */
static void node_rx_handler(tdma_frame_t *frame)
{
    tdma_packet_t *pkt = (tdma_packet_t *) frame->pkt.data;
    //tdma_frame_print(frame);
    switch (tdma_packet_header_type(pkt))
    {
        case TDMA_PKT_DATA:
            if (tdma_data.state == TDMA_NODE)
            {
                event_post(EVENT_QUEUE_APPLI, (handler_t) tdma_data_rx_handler, frame);
                return;
            }
            break;
        case TDMA_PKT_BEACON:
            if (frame->pkt.length != TDMA_PKT_SIZE_HEADER + TDMA_PKT_SIZE_BEACON
                    + 2 * pkt->payload.beacon.slot_desc_cnt)
            {
                log_error("Bad length packet for type %d", tdma_packet_header_type(pkt));
                break;
            }
            event_post(EVENT_QUEUE_APPLI, node_beacon, frame);
            return;
        default:
            log_warning("Received frame of unexpected type %u", tdma_packet_header_type(pkt));
            break;
    }

    tdma_frame_free(frame);
}

/*
 * Rx Beacon processing
 */
static void node_beacon(handler_arg_t arg)
{
    tdma_frame_t *frame = (tdma_frame_t *) arg;
    tdma_packet_t *pkt = (tdma_packet_t *) frame->pkt.data;
    uint8_t i;

    tdma_get();

    if (tdma_data.pan.coord == pkt->header.src)
    {
        log_debug("Received beacon");

        /* update time */
        tdma_slot_update_frame_start (frame->pkt.timestamp);

        /* unpack all beacon */
        for (i = 0; i < pkt->payload.beacon.slot_desc_cnt; i++)
        {
            pkt->payload.beacon.slot_desc_own[i] = packer_uint16_ntoh(pkt->payload.beacon.slot_desc_own[i]);
        }

        switch (tdma_data.state)
        {
            case TDMA_ASSOC:
                /* check for slot indication */
                for (i = 0; i < pkt->payload.beacon.slot_desc_cnt; i++)
                {
                    if (tdma_data.addr == pkt->payload.beacon.slot_desc_own[i])
                    {
                        // we have a slot => we are connected
                        tdma_data.state = TDMA_NODE;
                        break;
                    }
                }

                // stop here if not connected
                if (tdma_data.state != TDMA_NODE)
                {
                    break;
                }

            case TDMA_NODE:
                // update timeout
                soft_timer_start(&timeout_timer, 3 * TDMA_BEACON_PERIOD_S * SOFT_TIMER_FREQUENCY, 0);
                for (i = 0; i < pkt->payload.beacon.slot_desc_cnt; i++)
                {
                    uint16_t addr = pkt->payload.beacon.slot_desc_own[i];
                    // ignore brodcast slot since we have our own slot
                    if (addr == 0xffff)
                    {
                        addr = 0;
                    }
                    tdma_slot_configure(pkt->payload.beacon.slot_desc_off + i, addr);
                }
                break;

            default:
                break;
        }
    }

    tdma_frame_free(frame);

    tdma_release();
}

