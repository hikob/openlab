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

/*
 * mac_csma.c
 *
 *  Created on: Jan 20, 2013
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "FreeRTOS.h"
#include "semphr.h"

#include "mac_csma.h"
#include "packer.h"
#include "unique_id.h"
#include "random.h"

#include "debug.h"

/** Enter RX state */
static void csma_enter_rx(handler_arg_t arg);
/** Handle end of RX */
static void csma_rx_done(phy_status_t status);
static void csma_process_rx(handler_arg_t arg);
/** Handle end of TX */
static void csma_tx_done(phy_status_t status);

enum csma_state
{
    CSMA_STATE_RX,
    CSMA_STATE_TX
};

static struct
{
    xSemaphoreHandle mutex;

    phy_t phy;
    uint16_t local_addr;
    int channel;

    enum csma_state state;

    soft_timer_t reset_rx_timer;

    /** Packet for receiving */
    phy_packet_t rx_pkt;
    /** Packet for sending */
    phy_packet_t tx_pkt;
} mac;

static void take()
{
    xSemaphoreTake(mac.mutex, 1);
}
static void give()
{
    xSemaphoreGive(mac.mutex);
}

__attribute__((weak)) void serial_id_read(uint8_t *addr);

void mac_csma_init(int channel)
{
    if (mac.mutex == NULL)
    {
        mac.mutex = xSemaphoreCreateMutex();
    }

    // Store the PHY layer and channel
    mac.phy = mac_csma_config.phy;
    mac.channel = channel;

    // Get MAC address
    uint8_t ext_addr[8] =
    { 0, 0, 0, 0, 0, 0, 0, 0 };
    if (serial_id_read) serial_id_read(ext_addr);
    const uint8_t all_zeros[8] =
    { 0, 0, 0, 0, 0, 0, 0, 0 };

    // Check if all zero
    if (memcmp(ext_addr, all_zeros, 8) != 0)
    {
        packer_uint16_unpack(ext_addr + 6, &mac.local_addr);
    }
    else
    {
        int i;
        mac.local_addr = random_rand16();
        for (i = 0; i < 6; i++)
        {
            mac.local_addr ^= uid->uid16[i];
        }
    }
    // Print
    log_info("MAC CSMA address: %04x, channel %u", mac.local_addr, mac.channel);

    // Initialize the soft timer library
    soft_timer_init();

    // Reset the PHY
    phy_reset(mac.phy);

    // Prepare reset timer
    soft_timer_set_handler(&mac.reset_rx_timer, csma_enter_rx, NULL);

    // Enter RX
    event_post(EVENT_QUEUE_NETWORK, csma_enter_rx, NULL);
}

int mac_csma_data_send(uint16_t dest_addr, const uint8_t *data, uint8_t length)
{
    take();

    if (mac.state == CSMA_STATE_TX)
    {
        log_warning("Already sending, can't send");
        give();
        return 0;
    }

    // Prepare the packet
    phy_prepare_packet(&mac.tx_pkt);
    uint8_t *pkt_data = mac.tx_pkt.data;

    // Set our address, then destination address
    pkt_data = packer_uint16_pack(pkt_data, mac.local_addr);
    pkt_data = packer_uint16_pack(pkt_data, dest_addr);

    // Copy payload
    memcpy(pkt_data, data, length);
    mac.tx_pkt.length = 4 + length;

    // Set IDLE
    phy_idle(mac.phy);

    // Perform a CCA
    int32_t cca;
    phy_cca(mac.phy, &cca);

    if (cca)
    {
        // Channel is clear
        // Send
        phy_tx_now(mac.phy, &mac.tx_pkt, csma_tx_done);
        log_debug("Sending Packet");
    }
    else
    {
        log_warning("TX aborted, channel is busy");
        give();

        event_post(EVENT_QUEUE_APPLI, csma_enter_rx, NULL);
        return 0;
    }

    mac.state = CSMA_STATE_TX;
    give();
    return 1;
}

/** Enter RX state */
static void csma_enter_rx(handler_arg_t arg)
{
    take();

    phy_idle(mac.phy);
    phy_set_channel(mac.phy, mac.channel);

    // Prepare packet and start RX
    phy_prepare_packet(&mac.rx_pkt);
    phy_rx_now(mac.phy, &mac.rx_pkt, csma_rx_done);

    mac.state = CSMA_STATE_RX;

    // Re enter RX every 10 seconds
    soft_timer_start(&mac.reset_rx_timer, soft_timer_s_to_ticks(10), 0);

    give();
}

/** Handle end of RX */
static void csma_rx_done(phy_status_t status)
{
    if (status != PHY_SUCCESS)
    {
        // Enter RX again
        event_post(EVENT_QUEUE_APPLI, csma_enter_rx, NULL);
        return;
    }

    if (mac.rx_pkt.length < 4)
    {
        log_warning("Invalid length %u", mac.rx_pkt.length);

        // Enter RX again
        event_post(EVENT_QUEUE_APPLI, csma_enter_rx, NULL);
        return;
    }

    // Continue processing on appli queue
    event_post(EVENT_QUEUE_APPLI, csma_process_rx, NULL);
}

static void csma_process_rx(handler_arg_t arg)
{
    take();

    // Extract source and destination address
    uint16_t src_addr, dest_addr;
    packer_uint16_unpack(mac.rx_pkt.data, &src_addr);
    packer_uint16_unpack(mac.rx_pkt.data + 2, &dest_addr);

    const uint8_t *payload = mac.rx_pkt.data + 4;
    uint8_t length = mac.rx_pkt.length - 4;

    if (dest_addr == 0xFFFF || dest_addr == mac.local_addr)
    {
        mac_csma_data_indication(src_addr, payload, length);
    }
    else
    {
        log_warning("Got packet, not for me: dest %04x", dest_addr);
    }

    give();

    // Enter RX again
    csma_enter_rx(NULL);
}

/** Handle end of TX */
static void csma_tx_done(phy_status_t status)
{
    take();
    log_info("TX OK");
    give();

    csma_enter_rx(NULL);
}
