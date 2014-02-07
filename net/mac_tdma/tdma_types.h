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
 * tdma_types.c
 *
 * \date Jan 09, 2013
 * \author: Damien Hedde <damien.hedde.at.hikob.com>
 */

#ifndef TDMA_TYPES_H_
#define TDMA_TYPES_H_

#include <stdint.h>

#include "phy.h"
#include "handler.h"

#include "mac_tdma.h"
#include "tdma_config.h"

typedef struct tdma_frame tdma_frame_t;

enum tdma_state
{
    TDMA_IDLE = 0x01,
    TDMA_SCAN = 0x02,
    TDMA_WAIT = 0x03,
    TDMA_ASSOC = 0x04,
    TDMA_NODE = 0x10,
    TDMA_COORD = 0x20,
};

enum tdma_slot_type
{
    TDMA_SLOT_EMPTY,
    TDMA_SLOT_RX,
    TDMA_SLOT_TX,
};

enum tdma_status
{
    TDMA_STATUS_IDLE,
    TDMA_STATUS_TX,
    TDMA_STATUS_RX,
    TDMA_STATUS_SENT,
    TDMA_STATUS_RECV,
    TDMA_STATUS_FAILED,
};

typedef void (*tdma_finalize_handler_t)(tdma_frame_t *, uint8_t slot_id);
typedef void (*tdma_frame_handler_t)(tdma_frame_t *);

struct tdma_frame
{
    enum tdma_status status;
    tdma_frame_t *next;
    mac_tdma_tx_callback_t cb;
    void *arg;
    phy_packet_t pkt;
};

struct tdma_global
{
    mac_tdma_pan_config_t pan;
    uint16_t addr;
    uint16_t coord;
    enum tdma_state state;
    tdma_frame_t *tx_frames;
    tdma_frame_t *beacon_frame;
    tdma_frame_handler_t rx_handler;
    tdma_frame_handler_t tx_handler;
    mac_tdma_rx_handler_t data_rx_handler;
    int16_t  tx_slot;
    uint16_t slots[TDMA_MAX_SLOTS];
};

#endif /* TDMA_TYPES_H_ */
