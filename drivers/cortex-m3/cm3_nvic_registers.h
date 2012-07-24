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
 * cm3_nvic_registers.h
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef CM3_NVIC_REGISTERS_H_
#define CM3_NVIC_REGISTERS_H_

#include "cm3_memmap.h"
#include "nvic.h"

static inline volatile uint8_t *cm3_nvic_get_PRIORITY(nvic_irq_line_t line)
{
    return mem_get_reg8(CM3_NVIC_BASE_ADDRESS + CM3_NVIC_PRIORITY_OFFSET + line);
}
static inline volatile uint32_t *cm3_nvic_get_SET_ENABLE(uint8_t word)
{
    return (mem_get_reg32(CM3_NVIC_BASE_ADDRESS + CM3_NVIC_SET_ENABLE_OFFSET)
            + word);
}
static inline volatile uint32_t *cm3_nvic_get_CLEAR_ENABLE(uint8_t word)
{
    return (mem_get_reg32(CM3_NVIC_BASE_ADDRESS + CM3_NVIC_CLEAR_ENABLE_OFFSET)
            + word);
}
static inline volatile uint32_t *cm3_nvic_get_SET_PENDING(uint8_t word)
{
    return (mem_get_reg32(CM3_NVIC_BASE_ADDRESS + CM3_NVIC_SET_PENDING_OFFSET)
            + word);
}
static inline volatile uint32_t *cm3_nvic_get_CLEAR_PENDING(uint8_t word)
{
    return (mem_get_reg32(CM3_NVIC_BASE_ADDRESS + CM3_NVIC_CLEAR_PENDING_OFFSET)
            + word);
}

static inline volatile uint32_t *cm3_nvic_get_SYSTICK_CTRL()
{
    return (mem_get_reg32(CM3_SYSTICK_BASE_ADDRESS + CM3_SYSTICK_CTRL_OFFSET));
}
static inline volatile uint32_t *cm3_nvic_get_SYSTICK_RELOAD_VALUE()
{
    return (mem_get_reg32(CM3_SYSTICK_BASE_ADDRESS
                          + CM3_SYSTICK_RELOAD_VALUE_OFFSET));
}
static inline volatile uint32_t *cm3_nvic_get_SYSTICK_CURRENT_VALUE()
{
    return (mem_get_reg32(CM3_SYSTICK_BASE_ADDRESS
                          + CM3_SYSTICK_CURRENT_VALUE_OFFSET));
}
static inline volatile uint32_t *cm3_nvic_get_SYSTICK_CALIB_VALUE()
{
    return (mem_get_reg32(CM3_SYSTICK_BASE_ADDRESS
                          + CM3_SYSTICK_CALIB_VALUE_OFFSET));
}

enum
{
    SYSTICK_CTRL__ENABLE = 0x1,
    SYSTICK_CTRL__TICKINT = 0x2,
    SYSTICK_CTRL__CLKSOURCE = 0x4,
    SYSTICK_CTRL__COUNTFLAG = 0x1000,
};

#endif /* CM3_NVIC_REGISTERS_H_ */
