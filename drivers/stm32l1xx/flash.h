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
 * flash.h
 *
 *  Created on: Jul 8, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef FLASH_H_
#define FLASH_H_

enum
{
    FLASH_SIZE_PAGE = 256,
    FLASH_SIZE_SECTOR = 4096,
    FLASH_SIZE_EEPROM = 4096,

    FLASH_MEMORY_LOWER = 0x08000000,
    FLASH_MEMORY_UPPER = 0x08010000,
    FLASH_MEMORY_HALF_SIZE = 0x10000,
};

typedef enum
{
    FLASH_OK = 0, FLASH_ERR_INVALID_ADDRESS = 1, FLASH_ERR_INVALID_LENGTH = 2
} flash_status_t;

/* Flash Write */
flash_status_t flash_memory_erase_page(uint32_t address);
flash_status_t flash_memory_write_word(uint32_t address, uint32_t word);
flash_status_t flash_memory_write_half_pages(uint32_t address,
        const uint32_t *words, uint32_t word_number);

/* Full memory copy */
void flash_memory_copy_upper_to_lower();

/* Flash EEPROM Access */
uint8_t flash_eeprom_read_uint8(uint32_t offset);
uint16_t flash_eeprom_read_uint16(uint32_t offset);
uint32_t flash_eeprom_read_uint32(uint32_t offset);
void flash_eeprom_erase_uint32(uint32_t offset);
void flash_eeprom_write_uint32(uint32_t offset, uint32_t value);

/* Flash Access Modes */
void flash_set_0ws();
void flash_set_1ws();
void flash_set_0ws_nosleep();
void flash_set_1ws_nosleep();

/**
 * Set the flash access latency.
 *
 * Values are 0 or 1.
 * A latency of 0 may only be selected when the clock speed is 16MHz or lower.
 *
 *\param latency the latency
 */
void flash_set_latency(uint8_t latency);

#endif /* FLASH_H_ */
