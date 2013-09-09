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
 * eeprom_fake.c
 *
 *  Created on: Jul 5, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include <stddef.h>

#include "eeprom.h"
#include "eeprom/eeprom_.h"

static uint32_t get_size()
{
    return 4096;
}

static uint8_t read_uint8(uint32_t offset)
{
    return 0;
}
static uint16_t read_uint16(uint32_t offset)
{
    return 0;
}
static uint32_t read_uint32(uint32_t offset)
{
    return 0;
}

static void write_uint8(uint32_t offset, uint8_t value)
{
}
static void write_uint16(uint32_t offset, uint16_t value)
{
}
static void write_uint32(uint32_t offset, uint32_t value)
{
}

const eeprom_t eeprom_fake =
{
    .get_size = get_size,
    .read_uint8 = read_uint8,
    .read_uint16 = read_uint16,
    .read_uint32 = read_uint32,
    .write_uint8 = write_uint8,
    .write_uint16 = write_uint16,
    .write_uint32 = write_uint32
};

__attribute__((weak)) const eeprom_config_t eeprom_config =
{
        .internal = &eeprom_fake,
        .external = NULL
};
