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
 * tdma_packet.h
 *
 * \date Jan 09, 2013
 * \author: Damien Hedde <damien.hedde.at.hikob.com>
 */

#ifndef TDMA_PACKET_H_
#define TDMA_PACKET_H_

#include "packer.h"

#define TDMA_PKT_MAGIC 0x54444D41

#define TDMA_PKT_SIZE_PAYLOAD 100
#define TDMA_PKT_SIZE_HEADER sizeof(struct tdma_pkt_header)
#define TDMA_PKT_SIZE_BEACON sizeof(struct tdma_pkt_beacon)
#define TDMA_PKT_SIZE_ASSOC_IND sizeof(struct tdma_pkt_assoc_ind)

typedef struct tdma_packet tdma_packet_t;

enum tdma_type
{
    TDMA_PKT_DATA = 0,
    TDMA_PKT_BEACON = 1,
    TDMA_PKT_ASSOC_REQ = 2,
};

struct tdma_pkt_header
{
    uint32_t magic;
    uint16_t panid;
    uint16_t src;
    uint16_t dst;
    uint8_t  type;
} __attribute__((__packed__));

struct tdma_pkt_assoc_ind
{
    uint16_t addr;
    uint8_t slot;
} __attribute__((__packed__));

struct tdma_pkt_beacon
{
    uint8_t slot_count;
    uint8_t slot_duration;
    uint8_t assoc_count;
    struct tdma_pkt_assoc_ind assoc_ind[];
} __attribute__((__packed__));

struct tdma_packet
{
    struct tdma_pkt_header header;
    union {
        uint8_t raw[TDMA_PKT_SIZE_PAYLOAD];
        struct tdma_pkt_beacon beacon;
    } payload;
} __attribute__((__packed__));

static inline void tdma_packet_header_prepare(tdma_packet_t *pkt,
        enum tdma_type type, uint16_t panid, uint16_t src, uint16_t dst)
{
    pkt->header.magic = packer_uint32_hton(TDMA_PKT_MAGIC);
    pkt->header.panid = packer_uint16_hton(panid);
    pkt->header.src = packer_uint16_hton(src);
    pkt->header.dst = packer_uint16_hton(dst);
    pkt->header.type = type;
}

static inline void tdma_packet_header_decode(tdma_packet_t *pkt)
{
    pkt->header.magic = packer_uint32_ntoh(pkt->header.magic);
    pkt->header.panid = packer_uint16_ntoh(pkt->header.panid);
    pkt->header.src = packer_uint16_ntoh(pkt->header.src);
    pkt->header.dst = packer_uint16_ntoh(pkt->header.dst);
}

static inline bool tdma_packet_is_ok(tdma_packet_t *pkt, uint16_t panid, uint16_t addr)
{
    if (pkt->header.magic != TDMA_PKT_MAGIC)
    {
        return false;
    }
    if (pkt->header.panid != panid)
    {
        return false;
    }
    if (pkt->header.dst != addr && pkt->header.dst != 0xffff)
    {
        return false;
    }
    return true;
}

__attribute__((__unused__))
static inline void tdma_frame_prepare (tdma_frame_t *frame)
{
    frame->next = NULL;
    frame->cb = NULL;
    frame->arg = NULL;
    frame->status = TDMA_STATUS_IDLE;
}

#endif
