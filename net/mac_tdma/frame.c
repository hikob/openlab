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
 * frame.c
 *
 * \date Jan 09, 2013
 * \author: Damien Hedde <damien.hedde.at.hikob.com>
 */

#include "tdma_types.h"
#include "tdma_internal.h"
#include "tdma_packet.h"

#define LOG_LEVEL LOG_LEVEL_INFO
#include "debug.h"

static tdma_frame_t mac_tdma_frames[MAC_TDMA_MAX_FRAMES];
static tdma_frame_t *frames;
static unsigned nb_frames;

void tdma_frame_init ()
{
    int i;
    frames = NULL;
    for (i = 0; i < MAC_TDMA_MAX_FRAMES; i++)
    {
        mac_tdma_frames[i].next = frames;
        frames = &mac_tdma_frames[i];
    }
    nb_frames = MAC_TDMA_MAX_FRAMES;
}

tdma_frame_t * tdma_frame_alloc (unsigned rem)
{
    tdma_frame_t *res = frames;

    if (nb_frames <= rem)
    {
        /* not enough frames in list */
        return NULL;
    }

    if (!res)
    {
        log_error("Frame list shouldn't be empty");
        return NULL;
    }

    frames = res->next;
    nb_frames -= 1;
    tdma_frame_prepare(res);

    return res;
}

void tdma_frame_free (tdma_frame_t *frame)
{
    nb_frames += 1;
    frame->next = frames;
    frames = frame;
}

int tdma_frame_send (tdma_frame_t *frame)
{
    if (tdma_data.tx_slots == 0)
    {
        log_error("No slot for Tx");
        return 1;
    }

    /* add frame in queue */
    tdma_frame_t *f = tdma_data.tx_frames;
    if (f)
    {
        while (f->next)
        {
            f = f->next;
        }
        f->next = frame;
    }
    else
    {
        tdma_data.tx_frames = frame;
    }
    frame->next = NULL;
    return 0;
}

void tdma_frame_print(tdma_frame_t *frame)
{
    tdma_packet_t *pkt = (tdma_packet_t *) frame->pkt.data;
    __attribute__((__unused__)) uint32_t magic = packer_uint32_ntoh(pkt->header.magic);
    __attribute__((__unused__)) uint16_t panid = packer_uint16_ntoh(pkt->header.panid);
    __attribute__((__unused__)) uint16_t src = packer_uint16_ntoh(pkt->header.src);
    __attribute__((__unused__)) uint16_t dst = packer_uint16_ntoh(pkt->header.dst);
    log_printf("Packet L:%u M:%08x N:%04x S:%04x D:%04x V:%u T:%u\n",
            frame->pkt.length,
            magic, panid, src, dst,
            tdma_packet_header_version(pkt),
            tdma_packet_header_type(pkt));
    switch (tdma_packet_header_type(pkt))
    {
        int i;
        case TDMA_PKT_BEACON:
            log_printf("\t->beacon %u*%u\n", pkt->payload.beacon.slot_count, pkt->payload.beacon.slot_duration);
            for (i = 0; i < pkt->payload.beacon.slot_desc_cnt; i++)
            {
                log_printf("\t->slot %u for %04x\n", pkt->payload.beacon.slot_desc_off + i,
                        packer_uint16_ntoh(pkt->payload.beacon.slot_desc_own[i]));
            }
            break;
        case TDMA_PKT_ASSOC:
            log_printf("\t->assoc request for %u slots\n", pkt->payload.assoc.slots);
            break;
        default:
            break;
    }
}

