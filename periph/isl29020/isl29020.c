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
 * isl29020.c
 *
 *  Created on: Dec 3, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "isl29020_.h"
#include "debug.h"

enum
{
    ISL29020_REG_COMMAND = 0,
    ISL29020_REG_DATA_LSB = 1,
    ISL29020_REG_DATA_MSB = 2,
};
enum
{
    ISL29020_COMMAND__EN = 0x80,
    ISL29020_COMMAND__MODE = 0x40,
    ISL29020_COMMAND__LIGHT = 0x20,
    ISL29020_COMMAND__RES_MASK = 0x1C,
    ISL29020_COMMAND__RES_SHIFT = 0x2,
    ISL29020_COMMAND__RANGE_MASK = 0x03,
    ISL29020_COMMAND__RANGE_SHIFT = 0,
};
static struct
{
    i2c_t i2c;
    uint8_t address;

    uint8_t command_reg;

    float range;
    int resolution;
} isl;

void isl29020_config(i2c_t i2c, uint32_t a0)
{
    isl.i2c = i2c;
    isl.address = 0x88 | (a0 != 0 ? 2 : 0);
}

void isl29020_prepare(isl29020_light_t light, isl29020_resolution_t res,
        isl29020_range_t range)
{
    // Prepare the command register
    isl.command_reg = (light ? ISL29020_COMMAND__LIGHT : 0)
            | ((res << ISL29020_COMMAND__RES_SHIFT)
                    & ISL29020_COMMAND__RES_MASK)
            | ((range << ISL29020_COMMAND__RANGE_SHIFT)
                    & ISL29020_COMMAND__RANGE_MASK);

    // Store range and resolution used for future computation
    switch (range)
    {
        case ISL29020_RANGE__1000lux:
            isl.range = 1000;
            break;
        case ISL29020_RANGE__4000lux:
            isl.range = 4000;
            break;
        case ISL29020_RANGE__16000lux:
            isl.range = 16000;
            break;
        case ISL29020_RANGE__64000lux:
            isl.range = 64000;
            break;
    }
    switch (res)
    {
        case ISL29020_RESOLUTION__16bit:
            isl.resolution = 16;
            break;
        case ISL29020_RESOLUTION__12bit:
            isl.resolution = 12;
            break;
        case ISL29020_RESOLUTION__8bit:
            isl.resolution = 8;
            break;
        case ISL29020_RESOLUTION__4bit:
            isl.resolution = 4;
            break;
    }
}

void isl29020_sample_once()
{
    // Send the command register with enable and one shot
    uint8_t buf[2] =
    { ISL29020_REG_COMMAND, ISL29020_COMMAND__EN | isl.command_reg };

    // Send it
    i2c_tx(isl.i2c, isl.address, buf, 2);

}
void isl29020_sample_continuous()
{
    // Send the command register with enable and one shot
    uint8_t buf[2] =
    { ISL29020_REG_COMMAND, ISL29020_COMMAND__EN | ISL29020_COMMAND__MODE
            | isl.command_reg };

    // Send it
    i2c_tx(isl.i2c, isl.address, buf, 2);
}
void isl29020_powerdown()
{
    // Prepare the command
    uint8_t buf[2] =
    { ISL29020_REG_COMMAND, isl.command_reg };

    // Send it
    i2c_tx(isl.i2c, isl.address, buf, 2);
}

float isl29020_read_sample()
{
    uint8_t tx_buf[1] =
    { ISL29020_REG_DATA_MSB };
    uint8_t rx_buf[1] =
    { 0 };

    float value = 0;

    // Read MSB
    i2c_tx_rx(isl.i2c, isl.address, tx_buf, 1, rx_buf, 1);

    value = rx_buf[0];
    value *= 256;

    // Read LSB
    tx_buf[0] = ISL29020_REG_DATA_LSB;
    i2c_tx_rx(isl.i2c, isl.address, tx_buf, 1, rx_buf, 1);
    value += rx_buf[0];

    // Convert to lux
    value = (isl.range * value) / (1 << isl.resolution);

    return value;
}
