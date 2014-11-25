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
 * slot.c
 *
 * \date Jan 09, 2013
 * \author: Damien Hedde <damien.hedde.at.hikob.com>
 */

#include <string.h>

#include "platform.h"
#include "soft_timer.h"
#include "softtimer/soft_timer_.h"

#include "tdma_internal.h"
#include "tdma_packet.h"
#include "tdma_types.h"
#include "tdma_config.h"

#define LOG_LEVEL LOG_LEVEL_INFO
#include "debug.h"

#if LOG_LEVEL == LOG_LEVEL_DEBUG
#warning "Using big slot wakeup"
#define TDMA_SLOT_WAKEUP_US 4000u
#endif

#define TIME_LT(x,y) ((int32_t) (x)) < ((int32_t) (y))

struct tdma_slotsframe
{
    uint32_t frame_duration;
    uint32_t frame_start;
    uint32_t slot_time;
    tdma_frame_t *frame;
    uint8_t next_index;
    uint8_t beacon_backoff;
    soft_timer_t timer;
};

static struct tdma_slotsframe sf_data;

static void handle_slot(handler_arg_t arg);
static void slot_tx(uint8_t, uint32_t time);
static void slot_tx_handler(phy_status_t status);
static void slot_rx(uint8_t slot, uint32_t time);
static void slot_rx_handler(phy_status_t status);
static void slot_scan_handler(phy_status_t status);

void tdma_slot_init ()
{
    soft_timer_config_priority(EVENT_QUEUE_NETWORK);
    soft_timer_set_handler(&sf_data.timer, handle_slot, NULL);
    soft_timer_set_event_priority(&sf_data.timer, EVENT_QUEUE_NETWORK);
}

int tdma_slot_start (uint32_t start)
{
    uint32_t time;
    uint8_t id;

    if (tdma_data.pan.slot_count > TDMA_MAX_SLOTS)
    {
        log_error("Slots frame is too big");
        return 1;
    }

    tdma_data.tx_slots = 0;
    for (id = 0; id < tdma_data.pan.slot_count; id++)
    {
        tdma_data.slots[id] = 0x0000;
    }

    sf_data.frame = NULL;
    sf_data.beacon_backoff = 0;
    sf_data.frame_duration = soft_timer_us_to_ticks(TDMA_SLOT_DURATION_FACTOR_US * tdma_data.pan.slot_duration * tdma_data.pan.slot_count);

    log_info("pan id: %04x", tdma_data.pan.panid);
    log_info("pan slots: %u*%u us", tdma_data.pan.slot_count, TDMA_SLOT_DURATION_FACTOR_US * tdma_data.pan.slot_duration);

    phy_reset(mac_tdma_config.phy);
    phy_set_channel(mac_tdma_config.phy, tdma_data.pan.channel);

    time = soft_timer_time() - start;
    time -= (time % sf_data.frame_duration);
    start += time;
    if (TIME_LT(start, soft_timer_time() + soft_timer_ms_to_ticks(TDMA_STARTUP_DELAY_MS)))
    {
        start += sf_data.frame_duration;
    }
    sf_data.frame_start = start;
    sf_data.next_index = 0;
    sf_data.slot_time = start;
    soft_timer_start_at(&sf_data.timer, start - soft_timer_us_to_ticks(TDMA_SLOT_WAKEUP_US + TDMA_SLOT_RX_MARGIN_US));
    phy_sleep(mac_tdma_config.phy);
    return 0;
}

void tdma_slot_stop ()
{
    // stop timer
    soft_timer_stop(&sf_data.timer);

    // get pending frame
    tdma_frame_t *frame = sf_data.frame;
    sf_data.frame = NULL;

    // stop phy
    phy_sleep(mac_tdma_config.phy);

    // handle pending frame
    if (frame)
    {
        if (frame->status == TDMA_STATUS_RX)
        {
            tdma_frame_free(frame);
        }
        else
        {
            tdma_data.tx_handler(frame);
        }
    }
}

void tdma_slot_print ()
{
    int i;
    log_printf("Slots-frame:\n");
    for (i = 0; i < tdma_data.pan.slot_count; i++)
    {
        __attribute__((__unused__)) char c = 'R';
        if (tdma_data.slots[i] == 0)
        {
            continue;
        }
        else if (tdma_data.slots[i] == tdma_data.addr)
        {
            c = 'T';
        }
        log_printf("Slot %d\t%c 0x%04x\n", i, c, tdma_data.slots[i]);
    }
}

void tdma_slot_configure(uint8_t id, uint16_t owner)
{
    uint16_t prev;
    if (id >= TDMA_MAX_SLOTS)
    {
        return;
    }

    prev = tdma_data.slots[id];
    if (owner != 0 && prev != 0 && prev != owner)
    {
        log_warning("Changing slot[%u] owner from %04x to %04x", id, prev, owner);
    }

    /*
     * TODO: protect the 'tx_slots' modif with a mutex ?
     * not necessary while slot_configure call is called
     * in the same event_queue.
     */
    if (owner == tdma_data.addr)
    {
        tdma_data.tx_slots += 1;
    }
    if (prev == tdma_data.addr)
    {
        tdma_data.tx_slots -= 1;
    }
    tdma_data.slots[id] = owner;
    if (prev != owner)
    {
        log_info("Set slot[%u] to %04x (TX:%u)", id, owner, tdma_data.tx_slots);
    }
}

/*
 * called at each slot
 */
static void handle_slot(handler_arg_t arg)
{
    // unused
    (void) arg;

    uint32_t time;
    uint8_t index;
    uint16_t owner;
    tdma_get();

    time = sf_data.slot_time;

    index = sf_data.next_index;

    /* update index */
    if (1 + index >= tdma_data.pan.slot_count)
    {
        sf_data.next_index = 0;
        sf_data.frame_start += sf_data.frame_duration;

        /* update backoff downcounter */
        if (sf_data.beacon_backoff)
        {
            sf_data.beacon_backoff -= 1;
        }
    }
    else
    {
        sf_data.next_index = index + 1;
    }

    /* setup new slot timeout */
    sf_data.slot_time = soft_timer_us_to_ticks(TDMA_SLOT_DURATION_FACTOR_US * tdma_data.pan.slot_duration * sf_data.next_index);
    sf_data.slot_time += sf_data.frame_start;
    soft_timer_start_at(&sf_data.timer, sf_data.slot_time - soft_timer_us_to_ticks(TDMA_SLOT_WAKEUP_US + TDMA_SLOT_RX_MARGIN_US));

    /* check end of previous slot */
    if (sf_data.frame)
    {
        log_error("Previous slot is not yet finished");
        tdma_release();
        return;
    }

    /* do the slot things */
    owner = tdma_data.slots[index];
    if (owner != 0x0000)
    {
        if (owner == tdma_data.addr)
        {
            slot_tx(index, time);
        }
        else
        {
            slot_rx(index, time);
        }
    }

    tdma_release();

    /*
     * Call callback
     */
    if (tdma_data.slot_cb)
    {
        tdma_data.slot_cb(index, time);
    }
}

/*
 * start a tx slot
 */
static void slot_tx(uint8_t slot, uint32_t slot_time)
{
    // unused
    (void) slot;

    uint32_t t;
    tdma_frame_t *frame;

    if ((!sf_data.beacon_backoff || !tdma_data.tx_frames)
            && (frame = tdma_data.beacon_frame))
    {
        /* have a beacon to send */
        tdma_data.beacon_frame = NULL;
        sf_data.beacon_backoff = TDMA_BEACON_BACKOFF_COUNT + 1;
    }
    else if ((frame = tdma_data.tx_frames))
    {
        /* have a data frame to send */
        tdma_data.tx_frames = frame->next;
    }
    else
    {
        /* nothing to send */
        return;
    }

    log_debug("%u:Tx at %u", slot, slot_time);

    /* get frame */
    sf_data.frame = frame;
    frame->next = NULL;

    /* compute time and send */
    t = slot_time;
    if (t == 0)
    {
        t = 1;
    }
    if (phy_tx(mac_tdma_config.phy, t, &frame->pkt, slot_tx_handler) != PHY_SUCCESS)
    {
        sf_data.frame = NULL;
        frame->status = TDMA_STATUS_FAILED;
        tdma_data.tx_handler(frame);
    }
}

/*
 * handler of tx slot end
 */
static void slot_tx_handler(phy_status_t status)
{
    tdma_frame_t *frame;

    tdma_get();

    frame = sf_data.frame;
    sf_data.frame = NULL;
    if (!frame)
    {
        /* may happen when stopping slots-frame */
        tdma_release();
        return;
    }

    phy_sleep(mac_tdma_config.phy);
    if (status == PHY_SUCCESS)
    {
        log_debug("Sent ok");
        frame->status = TDMA_STATUS_SENT;
    }
    else
    {
        frame->status = TDMA_STATUS_FAILED;
    }

    /* call handler */
    tdma_data.tx_handler(frame);

    tdma_release();
}

/*
 * start a rx slot
 */
static void slot_rx(uint8_t slot, uint32_t slot_time)
{
    // unused
    (void) slot;

    uint32_t t,tt;
    tdma_frame_t *frame;
    sf_data.frame = (frame = tdma_frame_alloc(0));

    if (!frame)
    {
        log_error("Can't allocate a frame for RX");
        return;
    }
    log_debug("%u:Rx at %u", slot, slot_time);
    phy_prepare_packet(&frame->pkt);

    frame->status = TDMA_STATUS_RX;

    t = slot_time - soft_timer_us_to_ticks(TDMA_SLOT_RX_MARGIN_US);
    if (t == 0)
    {
        t = 1;
    }
    tt = slot_time + soft_timer_us_to_ticks(TDMA_SLOT_RX_MARGIN_US);
    if (tt == 0)
    {
        tt = 1;
    }
    if (phy_rx(mac_tdma_config.phy, t, tt, &frame->pkt, slot_rx_handler) != PHY_SUCCESS)
    {
        sf_data.frame = NULL;
        tdma_frame_free(frame);
    }
}

/*
 * end a rx slot
 */
static void slot_rx_handler(phy_status_t status)
{
    tdma_frame_t *frame;

    tdma_get();

    frame = sf_data.frame;
    sf_data.frame = NULL;
    if (!frame)
    {
        /* may happen when stopping slots-frame */
        tdma_release();
        return;
    }

    phy_sleep(mac_tdma_config.phy);
    if (status == PHY_SUCCESS)
    {
        log_debug("RX");
        tdma_packet_t *pkt = (tdma_packet_t *) frame->pkt.data;
        frame->status = TDMA_STATUS_RECV;

        tdma_packet_header_decode(pkt);
        //tdma_frame_print(frame);
        if (tdma_packet_is_ok(pkt, tdma_data.pan.panid, tdma_data.addr))
        {
            tdma_data.rx_handler(frame);
            tdma_release();
            return;
        }
    }

    tdma_frame_free(frame);
    tdma_release();
}

void slot_scan(tdma_frame_t *frame, uint8_t channel)
{
    phy_prepare_packet(&frame->pkt);

    sf_data.frame = frame;
    frame->status = TDMA_STATUS_RX;

    log_debug("scan");
    phy_idle(mac_tdma_config.phy);
    phy_set_channel(mac_tdma_config.phy, channel);
    if (phy_rx_now(mac_tdma_config.phy, &frame->pkt, slot_scan_handler) != PHY_SUCCESS)
    {
        frame->status = TDMA_STATUS_FAILED;
        tdma_data.rx_handler(frame);
    }
}

/*
 * end a scan
 */
static void slot_scan_handler(phy_status_t status)
{
    tdma_frame_t *frame;

    tdma_get();

    /* get frame */
    frame = sf_data.frame;
    sf_data.frame = NULL;

    frame->status = TDMA_STATUS_FAILED;
    if (status == PHY_SUCCESS)
    {
        frame->status = TDMA_STATUS_RECV;

        log_debug("(%u)Scanned packet at %u", soft_timer_time(), frame->pkt.timestamp);
    }

    tdma_data.rx_handler(frame);

    tdma_release();
}

void tdma_slot_update_frame_start (uint32_t time)
{
    if (time != sf_data.frame_start)
    {
        log_info("Shifted frame of %d ticks", time - sf_data.frame_start);
    }
    sf_data.frame_start = time;
}
