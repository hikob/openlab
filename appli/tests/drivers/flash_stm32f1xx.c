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
 * flash_stm32f1xx.c
 *
 *  Created on: Feb 16, 2012
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include <stddef.h>
#include "platform.h"
#include "stm32f1xx/flash.h"
#include "printf.h"
#include "debug.h"

static void print_page(uint32_t addr);

int main()
{
    uint32_t page = 0x0807F800;
    uint32_t i;

    // Initialize the platform
    platform_init();

    log_info("Flash Write Test\n");

    log_info("Before: \n");
    print_page(page);

    log_info("Erasing:\n");
    flash_erase_memory_page(page);
    print_page(page);

    log_info("Writing word by word:\n");

    for (i = 0; i < FLASH_SIZE_PAGE; i += 2)
    {
        flash_write_memory_half_word(page + i, 0x3234);
    }

    print_page(page);

    log_info("Erasing:\n");
    flash_erase_memory_page(page);
    print_page(page);

    while (1)
    {
        __asm__("nop");
    }

    return 0;
}

static void print_page(uint32_t addr)
{
    uint8_t *p = (uint8_t *) addr;
    uint32_t i;

    for (i = 0; i < 256; i++)
    {
        printf("%02X ", *p);

        p++;

        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
    }
}
