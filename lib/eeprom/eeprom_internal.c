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
 * eeprom_internal.c
 *
 *  Created on: Jan 2, 2013
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "eeprom.h"
#include "packer.h"
#include "stm32l1xx/flash.h"

static uint32_t get_size()
{
    return FLASH_SIZE_EEPROM;
}

static uint8_t read_uint8(uint32_t offset)
{
    return flash_eeprom_read_uint8(offset);
}
static uint16_t read_uint16(uint32_t offset)
{
    return flash_eeprom_read_uint16(offset);
}
static uint32_t read_uint32(uint32_t offset)
{
    return flash_eeprom_read_uint32(offset);
}

static void write_uint8(uint32_t offset, uint8_t value)
{
    // Get the base offset of the matching word
    uint32_t base_offset = offset & ~3;
    uint32_t byte_offset = offset & 3;

    // Read the word
    uint32_t base_word = flash_eeprom_read_uint32(base_offset);

    // Replace byte in word and write back
    base_word &= ~(0xFF << (8 * byte_offset));
    base_word |= value << (8 * byte_offset);

    // Write back
    flash_eeprom_write_uint32(base_offset, base_word);
}
static void write_uint16(uint32_t offset, uint16_t value)
{
    // Get the base offset of the matching word
    uint32_t base_offset = offset & ~3;
    uint32_t byte_offset = offset & 3;

    if (byte_offset < 3)
    {
        // Read the word
        uint32_t base_word = flash_eeprom_read_uint32(base_offset);

        // Replace short in word and write back
        base_word &= ~(0xFFFF << (8 * byte_offset));
        base_word |= value << (8 * byte_offset);

        // Write back
        flash_eeprom_write_uint32(base_offset, base_word);
    }
    else
    {
        // 2 words need to be written

        // Do first word
        uint32_t base_word = flash_eeprom_read_uint32(base_offset);
        base_word &= ~(0xFF << 24);
        base_word |= (value & 0xFF) << 24;
        flash_eeprom_write_uint32(base_offset, base_word);

        // Do second word
        base_word = flash_eeprom_read_uint32(base_offset + 4);
        base_word &= ~0xFF;
        base_word |= (value >> 8) & 0xFF;
        flash_eeprom_write_uint32(base_offset + 4, base_word);
    }

}
static void write_uint32(uint32_t offset, uint32_t value)
{
    // Get the base offset of the matching word
    uint32_t base_offset = offset & ~3;
    uint32_t byte_offset = offset & 3;

    if (byte_offset == 0)
    {
        // Just write
        flash_eeprom_write_uint32(offset, value);
    }
    else
    {
        // Two words must be written

        // Do first word
        uint32_t base_word = flash_eeprom_read_uint32(base_offset);
        base_word &= ~(0xFFFFFFFF << (byte_offset * 8));
        base_word |= (value  << (byte_offset * 8));
        flash_eeprom_write_uint32(base_offset, base_word);

        // Do second word
        base_word = flash_eeprom_read_uint32(base_offset + 4);
        base_word &= ~(0xFFFFFFFF >> (8 * (4 - byte_offset)));
        base_word |= (value >> (8 * (4 - byte_offset)));
        flash_eeprom_write_uint32(base_offset + 4, base_word);
    }
}

const eeprom_t eeprom_internal =
{
    .get_size = get_size,
    .read_uint8 = read_uint8,
    .read_uint16 = read_uint16,
    .read_uint32 = read_uint32,
    .write_uint8 = write_uint8,
    .write_uint16 = write_uint16,
    .write_uint32 = write_uint32
};
