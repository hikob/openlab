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
 * mac_tdma.h
 *
 * \date Jan 09, 2013
 * \author: Damien Hedde <damien.hedde.at.hikob.com>
 */

#ifndef MAC_TDMA_H_
#define MAC_TDMA_H_

#include <stdbool.h>

#include "packet.h"
#include "phy.h"

/**
 * Result type.
 */
enum tdma_result
{
    TDMA_OK,
    TDMA_FAILED,
};

/**
 * Handler type for packet receiving.
 */
typedef void (*mac_tdma_rx_handler_t)(packet_t *, uint16_t);

/**
 * Callback type for packet sending.
 */
typedef void (*mac_tdma_tx_callback_t)(void *cb_arg, enum tdma_result);

/**
 * Callback type for slot callback.
 */
typedef void (*mac_tdma_sl_callback_t)(uint8_t slot_id, uint32_t time);

/**
 * MAC configuration structure.
 */
typedef struct
{
    phy_t phy;
} mac_tdma_config_t;

/**
 * Coordinator configuration structure.
 */
typedef struct
{
    // Network id
    uint16_t panid;
    // Slot duration in 100us unit
    uint8_t slot_duration;
    // Total number of slot
    uint8_t slot_count;
    // Channel
    uint8_t channel;
    // Optional static slotsframe description
    // must be NULL or an array of slot_count size
    // [0] should be the coord addr
    // and it should contain 1 (and only 1) 0xffff slot
    const uint16_t *slotsframe;
} mac_tdma_coord_config_t;

/**
 * Node configuration structure.
 */
typedef struct
{
    // Network id
    uint16_t panid;
    // Output bandwidth in pkt/s
    uint8_t bandwidth;
    // Channel
    uint8_t channel;
} mac_tdma_node_config_t;

extern const mac_tdma_config_t mac_tdma_config;

/**
 * Initialize the MAC layer.
 *
 * If the given address is 0x0000 or 0xffff, an address is generated.
 *
 * \param addr the address of this device
 */
void mac_tdma_init(uint16_t addr);

/**
 * Start as node.
 *
 * \param channel the channel to use
 * \param panid the network panid
 */
void mac_tdma_start_node(const mac_tdma_node_config_t *cfg);

/**
 * Start as coordinator.
 *
 * \param cfg a pointer to the network configuration
 */
void mac_tdma_start_coord(const mac_tdma_coord_config_t *cfg);

/*
 * Set data packet callback.
 * The callback has to 'packet_free' the packet given as argument.
 *
 * \param handler the function called at each packet reception
 */
void mac_tdma_set_recv_handler(mac_tdma_rx_handler_t handler);

/*
 * Send a packet.
 * The callback is called only if this function succeed first.
 * The given packet is not 'packet_freed' by the TDMA mac,
 * it has to be done by caller.
 *
 * \param pkt a pointer to the packet to send
 * \param addr the destination node address
 * (0x0000 is an alias for the coordinator and 0xffff is for broadcast)
 * \param cb the function called when the sending is complete
 * \param cb_arg an argument that will be given to the the callback
 * \return TDMA_OK if the sending is valid
 */
enum tdma_result mac_tdma_send(packet_t *pkt, uint16_t addr, mac_tdma_tx_callback_t cb, void *cb_arg);

/*
 * Tell if the MAC is connected.
 */
bool mac_tdma_is_connected (void);

/**
 * Get the device address.
 */
uint16_t mac_tdma_get_address (void);

/**
 * Get the coordinator address when connected.
 */
uint16_t mac_tdma_get_coordinator (void);

/*
 * Register a slot callback.
 * It will be called in NETWORK TASK QUEUE.
 * It should used with care and should be short to avoid breaking the tdma timings.
 * the callback takes 2 parameters:
 *  + the slot id (starting from 0)
 *  + the time (soft_timer) at which it starts.
 */
void mac_tdma_set_slot_callback(mac_tdma_sl_callback_t cb);

#endif /* MAC_TDMA_H_ */
