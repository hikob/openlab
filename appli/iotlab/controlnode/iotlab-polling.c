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
 * iotlab-polling.c
 *
 *  Created on: Aug 12, 2013
 *      Author: burindes
 */

#include "platform.h"
#include "debug.h"
#include "packer.h"
#include "soft_timer.h"

#include "iotlab-polling.h"
#include "iotlab-control.h"
#include "iotlab-serial.h"
#include "constants.h"

#include "fiteco_lib_gwt.h"

static iotlab_serial_handler_t handler_config_powerpoll;
static int32_t config_powerpoll(uint8_t cmd_type, packet_t *pkt);
static void polling_sample(handler_arg_t arg, float voltage, float current,
        float power, float shunt_voltage, uint32_t timestamp);

void iotlab_polling_start()
{
    // Stop sampling
    fiteco_lib_gwt_current_monitor_stop();

    // Set configure handler
    handler_config_powerpoll.cmd_type = CONFIG_POWERPOLL;
    handler_config_powerpoll.handler = config_powerpoll;
    iotlab_serial_register_handler(&handler_config_powerpoll);
}

static int32_t config_powerpoll(uint8_t cmd_type, packet_t *pkt)
{
    // Stop sampling
    fiteco_lib_gwt_current_monitor_stop();

    /*
     * Expected packet is:
     *      * monitor input [1B]
     *      * sampling period [1B]
     *      * average mode [1B]
     */

    if (pkt->length != 3)
    {
        log_warning("Bad packet length: %u", pkt->length);
        pkt->length = 0;
        return 0;
    }

    uint8_t input = pkt->data[0];
    uint8_t period = pkt->data[1];
    uint8_t average = pkt->data[2];

    if (input > FITECO_GWT_CURRENT_MONITOR__BATTERY)
    {
        log_warning("Invalid input mode %u", input);
        pkt->length = 0;
        return 0;
    }

    if (period > INA226_PERIOD_8244us)
    {
        log_warning("Invalid period %u", period);
        pkt->length = 0;
        return 0;
    }

    if (average > INA226_AVERAGE_1024)
    {
        log_warning("Invalid average %u", period);
        pkt->length = 0;
        return 0;
    }

    if (input != FITECO_GWT_CURRENT_MONITOR__OFF)
    {
        // Set parameters and start sampling
        fiteco_lib_gwt_current_monitor_configure(period, average);
        fiteco_lib_gwt_current_monitor_select(input, polling_sample, NULL);
    }
    // OK
    pkt->length = 0;
    return 1;
}
static void polling_sample(handler_arg_t arg, float voltage, float current,
        float power, float shunt_voltage, uint32_t timestamp)
{
    // Sample received pack and send!
    packet_t *pkt = packet_alloc(IOTLAB_SERIAL_PACKET_OFFSET);
    if (pkt == NULL)
    {
        log_error("Failed to allocate packet for polling data");
        return;
    }

    /**
     * Prepare packet as follows:
     *      * timestamp [4B]
     *      * voltage [4B]
     *      * current [4B]
     *      * power [4B]
     */
    uint8_t *data = pkt->data;
    data = packer_uint32_pack(data, iotlab_control_convert_time(timestamp));
    data = packer_float_pack(data, voltage);
    data = packer_float_pack(data, current);
    data = packer_float_pack(data, power);

    pkt->length = data - pkt->data;
    if (!iotlab_serial_send_frame(POWERPOLL_NOTIF, pkt))
    {
        log_error("Failed to send to serial");
        packet_free(pkt);
    }

}
