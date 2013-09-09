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
 * flash_registers.h
 *
 *  Created on: Oct 25, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef FLASH_REGISTERS_H_
#define FLASH_REGISTERS_H_

#include <stdint.h>
#include "memmap.h"

inline static volatile uint32_t* flash_get_ACR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_ACR_OFFSET);
}
inline static volatile uint32_t* flash_get_KEYR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_KEYR_OFFSET);
}
inline static volatile uint32_t* flash_get_OPTKEYR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_OPTKEYR_OFFSET);
}
inline static volatile uint32_t* flash_get_SR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_SR_OFFSET);
}
inline static volatile uint32_t* flash_get_CR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_CR_OFFSET);
}
inline static volatile uint32_t* flash_get_OPTCR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_OPTCR_OFFSET);
}

enum
{
    FLASH_ACR__LATENCY_MASK = 0x0007,
    FLASH_ACR__PRFTEN = 0x0100,
    FLASH_ACR__ICEN = 0x0200,
    FLASH_ACR__DCEN = 0x0400,
    FLASH_ACR__ICRST = 0x0800,
    FLASH_ACR__DCRST = 0x1000,
};

#endif /* FLASH_REGISTERS_H_ */
