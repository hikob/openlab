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
 * flashtest.c
 *
 *  Created on: Jul 8, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include <stddef.h>
#include "platform.h"
#include "printf.h"

#include "stm32l1xx/flash.h"

static void print_page(uint32_t addr);

int main()
{
    // Initialize the platform
    platform_init();

    printf("Flash Write Test\n");

    uint32_t page = 0x0801f000;

    printf("Before: \n");
    print_page(page);

    printf("Erasing:\n");
    flash_memory_erase_page(page);
    print_page(page);

    printf("Writing word by word:\n");
    uint32_t i;

    for (i = 0; i < FLASH_SIZE_PAGE; i += 4)
    {
        flash_memory_write_word(page + i, 0x01234567);
    }

    print_page(page);

    printf("Erasing:\n");
    flash_memory_erase_page(page);
    print_page(page);

    printf("Writing half a page at once:\n");
    uint32_t my_half_page[64];
    uint8_t *p = (uint8_t *) my_half_page;

    for (i = 0; i < 256; i++)
    {
        p[i] = i;
    }

    flash_memory_write_half_pages(page, my_half_page, 64);
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
        if (*p < 0x10)
        {
            printf("0");
        }

        printf("%x ", *p);

        p++;

        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
    }

}
