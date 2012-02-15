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
 * Copyright (C) 2011 HiKoB.
 */

/*
 * phy.h
 *
 *  Created on: Jul 11, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef PHY_H_
#define PHY_H_

#include <stdint.h>
#include "lib/net_timer.h"

typedef void *phy_t;

typedef enum
{
    PHY_SUCCESS = 0x0,

    PHY_ERR_INVALID_STATE = 0x1,
    PHY_ERR_INVALID_LENGTH = 0x2,
    PHY_ERR_ALREADY_TAKEN = 0x3,

    PHY_RX_ERROR = 0x10,
    PHY_RX_LENGTH_ERROR = 0x11,
    PHY_RX_CRC_ERROR = 0x12,
    PHY_RX_TIMEOUT_ERROR = 0x13,
} phy_status_t;

typedef enum
{
    PHY_POWER_m30dBm,
    PHY_POWER_m20dBm,
    PHY_POWER_m10dBm,
    PHY_POWER_m5dBm,
    PHY_POWER_0dBm,
    PHY_POWER_3dBm,
    PHY_POWER_5dBm
} phy_power_t;

typedef enum
{
    PHY_MAP_CHANNEL_11 = (1 << 11),
    PHY_MAP_CHANNEL_12 = (1 << 12),
    PHY_MAP_CHANNEL_13 = (1 << 13),
    PHY_MAP_CHANNEL_14 = (1 << 14),
    PHY_MAP_CHANNEL_15 = (1 << 15),
    PHY_MAP_CHANNEL_16 = (1 << 16),
    PHY_MAP_CHANNEL_17 = (1 << 17),
    PHY_MAP_CHANNEL_18 = (1 << 18),
    PHY_MAP_CHANNEL_19 = (1 << 19),
    PHY_MAP_CHANNEL_20 = (1 << 20),
    PHY_MAP_CHANNEL_21 = (1 << 21),
    PHY_MAP_CHANNEL_22 = (1 << 22),
    PHY_MAP_CHANNEL_23 = (1 << 23),
    PHY_MAP_CHANNEL_24 = (1 << 24),
    PHY_MAP_CHANNEL_25 = (1 << 25),

    PHY_MAP_CHANNEL_ALL = 0x3FFF800
} phy_map_channel_t;

enum
{
    PHY_MAX_TX_LENGTH = 125,
    PHY_MAX_RX_LENGTH = 127,
    PHY_MIN_CHANNEL = 11,
    PHY_MAX_CHANNEL = 26
};

typedef struct
{
    /** A pointer of the beginning of the data of interest in raw_data */
    uint8_t *data;

    /** The length of the data of interest, starting at data */
    uint8_t length;

    /** The buffer holding the maximum PHY packet size */
    uint8_t raw_data[PHY_MAX_RX_LENGTH];

    /** The RSSI reading of a received packet */
    int8_t rssi;

    /** The LQI reading of a received packet */
    uint8_t lqi;

    /** The timestamp of a packet, i.e. its SFD, for both TX and RX packets */
    uint32_t timestamp;

    /** The end of packet time, for both RX and TX */
    uint32_t eop_time;

    /** Internal times for begin and end of RX */
    uint32_t t_rx_start, t_rx_end;

} phy_packet_t;

typedef void (*phy_handler_t)(phy_status_t status);

/**
 * Reset the PHY layer, and the corresponding radio chip.
 * Set everything to sleep.
 *
 * \param phy the PHY to reset
 */
void phy_reset(phy_t phy);

/**
 * Force the PHY layer in IDLE mode.
 * \param phy the PHY
 */
void phy_idle(phy_t phy);

/**
 * Force the PHY layer in sleep mode.
 * \param phy the PHY
 */
void phy_sleep(phy_t phy);

/**
 * Select the radio channel to use for the next communications.
 *
 * \param phy the PHY.
 * \param channel the radio channel to use.
 * \return the status of the operation, SUCCESS if channel taken into account,
 * or ERR_INVALID_STATE if the radio was in TX or RX already.
 */
phy_status_t phy_set_channel(phy_t phy, uint8_t channel);

/**
 * Set the TX power of the PHY.
 *
 * \param phy the PHY
 * \param power the TX power value to use.
 * \return the status of the operation, SUCCESS if channel taken into account,
 * or ERR_INVALID_STATE if the radio was in TX or RX already.
 */
phy_status_t phy_set_power(phy_t phy, phy_power_t power);

/**
 * Perform a CCA measurement, and return the result.
 *
 * The chip must be in Idle STATE to do this.
 *
 * \param phy the PHY;
 * \return 1 if the channel is clear, 0 if not.
 */
uint8_t phy_cca(phy_t phy);

/**
 * Set the radio chip in RX mode, listening for incoming packets, with a given
 * window.
 *
 * The radio will go in RX mode at start_time, and stop searching for a packet
 * at timeout_time. If a beginning of a packet is detected, the timeout is
 * removed.
 *
 * \param phy the PHY;
 * \param rx_time the time at which to start receiving;
 * \param timeout_time the time at which to stop receiving, if no packet
 * has been received;
 * \param pkt a pointer to the packet to send;
 * \param handler the handler to be called on operation success or failure
 * \return the status of the operation, SUCCESS if RX started,
 * or ERR_INVALID_STATE if state was not IDLE or SLEEP.
 */
phy_status_t phy_rx(phy_t phy, uint32_t rx_time, uint32_t timeout_time,
                    phy_packet_t *pkt, phy_handler_t handler);

/**
 * Set the radio chip in RX mode, listening for incoming packets.
 *
 * After receiving a valid packet, the radio chip will go in IDLE mode.
 *
 * \param phy the PHY;
 * \param pkt a pointer to the packet to send;
 * \param handler the handler to be called on operation success or failure;
 * \return the status of the operation, SUCCESS if RX started,
 * or ERR_INVALID_STATE if state was not IDLE or SLEEP.
 */
static inline phy_status_t phy_rx_now(phy_t phy, phy_packet_t *pkt,
                                      phy_handler_t handler)
{
    return phy_rx(phy, 0, 0, pkt, handler);
}

/**
 * Send a frame at a given time.
 *
 * \param phy the PHY;
 * \param start_time the time at which to start the transmission;
 * \param pkt a pointer to packet to send;
 * \param handler the function to call on TX end;
 * \return the status of the operation, SUCCESS if RX started,
 * or ERR_INVALID_STATE if state was not IDLE or SLEEP.
 */
phy_status_t phy_tx(phy_t phy, uint32_t tx_time, phy_packet_t *pkt,
                    phy_handler_t handler);

/**
 * Send a frame as soon as possible.
 *
 * \param phy the PHY;
 * \param pkt a pointer to packet to send;
 * \param handler the function to call on TX end;
 * \return the status of the operation, SUCCESS if RX started,
 * or ERR_INVALID_STATE if state was not IDLE or SLEEP.
 */
static inline phy_status_t phy_tx_now(phy_t phy, phy_packet_t *pkt,
                                      phy_handler_t handler)
{
    return phy_tx(phy, 0, pkt, handler);
}

#endif /* PHY_H_ */
