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
 * flash_ram.c
 *
 *  Created on: Jan 23, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include <stdint.h>
#include "flash.h"
#include "flash_ram.h"
#include "flash_registers.h"

void flash_ram_copy(uint32_t *dst, const uint32_t *src, uint32_t words_number)
{
    uint32_t buf[32], i;

    while (words_number)
    {
        // Copy to buffer
        for (i = 0; i < 32; i++)
        {
            buf[i] = src[i];
        }

        // Copy to memory
        for (i = 0; i < 32; i++)
        {
            dst[i] = buf[i];
        }

        // Update values
        dst += 32;
        src += 32;
        words_number -= 32;
    }
}
void flash_ram_copy_end()
{
    asm volatile("nop");
}

void flash_ram_copy_all()
{
    uint32_t *dst;
    uint32_t buffer[32];

    // Erase ALL
    for (dst = (uint32_t *) FLASH_MEMORY_LOWER; dst
            < (uint32_t *) FLASH_MEMORY_UPPER; dst += FLASH_SIZE_PAGE / 4)
    {
        *dst = 0;

        // Wait for BSY to be cleared
        while (*((volatile uint32_t *)(FLASH_BASE_ADDRESS + FLASH_SR_OFFSET))
                & FLASH_SR__BSY)
        {
        }

    }

    // Clear PECR
    *((volatile uint32_t *)(FLASH_BASE_ADDRESS + FLASH_PECR_OFFSET)) = 0;

    // Wait for BSY to be cleared
    while (*((volatile uint32_t *)(FLASH_BASE_ADDRESS + FLASH_SR_OFFSET))
            & FLASH_SR__BSY)
    {
    }

    // Set half page write mode
    *((volatile uint32_t *)(FLASH_BASE_ADDRESS + FLASH_PECR_OFFSET))
    |= FLASH_PECR__FPRG;
    *((volatile uint32_t *)(FLASH_BASE_ADDRESS + FLASH_PECR_OFFSET))
    |= FLASH_PECR__PROG;

    // Wait for BSY to be cleared
    while (*((volatile uint32_t *)(FLASH_BASE_ADDRESS + FLASH_SR_OFFSET))
            & FLASH_SR__BSY)
    {
    }

    // Offset indicating the current write, in bytes
    uint32_t offset = 0;

    while (offset < FLASH_MEMORY_HALF_SIZE)
    {
        // Copy a half page to the buffer
        uint32_t i;

        for (i = 0; i < FLASH_SIZE_PAGE / 8; i++)
        {
            buffer[i] = ((uint32_t *)(FLASH_MEMORY_UPPER + offset))[i];
        }

        // Copy the buffer to the memory
        for (i = 0; i < FLASH_SIZE_PAGE / 8; i++)
        {
            ((uint32_t *)(FLASH_MEMORY_LOWER + offset))[i] = buffer[i];
        }

        offset += FLASH_SIZE_PAGE / 2;
    }

    // Force reset
    *((volatile uint32_t *)(CM3_SCB_BASE_ADDRESS + CM3_SCB_AIRCR_OFFSET))
    = 0x05FA0004;
}

void flash_ram_copy_all_end()
{
    asm volatile("nop");
}
