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
 * Copyright (C) 2012 HiKoB.
 */

/*
 * iotlab-radio.c
 *
 *  Created on: Aug 12, 2013
 *      Author: burindes
 */

#include "platform.h"
#include "debug.h"

#include "iotlab-radio.h"
#include "iotlab-serial.h"
#include "iotlab-control.h"

#include "constants.h"

#include "phy.h"
#include "packer.h"
#include "soft_timer.h"

static iotlab_serial_handler_t handler_off;
static iotlab_serial_handler_t handler_sniffer;
static iotlab_serial_handler_t handler_polling;
static iotlab_serial_handler_t handler_injection;
static iotlab_serial_handler_t handler_jamming;

static int32_t radio_off(uint8_t cmd_type, packet_t *pkt);
static int32_t radio_sniffer(uint8_t cmd_type, packet_t *pkt);
static int32_t radio_polling(uint8_t cmd_type, packet_t *pkt);
static int32_t radio_injection(uint8_t cmd_type, packet_t *pkt);
static int32_t radio_jamming(uint8_t cmd_type, packet_t *pkt);

static struct
{
    soft_timer_t period_tim;
    uint32_t channels;
    uint32_t current_channel;

    struct
    {
        phy_packet_t pkt_buf[2];
        int pkt_index;
    } sniff;

    struct
    {
        packet_t *serial_pkt;
        uint32_t max_poll_per_pkt;
        uint32_t current_poll_in_pkt;
    } poll;

    struct
    {
        uint32_t num_pkts_per_channel;
        uint32_t current_pkts_on_channel;

        phy_packet_t pkt;
    } injection;

    struct
    {
        phy_power_t tx_power;
    } jam;

} radio;

void iotlab_radio_start()
{
    // Set the handlers
    handler_off.cmd_type = RADIO_OFF;
    handler_off.handler = radio_off;
    iotlab_serial_register_handler(&handler_off);

    handler_sniffer.cmd_type = RADIO_SNIFFER;
    handler_sniffer.handler = radio_sniffer;
    iotlab_serial_register_handler(&handler_sniffer);

    handler_polling.cmd_type = RADIO_POLLING;
    handler_polling.handler = radio_polling;
    iotlab_serial_register_handler(&handler_polling);

    handler_injection.cmd_type = RADIO_INJECTION;
    handler_injection.handler = radio_injection;
    iotlab_serial_register_handler(&handler_injection);

    handler_jamming.cmd_type = RADIO_JAMMING;
    handler_jamming.handler = radio_jamming;
    iotlab_serial_register_handler(&handler_jamming);
}

static void proper_stop()
{
    // Stop timer
    soft_timer_stop(&radio.period_tim);

    // Set PHY idle
    phy_idle(platform_phy);

    // Free polling packet
    if (radio.poll.serial_pkt)
    {
        packet_free(radio.poll.serial_pkt);
        radio.poll.serial_pkt = NULL;
    }
}

/* ********************** OFF **************************** */
static int32_t radio_off(uint8_t cmd_type, packet_t *pkt)
{
    // Stop all
    proper_stop();

    log_info("Radio OFF");

    // OK
    pkt->length = 0;
    return 1;
}
/* ********************** SNIFFER **************************** */

static void sniff_rx(phy_status_t status);
static void sniff_send_to_serial(handler_arg_t arg);
static void sniff_switch_channel(handler_arg_t arg);

static int32_t radio_sniffer(uint8_t cmd_type, packet_t *pkt)
{
    // Stop all
    proper_stop();

    /*
     * Expected packet format is (length:6B):
     *      * channels bitmap           [4B]
     *      * time per channel (1/100s) [2B]
     */
    if (pkt->length != 6)
    {
        log_warning("Bad Packet length: %u", pkt->length);
        pkt->length = 0;
        return 0;
    }

    /** GET values, system endian */
    uint16_t time_per_channel;

    const uint8_t *data = pkt->data;
    memcpy(&radio.channels, data, 4);
    data += 4;
    memcpy(&time_per_channel, data, 2);
    data += 2;

    // Check validity of channels
    if ((radio.channels & PHY_MAP_CHANNEL_2400_ALL) == 0)
    {
        log_warning("Bad Channels, none selected: %u", radio.channels);
        pkt->length = 0;
        return 0;
    }
    radio.channels &= PHY_MAP_CHANNEL_2400_ALL;

    log_info("Radio Sniffer channels %08x, period %u", radio.channels,
            time_per_channel);

    // Select first channel
    for (radio.current_channel = 0;
            (radio.channels & (1 << radio.current_channel)) == 0;
            radio.current_channel++)
    {
    }

    // Select first packet
    radio.sniff.pkt_index = 0;
    phy_prepare_packet(radio.sniff.pkt_buf + radio.sniff.pkt_index);

    // Start listening
    phy_set_channel(platform_phy, radio.current_channel);
    phy_status_t ret = phy_rx(platform_phy, 0,
            soft_timer_time() + soft_timer_ms_to_ticks(500),
            radio.sniff.pkt_buf + radio.sniff.pkt_index, sniff_rx);
    if (ret != PHY_SUCCESS)
    {
        log_error("PHY RX Failed");
    }
    log_debug("Sniff RX on channel %u", radio.current_channel);

    // Start channel switch timer
    if (time_per_channel != 0)
    {
        soft_timer_set_handler(&radio.period_tim, sniff_switch_channel, NULL);
        soft_timer_start(&radio.period_tim,
                soft_timer_ms_to_ticks(10 * time_per_channel), 1);
    }

    // OK
    pkt->length = 0;
    return 1;
}

static void sniff_rx(phy_status_t status)
{
    // Switch packets
    phy_packet_t *rx_packet = radio.sniff.pkt_buf + radio.sniff.pkt_index;
    radio.sniff.pkt_index = (radio.sniff.pkt_index + 1) % 2;

    // Enter RX again
    phy_status_t ret = phy_rx(platform_phy, 0,
            soft_timer_time() + soft_timer_ms_to_ticks(500),
            radio.sniff.pkt_buf + radio.sniff.pkt_index, sniff_rx);

    if (ret != PHY_SUCCESS)
    {
        log_error("PHY RX Failed");
    }

    if (status == PHY_SUCCESS)
    {
        // Send packet to serial
        packet_t *serial_pkt = packet_alloc(IOTLAB_SERIAL_PACKET_OFFSET);
        if (serial_pkt == NULL)
        {
            log_error("Failed to get a packet for sniffed RX");
            return;
        }

        uint8_t *data = serial_pkt->data;

        // Place timestamp, channel, RSSI, LQI, captured length, as header
        data = packer_uint32_pack(data,
                packer_uint32_hton(
                        iotlab_control_convert_time(rx_packet->timestamp)));
        *data++ = radio.current_channel;
        *data++ = rx_packet->rssi;
        *data++ = rx_packet->lqi;
        *data++ = rx_packet->length;
        memcpy(data, rx_packet->data, rx_packet->length);
        data += rx_packet->length;
        serial_pkt->length = data - serial_pkt->data;

        event_post(EVENT_QUEUE_APPLI, sniff_send_to_serial, serial_pkt);
    }
}

static void sniff_send_to_serial(handler_arg_t arg)
{
    packet_t *pkt = arg;
    if (!iotlab_serial_send_frame(RADIO_NOTIF_SNIFFED, pkt))
    {
        log_error("Failed to send captured frame");
        packet_free(pkt);
    }
}

static void sniff_switch_channel(handler_arg_t arg)
{
    phy_idle(platform_phy);

    // Select next channel
    do
    {
        radio.current_channel++;
        if (radio.current_channel > PHY_2400_MAX_CHANNEL)
        {
            radio.current_channel = PHY_2400_MIN_CHANNEL;
        }
    } while ((radio.channels & (1 << radio.current_channel)) == 0);

    // Enter RX on new channel
    phy_set_channel(platform_phy, radio.current_channel);
    phy_status_t ret = phy_rx(platform_phy, 0,
            soft_timer_time() + soft_timer_ms_to_ticks(500),
            radio.sniff.pkt_buf + radio.sniff.pkt_index, sniff_rx);
    if (ret != PHY_SUCCESS)
    {
        log_error("PHY RX Failed");
    }

    log_debug("Sniff RX on channel %u", radio.current_channel);

}
/* ********************** POLLING **************************** */
static void poll_time(handler_arg_t arg);

static int32_t radio_polling(uint8_t cmd_type, packet_t *pkt)
{
    // Stop all
    proper_stop();

    /*
     * Expected packet format is (length:3B):
     *      * channel                   [1B]
     *      * Sample period (1/1000s)   [2B]
     */

    if (pkt->length != 3)
    {
        log_warning("Bad Packet length: %u", pkt->length);
        pkt->length = 0;
        return 0;
    }

    const uint8_t *data = pkt->data;
    uint8_t channel = *data++;
    uint16_t sample_period;
    memcpy(&sample_period, data, 2);
    data += 2;

    if (channel < PHY_2400_MIN_CHANNEL || channel > PHY_2400_MAX_CHANNEL)
    {
        log_warning("Invalid channel: %u", channel);
        pkt->length = 0;
        return 0;
    }

    if (sample_period == 0)
    {
        log_warning("Invalid sample period: %u", sample_period);
        pkt->length = 0;
        return 0;
    }

    log_info("Radio Polling on channel %u, period %u", channel, sample_period);

    // Compute params
    radio.poll.current_poll_in_pkt = 0;
    radio.poll.max_poll_per_pkt = 1;
    if (sample_period < 50)
    {
        radio.poll.max_poll_per_pkt = 50 / sample_period;
    }

    // Set Channel
    phy_set_channel(platform_phy, channel);

    // Start Timer
    soft_timer_set_handler(&radio.period_tim, poll_time, NULL);
    soft_timer_start(&radio.period_tim, soft_timer_ms_to_ticks(sample_period),
            1);

    return 1;
}

static void poll_time(handler_arg_t arg)
{
    int32_t ed = 0;
    uint32_t timestamp = iotlab_control_convert_time(soft_timer_time());
    phy_ed(platform_phy, &ed);

    // Get packet if required
    if (radio.poll.serial_pkt == NULL)
    {
        radio.poll.serial_pkt = packet_alloc(IOTLAB_SERIAL_PACKET_OFFSET);
        if (radio.poll.serial_pkt == NULL)
        {
            log_error("Failed to get Packet");
            return;
        }
        memcpy(radio.poll.serial_pkt->data, &timestamp, 4);
        radio.poll.serial_pkt->length = 4;
    }

    // Append measurement
    radio.poll.serial_pkt->data[radio.poll.serial_pkt->length] = ed;
    radio.poll.serial_pkt->length++;

    // Increase count
    radio.poll.current_poll_in_pkt++;
    if (radio.poll.current_poll_in_pkt >= radio.poll.max_poll_per_pkt)
    {
        radio.poll.current_poll_in_pkt = 0;

        // Send
        if (iotlab_serial_send_frame(RADIO_NOTIF_POLLING, radio.poll.serial_pkt)
                != 1)
        {
            log_error("Failed to send serial log");
            packet_free(radio.poll.serial_pkt);
        }
        radio.poll.serial_pkt = NULL;
    }
}

/* ********************** INJECTION **************************** */
static void injection_time(handler_arg_t arg);
static void injection_tx_done(phy_status_t status);

static int32_t radio_injection(uint8_t cmd_type, packet_t *pkt)
{
    // Stop all
    proper_stop();

    /*
     * Expected packet format is (length:13B):
     *      * channels bitmap           [4B]
     *      * TX period (1/200s)        [2B]
     *      * num packets per channel   [2B]
     *      * TX power                  [4B]
     *      * packet size               [1B]
     */

    if (pkt->length != 13)
    {
        log_warning("Bad Packet length: %u", pkt->length);
        pkt->length = 0;
        return 0;
    }

    const uint8_t *data = pkt->data;
    memcpy(&radio.channels, data, 4);
    data += 4;
    uint16_t tx_period;
    memcpy(&tx_period, data, 2);
    data += 2;
    memcpy(&radio.injection.num_pkts_per_channel, data, 2);
    data += 2;
    float tx_power;
    memcpy(&tx_power, data, 4);
    data += 4;
    uint32_t pkt_size = *data++;

    if ((radio.channels & PHY_MAP_CHANNEL_2400_ALL) == 0)
    {
        log_warning("No channel selected %u", radio.channels);
        pkt->length = 0;
        return 0;
    }
    radio.channels &= PHY_MAP_CHANNEL_2400_ALL;

    if (pkt_size > 125)
    {
        log_warning("Bad injection length value: %u", pkt_size);
        pkt->length = 0;
        return 0;
    }

    if (tx_period == 0)
    {
        log_warning("Invalid injection TX period: %u", tx_period);
        pkt->length = 0;
        return 0;
    }

    log_info(
            "Radio Injection on channels %08x, period %u, %u pkt/ch, %fdBm, %ubytes",
            radio.channels, tx_period, radio.injection.num_pkts_per_channel,
            tx_power, pkt_size);

    // Prepare packet
    phy_prepare_packet(&radio.injection.pkt);
    radio.injection.pkt.length = pkt_size;
    int i;
    for (i = 0; i < pkt_size; i++)
    {
        // Set payload as ascii characters
        radio.injection.pkt.data[i] = (0x20 + i) % 95;
    }

    // Select first channel
    for (radio.current_channel = 0;
            (radio.channels & (1 << radio.current_channel)) == 0;
            radio.current_channel++)
    {
    }

    // Clear TX count
    radio.injection.current_pkts_on_channel = 0;

    // Wake PHY and configure
    phy_set_channel(platform_phy, radio.current_channel);
    phy_set_power(platform_phy, phy_convert_power(tx_power));

    // Start sending timer
    soft_timer_set_handler(&radio.period_tim, injection_time, NULL);
    soft_timer_start(&radio.period_tim, soft_timer_ms_to_ticks(5 * tx_period),
            1);

    // OK
    pkt->length = 0;
    return 1;
}

static void injection_time(handler_arg_t arg)
{
    if (phy_tx_now(platform_phy, &radio.injection.pkt, injection_tx_done) != PHY_SUCCESS)
    {
        log_error("Failed to send injection packet");
    }
}
static void injection_tx_done(phy_status_t status)
{
    // Increment packet count and check
    radio.injection.current_pkts_on_channel++;

    if (radio.injection.current_pkts_on_channel
            >= radio.injection.num_pkts_per_channel)
    {
        radio.injection.current_pkts_on_channel = 0;

        // Select next channel
        do
        {
            radio.current_channel++;
            if (radio.current_channel > PHY_2400_MAX_CHANNEL)
            {
                radio.current_channel = PHY_2400_MIN_CHANNEL;
            }
        } while ((radio.channels & (1 << radio.current_channel)) == 0);

        phy_set_channel(platform_phy, radio.current_channel);

        log_info("Injecting %u packets on channel %u",
                radio.injection.num_pkts_per_channel, radio.current_channel);
    }
}

/* ********************** JAMMING **************************** */
static void jam_change_channel_time(handler_arg_t arg);
static int32_t radio_jamming(uint8_t cmd_type, packet_t *pkt)
{
    // Stop all
    proper_stop();

    /*
     * Expected packet format is (length:10B):
     *      * channels bitmap           [4B]
     *      * channel period (1/200s)   [2B]
     *      * TX power                  [4B]
     */

    if (pkt->length != 10)
    {
        log_warning("Bad Packet length: %u", pkt->length);
        pkt->length = 0;
        return 0;
    }

    const uint8_t *data = pkt->data;
    memcpy(&radio.channels, data, 4);
    data += 4;
    uint16_t channel_period;
    memcpy(&channel_period, data, 2);
    data += 2;
    float tx_power;
    memcpy(&tx_power, data, 4);
    data += 4;

    if ((radio.channels & PHY_MAP_CHANNEL_2400_ALL) == 0)
    {
        log_warning("No channel selected %u", radio.channels);
        pkt->length = 0;
        return 0;
    }
    radio.channels &= PHY_MAP_CHANNEL_2400_ALL;

    log_info("Radio Jamming on channels %08x, change period %u, power %f",
            radio.channels, channel_period, tx_power);

    // Select first channel
    for (radio.current_channel = 0;
            (radio.channels & (1 << radio.current_channel)) == 0;
            radio.current_channel++)
    {
    }

    // Store power
    radio.jam.tx_power = phy_convert_power(tx_power);

    // Start jamming
    phy_jam(platform_phy, radio.current_channel, radio.jam.tx_power);

    if (channel_period)
    {
        soft_timer_set_handler(&radio.period_tim, jam_change_channel_time,
                NULL);
        soft_timer_start(&radio.period_tim,
                soft_timer_ms_to_ticks(5 * channel_period), 1);
    }

    return 1;
}

static void jam_change_channel_time(handler_arg_t arg)
{
    // Increment channel
    do
    {
        radio.current_channel++;
        if (radio.current_channel > PHY_2400_MAX_CHANNEL)
        {
            radio.current_channel = PHY_2400_MIN_CHANNEL;
        }
    } while ((radio.channels & (1 << radio.current_channel)) == 0);

    // Re-Jam
    phy_idle(platform_phy);
    phy_jam(platform_phy, radio.current_channel, radio.jam.tx_power);

    log_info("Jamming on channel %u", radio.current_channel);
}
