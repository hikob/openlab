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
 * flash_registers.h
 *
 *  Created on: Jul 8, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef FLASH_REGISTERS_H_
#define FLASH_REGISTERS_H_

#include "memmap.h"

static inline volatile uint32_t *flash_get_ACR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_ACR_OFFSET);
}
static inline volatile uint32_t *flash_get_PECR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_PECR_OFFSET);
}
static inline volatile uint32_t *flash_get_PDKEYR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_PDKEYR_OFFSET);
}
static inline volatile uint32_t *flash_get_PEKEYR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_PEKEYR_OFFSET);
}
static inline volatile uint32_t *flash_get_PRGKEYR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_PRGKEYR_OFFSET);
}
static inline volatile uint32_t *flash_get_OPTKEYR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_OPTKEYR_OFFSET);
}
static inline volatile uint32_t *flash_get_SR()
{
    return mem_get_reg32(FLASH_BASE_ADDRESS + FLASH_SR_OFFSET);
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
    FLASH_ACR__RUN_PD = 0x10,
    FLASH_ACR__SLEEP_PD = 0x08,
    FLASH_ACR__ACC64 = 0x04,
    FLASH_ACR__PRFTEN = 0x02,
    FLASH_ACR__LATENCY = 0x01,
};

enum
{
    FLASH_PECR__OBL_LAUNCH = 0x40000,
    FLASH_PECR__ERRIE = 0x20000,
    FLASH_PECR__EOPIE = 0x10000,

    FLASH_PECR__FPRG = 0x00400,
    FLASH_PECR__ERASE = 0x00200,
    FLASH_PECR__FTDW = 0x00100,

    FLASH_PECR__DATA = 0x00010,
    FLASH_PECR__PROG = 0x00008,
    FLASH_PECR__OPTLOCK = 0x00004,
    FLASH_PECR__PRGLOCK = 0x00002,
    FLASH_PECR__PELOCK = 0x00001,
};

enum
{
    FLASH_SR__OPTVERR = 0x0800,
    FLASH_SR__SIZERR = 0x0300,
    FLASH_SR__PGAERR = 0x0200,
    FLASH_SR__WRPERR = 0x0100,

    FLASH_SR__READY = 0x0008,
    FLASH_SR__ENDHV = 0x0004,
    FLASH_SR__EOP = 0x0002,
    FLASH_SR__BSY = 0x0001,
};

enum
{
    FLASH_OBR__nRST_STDBY = 0x00400000,
    FLASH_OBR__nRTS_STOP = 0x00200000,
    FLASH_OBR__IWDG_SW = 0x00100000,
    FLASH_OBR__BOR_LEV_MASK = 0x000F0000,
    FLASH_OBR__RDPRT_MASK = 0x000000FF,
};
#endif /* FLASH_REGISTERS_H_ */
