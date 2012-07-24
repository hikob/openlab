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
 * dac_registers.h
 *
 *  Created on: Oct 17, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef DAC_REGISTERS_H_
#define DAC_REGISTERS_H_

#include <stdint.h>
#include "memmap.h"

static inline volatile uint32_t *dac_get_CR()
{
    return mem_get_reg32(DAC_BASE_ADDRESS + DAC_CR_OFFSET);
}

static inline volatile uint32_t *dac_get_SWTRIGR()
{
    return mem_get_reg32(DAC_BASE_ADDRESS + DAC_SWTRIGR_OFFSET);
}

static inline volatile uint32_t *dac_get_DHR12Rx(uint8_t channel)
{
    return mem_get_reg32(DAC_BASE_ADDRESS + DAC_DHR12R1_OFFSET + channel
                         * (DAC_DHR12R2_OFFSET - DAC_DHR12R1_OFFSET));
}
static inline volatile uint32_t *dac_get_DHR12Lx(uint8_t channel)
{
    return mem_get_reg32(DAC_BASE_ADDRESS + DAC_DHR12L1_OFFSET + channel
                         * (DAC_DHR12R2_OFFSET - DAC_DHR12R1_OFFSET));
}
static inline volatile uint32_t *dac_get_DHR8Rx(uint8_t channel)
{
    return mem_get_reg32(DAC_BASE_ADDRESS + DAC_DHR8R1_OFFSET + channel
                         * (DAC_DHR12R2_OFFSET - DAC_DHR12R1_OFFSET));
}
static inline volatile uint32_t *dac_get_DHR12RD()
{
    return mem_get_reg32(DAC_BASE_ADDRESS + DAC_DHR12RD_OFFSET);
}
static inline volatile uint32_t *dac_get_DHR12LD()
{
    return mem_get_reg32(DAC_BASE_ADDRESS + DAC_DHR12LD_OFFSET);
}
static inline volatile uint32_t *dac_get_DHR8RD()
{
    return mem_get_reg32(DAC_BASE_ADDRESS + DAC_DHR8RD_OFFSET);
}
static inline volatile uint32_t *dac_get_DORx(uint8_t channel)
{
    return mem_get_reg32(DAC_BASE_ADDRESS + DAC_DOR1_OFFSET + channel
                         * (DAC_DOR2_OFFSET - DAC_DOR1_OFFSET));
}
static inline volatile uint32_t *dac_get_SR()
{
    return mem_get_reg32(DAC_BASE_ADDRESS + DAC_SR_OFFSET);
}

enum
{
    DAC_CR__EN1 = 0x00000001,
    DAC_CR__BOFF1 = 0x00000002,
    DAC_CR__TEN1 = 0x00000004,
    DAC_CR__TSEL1_MASK = 0x00000038,
    DAC_CR__WAVE1_MASK = 0x000000C0,
    DAC_CR__MAMP1_MASK = 0x00000F00,
    DAC_CR__DMAEN1 = 0x00001000,
    DAC_CR__DMEUDRIE1 = 0x00002000,

    DAC_CR__EN2 = 0x00010000,
    DAC_CR__BOFF2 = 0x00020000,
    DAC_CR__TEN2 = 0x00040000,
    DAC_CR__TSEL2_MASK = 0x00380000,
    DAC_CR__WAVE2_MASK = 0x00C00000,
    DAC_CR__MAMP2_MASK = 0x0F000000,
    DAC_CR__DMAEN2 = 0x10000000,
    DAC_CR__DMEUDRIE2 = 0x20000000
};

enum
{
    DAC_SWTRIGR__SWTRIG1 = 0x1,
    DAC_SWTRIGR__SWTRIG2 = 0x2
};

#endif /* DAC_REGISTERS_H_ */
