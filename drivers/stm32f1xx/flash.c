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
 * flash.c
 *
 *  Created on: Jul 15, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include <stddef.h>

#include "rcc.h"
#include "flash.h"
#include "flash_registers.h"
#include "printf.h"

enum
{
    RDPRT = 0x00A5,
    KEY1  = 0x45670123,
    KEY2  = 0xCDEF89AB
};

/* Handy Functions */
static void unlock_program_memory();
static void lock_program_memory();

void flash_set_wait_cycle(flash_wait_cycle_t cycles)
{
    uint32_t acr = *flash_get_ACR();

    // Enable prefetch buffer and set number of wait states
    acr &= ~(FLASH_ACR__LATENCY_MASK);
    acr |= FLASH_ACR__PRFTBE | cycles;

    *flash_get_ACR() = acr;

    // Wait for the prefetch buffer to be enabled
    while (!((*flash_get_ACR()) & FLASH_ACR__PRFTBS))
    {
        asm("nop");
    }
}

flash_status_t flash_erase_memory_page(uint32_t address)
{
    bool hsi_enabled = rcc_is_hsi_enabled();

    // Check the address is a page border
    if (address % FLASH_SIZE_PAGE)
    {
        return FLASH_ERR_INVALID_ADDRESS;
    }

    // Unlock program memory
    unlock_program_memory();

    // Set the PER bit in FLASH_CR
    *flash_get_CR() = FLASH_CR__PER;

    // Set FLASH_AR to the selected page address
    *flash_get_AR() = address;

    // According to PM0075 HSI must be enable when erasing
    if (!hsi_enabled)
    {
        rcc_hsi_enable();
    }

    // Set the STRT bit in FLASH_CR
    *flash_get_CR() = FLASH_CR__PER | FLASH_CR__STRT;

    // Wait for BSY to be cleared
    while (*flash_get_SR() & FLASH_SR__BSY)
    {
    }

    // Disable HSI if it was not previously activated
    if (!hsi_enabled)
    {
        rcc_hsi_disable();
    }

    // Lock program memory
    lock_program_memory();

    return FLASH_OK;
}

flash_status_t flash_write_memory_half_word(uint32_t address, uint16_t half_word)
{
    bool hsi_enabled = rcc_is_hsi_enabled();

    // Unlock program memory
    unlock_program_memory();

    // Write FLASH_CR_PG to 1;
    *flash_get_CR() = FLASH_CR__PG;

    // According to PM0075 HSI must be enable when writing
    if (!hsi_enabled)
    {
        rcc_hsi_enable();
    }

    // Write the word to the address
    *mem_get_reg16(address) = half_word;

    // Wait for end of operation
    while (*flash_get_SR() & FLASH_SR__BSY)
    {
    }

    // Disable HSI if it was not previously activated
    if (!hsi_enabled)
    {
        rcc_hsi_disable();
    }

    // Lock program memory
    lock_program_memory();

    if (*mem_get_reg16(address) != half_word)
    {
        return FLASH_ERR_VALUE_NOT_COPIED;
    }

    return FLASH_OK;
}

void flash_copy_upper_to_lower()
{
    //! \todo implement copy from upper to lower flash area
}

static void unlock_program_memory()
{
    // Unlock FPEC
    // Write the keys to KEYR
    *flash_get_KEYR() = KEY1;
    *flash_get_KEYR() = KEY2;
}

static void lock_program_memory()
{
    // Lock FPEC
    // Set LOCK bit in FLASH_CR
    *flash_get_CR() |= FLASH_CR__LOCK;
}
