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
 * iotlab-control.c
 *
 *  Created on: Aug 12, 2013
 *      Author: burindes
 */

#include "platform.h"
#include "debug.h"
#include "packer.h"
#include "soft_timer.h"

#include "iotlab-control.h"
#include "iotlab-serial.h"
#include "constants.h"
#include "fiteco_lib_gwt.h"

static iotlab_serial_handler_t handler_start, handler_stop,
        handler_start_battery;
static iotlab_serial_handler_t handler_battery_charge,
        handler_battery_discharge;
static iotlab_serial_handler_t handler_set_time;

static int32_t start_stop(uint8_t cmd_type, packet_t *pkt);
static int32_t battery(uint8_t cmd_type, packet_t *pkt);
static int32_t set_time(uint8_t cmd_type, packet_t *pkt);

static struct
{
    int32_t time_offset;
} ctrl;

void iotlab_control_start()
{
    // Configure and register all handlers
    handler_start.cmd_type = OPENNODE_START;
    handler_start.handler = start_stop;
    iotlab_serial_register_handler(&handler_start);

    handler_start_battery.cmd_type = OPENNODE_STARTBATTERY;
    handler_start_battery.handler = start_stop;
    iotlab_serial_register_handler(&handler_start_battery);

    handler_stop.cmd_type = OPENNODE_STOP;
    handler_stop.handler = start_stop;
    iotlab_serial_register_handler(&handler_stop);

    handler_battery_charge.cmd_type = BATTERY_CHARGE;
    handler_battery_charge.handler = battery;
    iotlab_serial_register_handler(&handler_battery_charge);

    handler_battery_discharge.cmd_type = BATTERY_DISCHARGE;
    handler_battery_discharge.handler = battery;
    iotlab_serial_register_handler(&handler_battery_discharge);

    handler_set_time.cmd_type = SET_TIME;
    handler_set_time.handler = set_time;
    iotlab_serial_register_handler(&handler_set_time);

    // Initilize the time
    ctrl.time_offset = 0;
}

static int32_t start_stop(uint8_t cmd_type, packet_t *pkt)
{
    /*
     * Expected packet is empty
     */

    switch (cmd_type)
    {
        case OPENNODE_START:
            fiteco_lib_gwt_opennode_power_select(
                    FITECO_GWT_OPENNODE_POWER__MAIN);
            break;
        case OPENNODE_STARTBATTERY:
            fiteco_lib_gwt_opennode_power_select(
                    FITECO_GWT_OPENNODE_POWER__BATTERY);
            break;
        case OPENNODE_STOP:
            fiteco_lib_gwt_opennode_power_select(
                    FITECO_GWT_OPENNODE_POWER__OFF);
            break;
        default:
            // Unknown command
            pkt->length = 0;
            return 0;
    }

    // OK, no payload
    pkt->length = 0;
    return 1;
}

static int32_t battery(uint8_t cmd_type, packet_t *pkt)
{
    /*
     * Expected packet is empty
     */

    switch (cmd_type)
    {
        case BATTERY_CHARGE:
            fiteco_lib_gwt_battery_charge_enable();
            break;
        case BATTERY_DISCHARGE:
            fiteco_lib_gwt_battery_charge_disable();
            break;
        default:
            // Unknown command
            pkt->length = 0;
            return 0;
    }

    // OK, no payload
    pkt->length = 0;
    return 1;
}

static int32_t set_time(uint8_t cmd_type, packet_t *pkt)
{
    /*
     * Expected packet is:
     *      * time to set [4B]
     */
    if (pkt->length != 4)
    {
        log_warning("Bad packet length: %u", pkt->length);
        pkt->length = 0;
        return 1;
    }

    uint32_t new_time;
    packer_uint32_unpack(pkt->data, &new_time);

    // Store offset
    ctrl.time_offset = new_time - soft_timer_time();

    // OK, no payload
    pkt->length = 0;
    return 1;
}

uint32_t iotlab_control_convert_time(uint32_t timestamp)
{
    return timestamp + ctrl.time_offset;
}
