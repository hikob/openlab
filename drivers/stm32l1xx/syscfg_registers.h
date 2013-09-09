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
 * syscfg_registers.h
 *
 *  Created on: Jul 12, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef SYSCFG_REGISTERS_H_
#define SYSCFG_REGISTERS_H_

#include "memmap.h"

static inline volatile uint32_t *syscfg_get_MEMRMP()
{
    return mem_get_reg32(SYSCFG_BASE_ADDRESS + SYSCFG_MEMRMP_OFFSET);
}
static inline volatile uint32_t *syscfg_get_PMC()
{
    return mem_get_reg32(SYSCFG_BASE_ADDRESS + SYSCFG_PMC_OFFSET);
}
static inline volatile uint32_t *syscfg_get_EXTICR1()
{
    return mem_get_reg32(SYSCFG_BASE_ADDRESS + SYSCFG_EXTICR1_OFFSET);
}
static inline volatile uint32_t *syscfg_get_EXTICR2()
{
    return mem_get_reg32(SYSCFG_BASE_ADDRESS + SYSCFG_EXTICR2_OFFSET);
}
static inline volatile uint32_t *syscfg_get_EXTICR3()
{
    return mem_get_reg32(SYSCFG_BASE_ADDRESS + SYSCFG_EXTICR3_OFFSET);
}
static inline volatile uint32_t *syscfg_get_EXTICR4()
{
    return mem_get_reg32(SYSCFG_BASE_ADDRESS + SYSCFG_EXTICR4_OFFSET);
}

enum
{
    NVIC_SYSTICK_CTRL__ENABLE = 0x1,
    NVIC_SYSTICK_CTRL__TICKINT = 0x2,
    NVIC_SYSTICK_CTRL__CLKSOURCE = 0x4,
    NVIC_SYSTICK_CTRL__COUNTFLAG = 0x1000,
};


#endif /* SYSCFG_REGISTERS_H_ */
