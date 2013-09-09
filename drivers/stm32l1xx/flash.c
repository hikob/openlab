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
 * flash.c
 *
 *  Created on: Jul 8, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include "flash.h"
#include "flash_ram.h"
#include "flash_registers.h"
#include "pwr.h"
#include "eeprom.h"

#include "platform.h"

#include "cm3_scb_registers.h"

enum
{
    EEPROM_ADDRESS = 0x08080000,

    PEKEY1 = 0x89ABCDEF,
    PEKEY2 = 0x02030405,
    PRGKEY1 = 0x8C9DAEBF,
    PRGKEY2 = 0x13141516,
};

/* Handy Functions */
static void unlock_program_memory();
static void lock_program_memory();

/* EEPROM API implementation */

uint8_t flash_eeprom_read_uint8(uint32_t offset)
{
    // Check offset
    if (offset >= FLASH_SIZE_EEPROM)
    {
        return 0;
    }

    // Read byte and return
    return *mem_get_reg8(EEPROM_ADDRESS + offset);
}
uint16_t flash_eeprom_read_uint16(uint32_t offset)
{
    // Check offset
    if (offset >= FLASH_SIZE_EEPROM)
    {
        return 0;
    }

    // Read half word and return
    return *mem_get_reg16(EEPROM_ADDRESS + offset);
}
uint32_t flash_eeprom_read_uint32(uint32_t offset)
{
    // Check offset
    if (offset >= FLASH_SIZE_EEPROM)
    {
        return 0;
    }

    // Read word and return
    return *mem_get_reg32(EEPROM_ADDRESS + offset);
}

void flash_eeprom_erase_uint32(uint32_t offset)
{
    // Check offset
    if (offset >= FLASH_SIZE_EEPROM)
    {
        return;
    }

    // Disable interrupts during write
    platform_enter_critical();

    // Unlock PECR
    // Write the keys to PEKEYR
    *flash_get_PEKEYR() = PEKEY1;
    *flash_get_PEKEYR() = PEKEY2;

    // Erase word
    *mem_get_reg32(EEPROM_ADDRESS + offset) = 0;

    // Wait end of operation
    while (*flash_get_SR() & FLASH_SR__BSY)
    {
    }

    // Lock PECR
    // Set PELOCK bit in PECR
    *flash_get_PECR() |= FLASH_PECR__PELOCK;

    // Enable interrupts after write
    platform_exit_critical();
}
void flash_eeprom_write_uint32(uint32_t offset, uint32_t word)
{
    // Check offset
    if (offset >= FLASH_SIZE_EEPROM)
    {
        return;
    }

    // Disable interrupts during write
    platform_enter_critical();

    // Unlock PECR
    // Write the keys to PEKEYR
    *flash_get_PEKEYR() = PEKEY1;
    *flash_get_PEKEYR() = PEKEY2;

    // Set the FTDW bit in PECR to force erase
    *flash_get_PECR() |= FLASH_PECR__FTDW;

    // Write the word
    *mem_get_reg32(EEPROM_ADDRESS + offset) = word;

    // Wait for end of operation
    while (*flash_get_SR() & FLASH_SR__BSY)
    {
    }

    // Lock PECR
    // Set PELOCK bit in PECR
    *flash_get_PECR() |= FLASH_PECR__PELOCK;

    // Enable interrupts after write
    platform_exit_critical();
}

/* FLASH API implementation */

flash_status_t flash_memory_erase_page(uint32_t address)
{
    // Check the address is a page border
    if (address % FLASH_SIZE_PAGE)
    {
        return FLASH_ERR_INVALID_ADDRESS;
    }

    // Disable interrupts during write
    platform_enter_critical();

    // Unlock program memory
    unlock_program_memory();

    // Set the ERASE bit in PECR
    *flash_get_PECR() |= FLASH_PECR__ERASE;

    // Set the PROG bit in PECR
    *flash_get_PECR() |= FLASH_PECR__PROG;

    // Wait for BSY to be cleared
    while (*flash_get_SR() & FLASH_SR__BSY)
    {
    }

    // Write 0x0 to start erasing
    *mem_get_reg32(address) = 0x0;

    // Wait for end of operation
    while (*flash_get_SR() & FLASH_SR__BSY)
    {
    }

    // Lock program memory
    lock_program_memory();

    // Enable interrupts after write
    platform_exit_critical();

    return FLASH_OK;
}
flash_status_t flash_memory_write_word(uint32_t address, uint32_t word)
{
    // Disable interrupts during write
    asm volatile("cpsid i\n");

    // Unlock program memory
    unlock_program_memory();

    // Write the word to the address
    *mem_get_reg32(address) = word;

    // Wait for end of operation
    while (*flash_get_SR() & FLASH_SR__BSY)
    {
    }

    // Lock program memory
    lock_program_memory();

    // Enable interrupts after write
    platform_exit_critical();

    return FLASH_OK;
}

flash_status_t flash_memory_write_half_pages(uint32_t address,
        const uint32_t *words, uint32_t words_number)
{
    // Check the address is a half page border
    if (address % (FLASH_SIZE_PAGE / 2))
    {
        return FLASH_ERR_INVALID_ADDRESS;
    }

    // Check if length has a correct words number 32 words per half page
    if (words_number % 32)
    {
        return FLASH_ERR_INVALID_LENGTH;
    }

    // Disable interrupts during write
    platform_enter_critical();

    // Unlock program memory
    unlock_program_memory();

    // Set the FPRG bit in PECR
    *flash_get_PECR() |= FLASH_PECR__FPRG;

    // Set the PROG bit in PECR
    *flash_get_PECR() |= FLASH_PECR__PROG;

    // Wait for BSY to be cleared
    while (*flash_get_SR() & FLASH_SR__BSY)
    {
    }

#define RAM_COPY_CODE_LENGTH ((uint32_t) flash_ram_copy_end - (uint32_t) flash_ram_copy)
    uint8_t ram_code[RAM_COPY_CODE_LENGTH + 3];

    void
    (*ram_func)(uint32_t * dst, const uint32_t * src, uint32_t length) =
        (void( *)(uint32_t * dst, const uint32_t * src,
                  uint32_t length))(((uint32_t) ram_code + 3) & ~0x3);

    uint32_t func_i, copy_i;
    func_i = ((uint32_t) ram_func) & ~0x3;
    copy_i = ((uint32_t) flash_ram_copy) & ~0x3;

    // Copy
    memcpy((void *) func_i, (void *) copy_i, RAM_COPY_CODE_LENGTH);

    // Set pointer
    ram_func
    = (void( *)(uint32_t * dst, const uint32_t * src, uint32_t length))(func_i
            | 0x1);

    // Call
    ram_func((uint32_t *) address, words, words_number);

    // Lock the program memory
    lock_program_memory();

    // Enable interrupts after write
    platform_exit_critical();

    return FLASH_OK;
}
void flash_memory_copy_upper_to_lower()
{
    // Disable interrupts during write
    platform_enter_critical();

    // Set high power mode
    pwr_main_mode(PWR_VRANGE_1);

    // Unlock program memory
    unlock_program_memory();

    // Clear PECR
    *flash_get_PECR() = 0;

    // Set the ERASE bit in PECR
    *flash_get_PECR() |= FLASH_PECR__ERASE;

    // Set the PROG bit in PECR
    *flash_get_PECR() |= FLASH_PECR__PROG;

    // Wait for BSY to be cleared
    while (*flash_get_SR() & FLASH_SR__BSY)
    {
    }

#define RAM_CODE_LENGTH ((uint32_t) flash_ram_copy_all_end - (uint32_t) flash_ram_copy_all)
    uint8_t ram_code[RAM_CODE_LENGTH + 3];

    void (*ram_func)() = (void( *)())(((uint32_t) ram_code + 3) & ~0x3);

    uint32_t func_i, copy_i;
    func_i = ((uint32_t) ram_func) & ~0x3;
    copy_i = ((uint32_t) flash_ram_copy_all) & ~0x3;

    // Copy
    memcpy((void *) func_i, (void *) copy_i, RAM_CODE_LENGTH);

    // Set pointer
    ram_func = (void( *)())(func_i | 0x1);

    // Call
    ram_func();

    // Enable interrupts after write
    platform_exit_critical();
}

void flash_set_1ws()
{
    // Set 1 wait state, 64bit access
    *flash_get_ACR() |= FLASH_ACR__ACC64;
    *flash_get_ACR() = FLASH_ACR__SLEEP_PD | FLASH_ACR__ACC64
                       | FLASH_ACR__PRFTEN | FLASH_ACR__LATENCY;
}

void flash_set_0ws()
{
    // Set 0 wait state, 32bit access
    *flash_get_ACR() |= FLASH_ACR__ACC64;
    *flash_get_ACR() = FLASH_ACR__SLEEP_PD | FLASH_ACR__ACC64;
    *flash_get_ACR() = FLASH_ACR__SLEEP_PD;
}
void flash_set_1ws_nosleep()
{
    // Set 1 wait state, 64bit access
    *flash_get_ACR() |= FLASH_ACR__ACC64;
    *flash_get_ACR() = FLASH_ACR__ACC64 | FLASH_ACR__PRFTEN
                       | FLASH_ACR__LATENCY;
}

void flash_set_0ws_nosleep()
{
    // Set 0 wait state, 32bit access
    *flash_get_ACR() |= FLASH_ACR__ACC64;
    *flash_get_ACR() = FLASH_ACR__ACC64;
    *flash_get_ACR() = 0;
}

static void unlock_program_memory()
{
    // Unlock PECR
    // Write the keys to PEKEYR
    *flash_get_PEKEYR() = PEKEY1;
    *flash_get_PEKEYR() = PEKEY2;

    // Write the keys to PRGKEYR
    *flash_get_PRGKEYR() = PRGKEY1;
    *flash_get_PRGKEYR() = PRGKEY2;
}
static void lock_program_memory()
{
    // Set the PRGLOCK bit in PECR
    *flash_get_PECR() |= FLASH_PECR__PRGLOCK;

    // Lock PECR
    // Set PELOCK bit in PECR
    *flash_get_PECR() |= FLASH_PECR__PELOCK;
}
