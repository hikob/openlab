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
 *  Created on: Jul 15, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef FLASH_REGISTERS_H_
#define FLASH_REGISTERS_H_

#include "memmap.h"

static inline volatile uint32_t *flash_get_ACR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_ACR_OFFSET);
}

static inline volatile uint32_t *flash_get_KEYR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_KEYR_OFFSET);
}

static inline volatile uint32_t *flash_get_OPTKEYR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_OPTKEYR_OFFSET);
}

static inline volatile uint32_t *flash_get_SR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_SR_OFFSET);
}

static inline volatile uint32_t *flash_get_CR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_CR_OFFSET);
}

static inline volatile uint32_t *flash_get_AR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_AR_OFFSET);
}

static inline volatile uint32_t *flash_get_OBR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_OBR_OFFSET);
}

static inline volatile uint32_t *flash_get_WRPR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_WRPR_OFFSET);
}

enum
{
    FLASH_ACR__LATENCY_MASK = 0x7,
    FLASH_ACR__HLFCYA  = 0x8,
    FLASH_ACR__PRFTBE  = 0x10,
    FLASH_ACR__PRFTBS  = 0x20
};

#endif /* FLASH_REGISTERS_H_ */
