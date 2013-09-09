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
 * pwr_registers.h
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef PWR_REGISTERS_H_
#define PWR_REGISTERS_H_

#include "memmap.h"

static inline volatile uint16_t *pwr_get_CR()
{
    return mem_get_reg16(PWR_BASE_ADDRESS + PWR_CR_OFFSET);
}
static inline volatile uint16_t *pwr_get_CSR()
{
    return mem_get_reg16(PWR_BASE_ADDRESS + PWR_CSR_OFFSET);
}
enum
{
    PWR_CR__VOS = 0x8000,

    PWR_CR__FPDS = 0x0200,
    PWR_CR__DBP = 0x0100,
    PWR_CR__PLS_MASK = 0x00E0,
    PWR_CR__PVDE = 0x0010,
    PWR_CR__CSBF = 0x0008,
    PWR_CR__CWUF = 0x0004,
    PWR_CR__PDDS = 0x0002,
    PWR_CR__LPSDS = 0x0001,
};
enum
{
    PWR_CSR__VOSRDY= 0x4000,
    PWR_CSR__BRE = 0x0200,
    PWR_CSR__EWUP = 0x0100,
    PWR_CSR__BRR = 0x0008,
    PWR_CSR__PVDO = 0x0004,
    PWR_CSR__SBF = 0x0002,
    PWR_CSR__WUF = 0x0001,
};

#endif /* PWR_REGISTERS_H_ */
