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
 * Copyright (C) 2011 HiKoB.
 */

/*
 * flash.c
 *
 *  Created on: Jul 15, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include <stddef.h>

#include "flash.h"
#include "flash_registers.h"

void flash_set_wait_cycle(flash_wait_cycle_t cycles)
{
    uint32_t acr = *flash_get_ACR();

    // Enable prefetch buffer and set number of wait states
    acr &= ~(FLASH_ACR__LATENCY_MASK);
    acr |= FLASH_ACR__PRFTBE | cycles;

    *flash_get_ACR() = acr;

    // Wait for the prefetch buffer to be enabled
    while(!((*flash_get_ACR()) & FLASH_ACR__PRFTBS))
    {
        asm("nop");
    }
}
