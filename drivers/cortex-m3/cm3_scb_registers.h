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
 * cm3_scb_registers.h
 *
 *  Created on: Jul 22, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef CM3_SCB_REGISTERS_H_
#define CM3_SCB_REGISTERS_H_

#include "cm3_memmap.h"

static inline volatile uint32_t *cm3_scb_get_AIRCR()
{
    return mem_get_reg32(CM3_SCB_BASE_ADDRESS + CM3_SCB_AIRCR_OFFSET);
}
static inline volatile uint32_t *cm3_scb_get_SCR()
{
    return mem_get_reg32(CM3_SCB_BASE_ADDRESS + CM3_SCB_SCR_OFFSET);
}
static inline volatile uint32_t *cm3_scb_get_CCR()
{
    return mem_get_reg32(CM3_SCB_BASE_ADDRESS + CM3_SCB_CCR_OFFSET);
}
static inline volatile uint32_t *cm3_scb_get_SHCSR()
{
    return mem_get_reg32(CM3_SCB_BASE_ADDRESS + CM3_SCB_SHCSR_OFFSET);
}
static inline volatile uint32_t *cm3_scb_get_CFSR()
{
    return mem_get_reg32(CM3_SCB_BASE_ADDRESS + CM3_SCB_CFSR_OFFSET);
}
static inline volatile uint32_t *cm3_scb_get_HSFR()
{
    return mem_get_reg32(CM3_SCB_BASE_ADDRESS + CM3_SCB_HSFR_OFFSET);
}
static inline volatile uint32_t *cm3_scb_get_BFAR()
{
    return mem_get_reg32(CM3_SCB_BASE_ADDRESS + CM3_SCB_BFAR_OFFSET);
}
static inline volatile uint32_t *cm3_scb_get_MMFAR()
{
    return mem_get_reg32(CM3_SCB_BASE_ADDRESS + CM3_SCB_MMFAR_OFFSET);
}
static inline volatile uint32_t *cm3_scb_get_CPACR()
{
    return mem_get_reg32(CM3_SCB_BASE_ADDRESS + CM3_SCB_CPACR_OFFSET);
}


enum
{
    CM3_SCB_CFSR__IACCVIOL = 0x1,
    CM3_SCB_CFSR__DACCVIOL = 0x2,
    CM3_SCB_CFSR__MUNSTKERR = 0x8,
    CM3_SCB_CFSR__MSTKERR = 0x10,
    CM3_SCB_CFSR__MLSPERR = 0x20,
    CM3_SCB_CFSR__MMFARVALID = 0x80,

    CM3_SCB_CFSR__IBUSERR = 0x0100,
    CM3_SCB_CFSR__PRECISERR = 0x0200,
    CM3_SCB_CFSR__IMPRECISERR = 0x0400,
    CM3_SCB_CFSR__UNSTKERR = 0x0800,
    CM3_SCB_CFSR__STKERR = 0x1000,
    CM3_SCB_CFSR__LSPERR = 0x2000,
    CM3_SCB_CFSR__BFARVALID = 0x8000,

    CM3_SCB_CFSR__UNDEFINSTR = 0x00010000,
    CM3_SCB_CFSR__INVSTATE = 0x00020000,
    CM3_SCB_CFSR__INVPC = 0x00040000,
    CM3_SCB_CFSR__NOCP = 0x00080000,
    CM3_SCB_CFSR__UNALIGNED = 0x01000000,
    CM3_SCB_CFSR__DIVBYZERO = 0x02000000,
};

enum
{
    CM3_SCB_SCR__SEVONPEND = 0x0010,
    CM3_SCB_SCR__SLEEPDEEP = 0x0004,
    CM3_SCB_SCR__SLEEPONEXIT = 0x0002,
};

#endif /* CM3_SCB_REGISTERS_H_ */
