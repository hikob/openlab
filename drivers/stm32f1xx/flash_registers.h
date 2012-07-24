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
    FLASH_ACR__LATENCY_MASK = 0x07,
    FLASH_ACR__HLFCYA       = 0x08,
    FLASH_ACR__PRFTBE       = 0x10,
    FLASH_ACR__PRFTBS       = 0x20
};

enum
{
    FLASH_SR__BSY      = 0x01,
    FLASH_SR__PGERR    = 0x04,
    FLASH_SR__WRPRTERR = 0x10,
    FLASH_SR__EOP      = 0x20
};

enum
{
    FLASH_CR__PG     = 0x0001,
    FLASH_CR__PER    = 0x0002,
    FLASH_CR__MER    = 0x0004,
    FLASH_CR__OPTPG  = 0x0010,
    FLASH_CR__OPTER  = 0x0020,
    FLASH_CR__STRT   = 0x0040,
    FLASH_CR__LOCK   = 0x0080,
    FLASH_CR__OPTWRE = 0x0200,
    FLASH_CR__ERRIE  = 0x0400,
    FLASH_CR__EOPIE  = 0x1000
};

#endif /* FLASH_REGISTERS_H_ */
