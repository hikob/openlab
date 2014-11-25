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
 * data.c
 *
 * \date Jan 09, 2013
 * \author: Damien Hedde <damien.hedde.at.hikob.com>
 */

#include <string.h>

#include "tdma_types.h"
#include "tdma_internal.h"
#include "tdma_packet.h"

#include "event.h"
#include "packet.h"

#include "debug.h"

enum tdma_result mac_tdma_send(packet_t *pkt, uint16_t dst, mac_tdma_tx_callback_t cb, void *cb_arg)
{
    tdma_frame_t *frame;
    tdma_packet_t *tdma_pkt;

    if (!pkt)
    {
        log_error("Can't send NULL packet");
        return TDMA_FAILED;
    }

    if (pkt->length > TDMA_PKT_SIZE_PAYLOAD)
    {
        log_error("Packet is to big");
        return TDMA_FAILED;
    }

    /* get a frame (but ensure there is at least still 1 available */
    tdma_get();
    frame = tdma_frame_alloc(1);
    tdma_release();
    if (!frame)
    {
        log_error("Can't allocate a frame");
        return TDMA_FAILED;
    }

    /* frame info */
    phy_prepare_packet(&frame->pkt);
    frame->cb = cb;
    frame->arg = cb_arg;

    /* fill frame */
    if (dst == 0x0000)
    {
        dst = tdma_data.pan.coord;
    }
    tdma_pkt = (tdma_packet_t *) &frame->pkt.raw_data[0];
    tdma_packet_header_prepare(tdma_pkt, TDMA_PKT_DATA, tdma_data.pan.panid, tdma_data.addr, dst);
    memcpy(&tdma_pkt->payload.raw, pkt->data, pkt->length);
    frame->pkt.length = pkt->length + TDMA_PKT_SIZE_HEADER;

    /* programe frame sending */
    tdma_get();
    if (!mac_tdma_is_connected())
    {
        tdma_frame_free(frame);
        tdma_release();
        log_error("Can't send frame, network is disonnected");
        return TDMA_FAILED;
    }
    if (tdma_frame_send(frame))
    {
        tdma_frame_free(frame);
        tdma_release();
        log_error("Can't send frame");
        return TDMA_FAILED;
    }
    tdma_release();
    return TDMA_OK;
}

void tdma_data_tx_handler(tdma_frame_t *frame)
{
    tdma_frame_t *next = frame->next;
    enum tdma_result res;
    mac_tdma_tx_callback_t cb = frame->cb;
    void *arg = frame->arg;

    switch (frame->status)
    {
        case TDMA_STATUS_SENT:
            res = TDMA_OK;
            break;
        default:
            res = TDMA_FAILED;
            break;
    }

    /* free frame */
    tdma_get();
    tdma_frame_free(frame);
    tdma_release();

    /* eventually schedule another handler */
    if (next)
    {
        event_post(EVENT_QUEUE_APPLI, (handler_t) tdma_data_tx_handler, next);
    }

    /* call callback */
    if (cb)
    {
        cb(arg, res);
    }
    else
    {
        log_warning("Tx callback is NULL");
    }
}

void tdma_data_rx_handler(tdma_frame_t *frame)
{
    packet_t *pkt;
    tdma_packet_t *tdma_pkt;
    uint16_t addr;
    mac_tdma_rx_handler_t hdl;

    /* get a packet */
    if (!(pkt = packet_alloc(0)))
    {
        log_warning("Can't allocate packet: dropping");
        tdma_get();
        tdma_frame_free(frame);
        tdma_release();
        return;
    }

    /* check length */
    pkt->length = frame->pkt.length - TDMA_PKT_SIZE_HEADER;
    if (pkt->length > PACKET_MAX_SIZE)
    {
        log_error("Received payload is too big");
        packet_free(pkt);
        tdma_get();
        tdma_frame_free(frame);
        tdma_release();
        return;
    }

    /* copy payload */
    tdma_pkt = (tdma_packet_t *) &frame->pkt.raw_data[0];
    memcpy(pkt->data, &tdma_pkt->payload.raw, pkt->length);

    /* backup info */
    addr = tdma_pkt->header.src;

    /* release frame */
    tdma_get();
    tdma_frame_free(frame);
    tdma_release();

    /* call handler */
    if ((hdl = tdma_data.data_rx_handler))
    {
        hdl(pkt, addr);
    }
}
