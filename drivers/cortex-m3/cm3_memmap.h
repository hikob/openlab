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
 * cm3_memmap.h
 *
 *  Created on: Oct 7, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef CM3_MEMMAP_H_
#define CM3_MEMMAP_H_

#include <stdint.h>

/* Useful macros */
#ifndef BV
#define BV(x) (1<<(x))
#endif

static inline volatile uint32_t *mem_get_reg32(uint32_t addr)
{
    return (volatile uint32_t *) addr;
}
static inline volatile uint16_t *mem_get_reg16(uint32_t addr)
{
    return (volatile uint16_t *) addr;
}
static inline volatile uint8_t *mem_get_reg8(uint32_t addr)
{
    return (volatile uint8_t *) addr;
}

/* CortexM3 SYSTICK section */
#define CM3_SYSTICK_BASE_ADDRESS            0xE000E010

// Offsets for SysTick
#define CM3_SYSTICK_CTRL_OFFSET         0x00
#define CM3_SYSTICK_RELOAD_VALUE_OFFSET 0x04
#define CM3_SYSTICK_CURRENT_VALUE_OFFSET    0x08
#define CM3_SYSTICK_CALIB_VALUE_OFFSET      0x0C

/* CortexM3 NVIC section */
#define CM3_NVIC_BASE_ADDRESS               0xE000E100

// Offsets for NVIC
#define CM3_NVIC_SET_ENABLE_OFFSET          0x000
#define CM3_NVIC_CLEAR_ENABLE_OFFSET        0x080
#define CM3_NVIC_SET_PENDING_OFFSET         0x100
#define CM3_NVIC_CLEAR_PENDING_OFFSET       0x180
#define CM3_NVIC_ACTIVE_BIT_OFFSET          0x200
#define CM3_NVIC_PRIORITY_OFFSET            0x300

/* CortexM3 SCB section */
#define CM3_SCB_BASE_ADDRESS                0xE000ED00

// Offsets
#define CM3_SCB_CPUID_OFFSET        0x0
#define CM3_SCB_ICSR_OFFSET         0x4
#define CM3_SCB_VTOR_OFFSET         0x8
#define CM3_SCB_AIRCR_OFFSET        0xC
#define CM3_SCB_SCR_OFFSET          0x10
#define CM3_SCB_CCR_OFFSET          0x14
#define CM3_SCB_SHPR_4_7_OFFSET     0x18
#define CM3_SCB_SHPR_8_11_OFFSET    0x1C
#define CM3_SCB_SHPR_12_15_OFFSET   0x20

#define CM3_SCB_SHCSR_OFFSET        0x24
#define CM3_SCB_CFSR_OFFSET         0x28
#define CM3_SCB_HSFR_OFFSET         0x2C
#define CM3_SCB_DFSR_OFFSET         0x30
#define CM3_SCB_MMFAR_OFFSET        0x34
#define CM3_SCB_BFAR_OFFSET         0x38

#define CM3_SCB_CPACR_OFFSET        0x88

static inline volatile uint32_t* mem_get_bitband(uint32_t reg_addr, uint32_t reg_bit)
{
#define BITBAND_PERI_REF 0x40000000
#define BITBAND_PERI_BASE 0x42000000
    return mem_get_reg32(BITBAND_PERI_BASE + (reg_addr - BITBAND_PERI_REF) * 32 + reg_bit * 4);
}

#endif /* CM3_MEMMAP_H_ */
