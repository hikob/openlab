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
 * Copyright (C) 2011,2012 HiKoB.
 */

/**
 * \file phy.h
 *
 * \date Jul 11, 2011
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef PHY_H_
#define PHY_H_

/**
 * \addtogroup net
 * @{
 *
 * \defgroup phy Physical Layer
 * @{
 *
 * The PHY network layer provides a flexible API to control the underlying
 * radio chip driver in a packet oriented way. This allows the most advanced
 * features such as switching between the different operating modes (SLEEP, IDLE,
 * RX, TX), configuring the output power or the radio channel, sending and
 * receiving packets with precise frame timestamping.
 *
 * \note All time values are expressed in 32kHz ticks based on the \ref soft_timer time
 * values.
 *
 * \see \ref example_phy_tx_rx.c for an example of using this library.
 */

#include <stdint.h>

/**
 * Abstract type defining a PHY implementation.
 *
 * This is used as the first argument of any API method call, allowing a platform
 * to have several radio chip and independent PHY implementations.
 */
typedef void *phy_t;

/**
 * Enumeration of the possible return values. Most API methods return a phy_status_t
 * value to indicate the success or any error encountered during its execution.
 */
typedef enum
{
    /** Operation was successful */
    PHY_SUCCESS = 0x0,

    /** Requested operation was not permitted in current state */
    PHY_ERR_INVALID_STATE = 0x1,
    /** Packet to send is too big */
    PHY_ERR_INVALID_LENGTH = 0x2,
    /** RX was requested at a time already passed */
    PHY_ERR_TOO_LATE = 0x3,
    /** Internal error while communicating with the chip */
    PHY_ERR_INTERNAL = 0x4,

    /** Packet received had an invalid length */
    PHY_RX_LENGTH_ERROR = 0x11,
    /** Packet received had an invalid CRC */
    PHY_RX_CRC_ERROR = 0x12,
    /** No packet received before timeout */
    PHY_RX_TIMEOUT_ERROR = 0x13,
} phy_status_t;

/**
 * Enumeration of possible output TX power values.
 *
 * \note Not all output power values are available on all underlying radio chips,
 * therefore real configured output power will be approximated. See \ref phy_set_power
 * for details.
 */
typedef enum
{
    PHY_POWER_m30dBm,
    PHY_POWER_m29dBm,
    PHY_POWER_m28dBm,
    PHY_POWER_m27dBm,
    PHY_POWER_m26dBm,
    PHY_POWER_m25dBm,
    PHY_POWER_m24dBm,
    PHY_POWER_m23dBm,
    PHY_POWER_m22dBm,
    PHY_POWER_m21dBm,
    PHY_POWER_m20dBm,
    PHY_POWER_m19dBm,
    PHY_POWER_m18dBm,
    PHY_POWER_m17dBm,
    PHY_POWER_m16dBm,
    PHY_POWER_m15dBm,
    PHY_POWER_m14dBm,
    PHY_POWER_m13dBm,
    PHY_POWER_m12dBm,
    PHY_POWER_m11dBm,
    PHY_POWER_m10dBm,
    PHY_POWER_m9dBm,
    PHY_POWER_m8dBm,
    PHY_POWER_m7dBm,
    PHY_POWER_m6dBm,
    PHY_POWER_m5dBm,
    PHY_POWER_m4dBm,
    PHY_POWER_m3dBm,
    PHY_POWER_m2dBm,
    PHY_POWER_m1dBm,
    PHY_POWER_0dBm,
    PHY_POWER_0_7dBm,
    PHY_POWER_1dBm,
    PHY_POWER_1_3dBm,
    PHY_POWER_1_8dBm,
    PHY_POWER_2dBm,
    PHY_POWER_2_3dBm,
    PHY_POWER_2_8dBm,
    PHY_POWER_3dBm,
    PHY_POWER_4dBm,
    PHY_POWER_5dBm
} phy_power_t;

/**
 * List of available radio channels.
 *
 * \note Not all channels are available on all radio chip. 868Mhz chips provide only
 * channel #0, whereas 2.4GHz chips provide channels 11 to 26 included. See
 * \ref phy_set_channel for details.
 */
typedef enum
{
    /** Channel 0 only for 868MHz chips */
    PHY_MAP_CHANNEL_0 = (1 << 0),

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
    PHY_MAP_CHANNEL_26 = (1 << 26),

    /** Map of all 868MHz-chip channels (i.e. only channel 0) */
    PHY_MAP_CHANNEL_868_ALL = 0x00000001,

    /** Map of all 2.5GHz-chip channels (i.e. from 11 to 26) */
    PHY_MAP_CHANNEL_2400_ALL = 0x07FFF800
} phy_map_channel_t;

/**
 * Handy constants.
 */
enum
{
    PHY_MAX_TX_LENGTH = 125,
    PHY_MAX_RX_LENGTH = 127,

    PHY_868_MIN_CHANNEL = 0,
    PHY_868_MAX_CHANNEL = 0,

    PHY_2400_MIN_CHANNEL = 11,
    PHY_2400_MAX_CHANNEL = 26
};
/**
 * Structure defining a PHY packet.
 *
 * This contains storage for the real frame, plus additional fields providing
 * extra information about the received packet.
 */
typedef struct
{
    /** A pointer of the beginning of the data of interest in \ref raw_data */
    uint8_t *data;

    /** The length of the data of interest, starting at \ref data */
    uint8_t length;

    /** The buffer holding the maximum PHY packet size and LQI byte*/
    uint8_t raw_data[PHY_MAX_RX_LENGTH + 1];

    /** The RSSI reading of a received packet */
    int8_t rssi;

    /** The LQI reading of a received packet */
    uint8_t lqi;

    /** The timestamp of a packet, i.e. its SFD, for both TX and RX packets */
    uint32_t timestamp;

    /** The end of packet time, for both RX and TX */
    uint32_t eop_time;

    /** Internal time values used for scheduled RX, see \ref phy_rx */
    uint32_t t_rx_start, t_rx_end;
} phy_packet_t;

/**
 * Function pointer type used to notify the upper layer of the end of RX state.
 *
 * \param status the status of the RX.
 */
typedef void (*phy_handler_t)(phy_status_t status);

/**
 * Reset the PHY layer.
 *
 * This method reset the underlying radio chip, and all states to SLEEP mode.
 * It can be called from any state, ongoing RX or TX will be interrupted.
 *
 * \param phy the PHY
 */
void phy_reset(phy_t phy);

/**
 * Force the PHY layer in IDLE mode.
 *
 * This stops any ongoing RX or TX and put the underlying radio chip in IDLE mode.
 * It can be called from any state.
 *
 * \param phy the PHY
 */
void phy_idle(phy_t phy);

/**
 * Force the PHY layer in SLEEP mode.
 *
 * This method stops any ongoing RX or TX and put the underlying radio chip in SLEEP mode.
 * It can be called from any state.
 *
 * \param phy the PHY
 */
void phy_sleep(phy_t phy);

/**
 * Select the radio channel to use.
 *
 * Depending on the type of the underlying radio chip, the available radio channels
 * differ.
 *
 * \note This may only be called when in SLEEP or IDLE state.
 * \note If the requested channel is not available, it will be rounded to the
 * closest valid.
 *
 * \param phy the PHY
 * \param channel the radio channel to use, valid values are 0 for 868MHz chips
 * and 11-26 for 2.4GHz chips
 * \return the status of the operation, \ref PHY_SUCCESS on success,
 * or \ref PHY_ERR_INVALID_STATE if the radio was an invalid state
 */
phy_status_t phy_set_channel(phy_t phy, uint8_t channel);

/**
 * Set the TX power of the PHY.
 *
 * \note Not all values are possible an all radio chips, therefore the closest value
 * will be applied.
 *
 * \param phy the PHY
 * \param power the TX power value to use
 * \return the status of the operation, \ref PHY_SUCCESS on success,
 * or \ref PHY_ERR_INVALID_STATE if the radio was an invalid state
 */
phy_status_t phy_set_power(phy_t phy, phy_power_t power);

/**
 * Perform a CCA measurement.
 *
 * This method enters RX for the minimum required time to get a CCA information
 * and stores in a given pointer.
 *
 * \note The PHY must be in SLEEP or IDLE state to perform a CCA.
 *
 * \param phy the PHY
 * \param cca a pointer to get the CCA result, which will be set to 1 if channel
 * is free, and 0 if channel is busy
 * \return the status of the operation, \ref PHY_SUCCESS on success,
 * or \ref PHY_ERR_INVALID_STATE if the radio was an invalid state
 */
phy_status_t phy_cca(phy_t phy, int32_t *cca);

/**
 * Perform a Energy Detection measurement.
 *
 * This method enters RX for the minimum required time to get an ED measurement
 * and stores in a given pointer.
 *
 * \note The PHY must be in SLEEP or IDLE state to perform a ED measurement.
 *
 * \param phy the PHY
 * \param ed a pointer to get the ED result, which will be set to the energy
 *          value in dBm
 * \return the status of the operation, \ref PHY_SUCCESS on success,
 * or \ref PHY_ERR_INVALID_STATE if the radio was an invalid state
 */
phy_status_t phy_ed(phy_t phy, int32_t *ed);

/**
 * Set the PHY in RX state, at a given time.
 *
 * This methods set the radio chip in RX mode, listening for incoming packets,
 * with an optional time window for starting to listen and a stopping to listen
 * if no packet was received.
 *
 * The radio will go in RX mode at \ref rx_time if not zero, and stop searching for a packet
 * at \ref timeout_time if not zero. If a beginning of a packet is detected, the timeout is
 * removed.
 *
 * If \ref rx_time is zero, RX is entered right away. If \ref timout_time is zero,
 * RX will end only on reception of a packet.
 *
 * \note The PHY must be in SLEEP or IDLE state to enter RX.
 * \note At the end of RX, the PHY goes in IDLE state.
 * \note The handler is posted with the \ref event, using the
 *          \ref EVENT_QUEUE_NETWORK priority.
 * \note rx_time MUST be less than 1s in the future
 * \note timeout_time MUST be less than 1s in the future
 *
 * \param phy the PHY
 * \param rx_time the time at which to start receiving
 * \param timeout_time the time at which to stop receiving, if no packet
 * has been received
 * \param pkt a pointer to a \ref phy_packet_t structure to hold the received
 * packet (if any) and associated timing information
 * \param handler a handler function pointer to be called on end of RX, either
 * if a packet was received or timeout elapsed
 * \return the status of the operation, \ref PHY_SUCCESS on success,
 * or \ref PHY_ERR_INVALID_STATE if the radio was an invalid state
 */
phy_status_t phy_rx(phy_t phy, uint32_t rx_time, uint32_t timeout_time,
                    phy_packet_t *pkt, phy_handler_t handler);

/**
 * Set the PHY in RX mode, without timeout.
 *
 * Same as \ref phy_rx where RX starts as soon as possible without any timeout.
 *
 * \param phy the PHY;
 * \param pkt a pointer to a \ref phy_packet_t structure to hold the received
 * packet (if any) and associated timing information
 * \param handler a handler function pointer to be called on end of RX, either
 * if a packet was received or timeout elapsed
 * \return the status of the operation, \ref PHY_SUCCESS on success,
 * or \ref PHY_ERR_INVALID_STATE if the radio was an invalid state
 */
static inline phy_status_t phy_rx_now(phy_t phy, phy_packet_t *pkt,
                                      phy_handler_t handler)
{
    return phy_rx(phy, 0, 0, pkt, handler);
}

/**
 * Send a packet at a given time.
 *
 * This methods set the radio chip in TX mode to send a packet. The \ref tx_time
 * parameter, if not zero, is the time at which the transmission should start,
 * of right away if zero.
 *
 * \note The PHY must be in SLEEP or IDLE state to start sending.
 * \note At the end of TX, the PHY goes in IDLE state.
 * \note The handler is posted with the \ref event, using the
 *          \ref EVENT_QUEUE_NETWORK priority.
 *
 * \param phy the PHY;
 * \param tx_time the desired time of the start of TX
 * \param pkt a pointer to packet to send
 * \param handler the handler function pointer to call on TX end
 * \return the status of the operation, \ref PHY_SUCCESS on success,
 * or \ref PHY_ERR_INVALID_STATE if the radio was an invalid state
 */
phy_status_t phy_tx(phy_t phy, uint32_t tx_time, phy_packet_t *pkt,
                    phy_handler_t handler);

/**
 * Send a packet now.
 *
 * Same as \ref phy_tx but sends the packet as soon as possible instead of waiting
 * for a precise date.
 *
 * \param phy the PHY
 * \param pkt a pointer to packet to send
 * \param handler the function to call on TX end
 * \return the status of the operation, SUCCESS if RX started,
 * or ERR_INVALID_STATE if state was not IDLE or SLEEP.
 */
static inline phy_status_t phy_tx_now(phy_t phy, phy_packet_t *pkt,
                                      phy_handler_t handler)
{
    return phy_tx(phy, 0, pkt, handler);
}

/**
 * Prepare a PHY packet, should be used before filling its data field.
 *
 * \param pkt the packet to prepare
 */
static inline void phy_prepare_packet(phy_packet_t *pkt)
{
    pkt->data = pkt->raw_data;
}

/**
 * Get the closest parameter matching the given power in dBm
 *
 * \param power the desired power in dBm
 * \return the phy_power_t equivalent value, to be used with phy_set_power
 */
phy_power_t phy_convert_power(float power);

/**
 * Jam the environment.
 *
 * Start sending pseudo-random bytes on the configured channel with the configured
 * TX power. Stopped when called phy_idle.
 *
 * \param phy the PHY
 * \param channel the channel to jam
 * \param power the power to use
 * \return the status of the operation, SUCCESS if jamming is ongoing
 */
phy_status_t phy_jam(phy_t phy, uint8_t channel, phy_power_t power);

/**
 * @}
 * @}
 */

#endif /* PHY_H_ */
