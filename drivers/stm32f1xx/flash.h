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
 * flash.h
 *
 *  Created on: Jul 15, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef FLASH_H_
#define FLASH_H_

#include <stdint.h>

enum
{
    FLASH_SIZE_PAGE = 2048,

    FLASH_MEMORY_LOWER = 0x08000000,
    FLASH_MEMORY_UPPER = 0x08040000,
    FLASH_MEMORY_HALF_SIZE = 0x40000,
};

typedef enum
{
    FLASH_0_CYCLE = 0,
    FLASH_1_CYCLE = 1,
    FLASH_2_CYCLE = 2
} flash_wait_cycle_t;

typedef enum
{
    FLASH_OK                   = 0,
    FLASH_ERR_INVALID_ADDRESS  = 1,
    FLASH_ERR_INVALID_LENGTH   = 2,
    FLASH_ERR_VALUE_NOT_COPIED = 3
} flash_status_t;

void flash_set_wait_cycle(flash_wait_cycle_t cycles);

/* Flash write */
flash_status_t flash_erase_memory_page(uint32_t address);
flash_status_t flash_write_memory_half_word(uint32_t address, uint16_t half_word);

/* Full memory copy */
void flash_copy_upper_to_lower();

#endif /* FLASH_H_ */
