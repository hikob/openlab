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
#define TDMA_VERSION 1

#define TDMA_PKTHDR_VT_TYPE_MASK 0x7
#define TDMA_PKTHDR_VT_VERSION_SHIFT 3

#define TDMA_PKT_SIZE_PAYLOAD 100
#define TDMA_PKT_SIZE_HEADER sizeof(struct tdma_pkt_header)
#define TDMA_PKT_SIZE_BEACON sizeof(struct tdma_pkt_beacon)
#define TDMA_PKT_SIZE_ASSOC sizeof(struct tdma_pkt_assoc)

typedef struct tdma_packet tdma_packet_t;

enum tdma_type
{
    TDMA_PKT_DATA = 0,
    TDMA_PKT_BEACON = 1,
    TDMA_PKT_ASSOC = 2,
};

struct tdma_pkt_header
{
    // magic (ie: "TDMA")
    uint32_t magic;
    // network id
    uint16_t panid;
    // src addr
    uint16_t src;
    // dst addr
    uint16_t dst;
    // protocol version & packet type
    // 3lsb -> type
    // 5msb -> version
    uint8_t  vt;
} __attribute__((__packed__));

struct tdma_pkt_assoc
{
    // number of slots requested
    uint8_t slots;
} __attribute__((__packed__));

struct tdma_pkt_beacon
{
    uint8_t slot_count;
    // slot length in defined unit (see tdma_config.h)
    uint8_t slot_duration;
    // number of following address in 'own' array
    uint8_t slot_desc_cnt;
    // starting slot offset for 'own' array
    uint8_t slot_desc_off;
    uint16_t slot_desc_own[];
} __attribute__((__packed__));

struct tdma_packet
{
    struct tdma_pkt_header header;
    union {
        uint8_t raw[TDMA_PKT_SIZE_PAYLOAD];
        struct tdma_pkt_assoc assoc;
        struct tdma_pkt_beacon beacon;
    } payload;
} __attribute__((__packed__));

__attribute__((__always_inline__))
static inline enum tdma_type tdma_packet_header_type(const tdma_packet_t *pkt)
{
    return pkt->header.vt & TDMA_PKTHDR_VT_TYPE_MASK;
}

__attribute__((__always_inline__))
static inline uint8_t tdma_packet_header_version(const tdma_packet_t *pkt)
{
    return pkt->header.vt >> TDMA_PKTHDR_VT_VERSION_SHIFT;
}

static inline void tdma_packet_header_prepare(tdma_packet_t *pkt,
        enum tdma_type type, uint16_t panid, uint16_t src, uint16_t dst)
{
    pkt->header.magic = packer_uint32_hton(TDMA_PKT_MAGIC);
    pkt->header.panid = packer_uint16_hton(panid);
    pkt->header.src = packer_uint16_hton(src);
    pkt->header.dst = packer_uint16_hton(dst);
    pkt->header.vt = (TDMA_VERSION << TDMA_PKTHDR_VT_VERSION_SHIFT) | type;
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
    if (tdma_packet_header_version(pkt) != TDMA_VERSION)
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
