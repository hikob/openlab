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
 * crc.c
 *
 *  Created on: Jan 17, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "crc.h"
#include "crc_registers.h"

#include "rcc.h"

void crc_enable()
{
    // Enable the CRC module in the RCC
    rcc_ahb_enable(RCC_AHB_BIT_CRC);
}

void crc_disable()
{
    // Disable the CRC module in the RCC
    rcc_ahb_disable(RCC_AHB_BIT_CRC);
}

void crc_reset()
{
    // Set the reset bit
    *crc_get_CR() = CRC_CR__RESET;
}

uint32_t crc_compute(const uint32_t *addr, uint32_t length)
{
    // Place all the words in the CRC modlue
    while(length--)
    {
        *crc_get_DR() = *addr++;
    }

    // Return its value
    return *crc_get_DR();
}
