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
 * rcc_registers.h
 *
 *  Created on: Oct 22, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef RCC_REGISTERS_H_
#define RCC_REGISTERS_H_

#include <stdint.h>
#include "memmap.h"

inline static volatile uint32_t* rcc_get_CR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_CR_OFFSET);
}

inline static volatile uint32_t* rcc_get_PLLCFGR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_PLLCFGR_OFFSET);
}
inline static volatile uint32_t* rcc_get_CFGR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_CFGR_OFFSET);
}

inline static volatile uint32_t* rcc_get_CIR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_CIR_OFFSET);
}

inline static volatile uint32_t* rcc_get_AHBxENR(uint8_t bus)
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_AHB1ENR_OFFSET + 4 * (bus - 1));
}

inline static volatile uint32_t* rcc_get_APBxENR(uint8_t bus)
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_APB1ENR_OFFSET + 4 * (bus - 1));
}

inline static volatile uint32_t *rcc_get_APBxRSTR(uint8_t bus)
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_APB1RSTR_OFFSET + 4 * (bus - 1));
}

inline static volatile uint32_t* rcc_get_BDCR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_BDCR_OFFSET);
}

inline static volatile uint32_t* rcc_get_CSR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_CSR_OFFSET);
}
inline static volatile uint32_t* rcc_get_SSCGR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_SSCGR_OFFSET);
}

inline static volatile uint32_t* rcc_get_PLLI2SCFGR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_PLLI2SCFGR_OFFSET);
}

inline static volatile uint32_t* rcc_get_DBGMCU_APB1_FZ()
{
    return mem_get_reg32(DBGMCU_APB1_FZ);
}

inline static volatile uint32_t* rcc_get_DBGMCU_APB2_FZ()
{
    return mem_get_reg32(DBGMCU_APB2_FZ);
}

enum
{
    RCC_CR__PLLI2SRDY = 0x08000000,
    RCC_CR__PLLI2SON = 0x04000000,
    RCC_CR__PLLRDY = 0x02000000,
    RCC_CR__PLLON = 0x01000000,
    RCC_CR__CSSON = 0x00080000,
    RCC_CR__HSEBYP = 0x00040000,
    RCC_CR__HSERDY = 0x00020000,
    RCC_CR__HSEON = 0x00010000,
    RCC_CR__HSICAL_MASK = 0x0000FF00,
    RCC_CR__HSITRIM_MASK = 0x000000F8,
    RCC_CR__HSIRDY = 0x00000002,
    RCC_CR__HSION = 0x00000001
};
enum
{
    RCC_PLLCFGR__PLLQ_MASK = 0x0F000000,
    RCC_PLLCFGR__PLLSRC = 0x00400000,
    RCC_PLLCFGR__PLLP_MASK = 0x00030000,
    RCC_PLLCFGR__PLLN_MASK = 0x00007FC0,
    RCC_PLLCFGR__PLLM_MASK = 0x0000003F,
};
enum
{
    RCC_CFGR__MCO2_MASK = 0xC0000000,
    RCC_CFGR__MCO2PRE_MASK = 0x38000000,
    RCC_CFGR__MCO1PRE_MASK = 0x07000000,
    RCC_CFGR__I2SSCR = 0x00800000,

    RCC_CFGR__MCO1_MASK = 0x00600000,
    RCC_CFGR__RTCPRE_MASK = 0x001F0000,
    RCC_CFGR__PPRE2_MASK = 0x0000E000,
    RCC_CFGR__PPRE1_MASK = 0x00001C00,
    RCC_CFGR__HPRE_MASK = 0x000000F0,
    RCC_CFGR__SWS_MASK = 0x0000000C,
    RCC_CFGR__SW_MASK = 0x00000003,

};

enum
{
    RCC_BDCR__BDRST = 0x00010000,
    RCC_BDCR__RTCEN = 0x00008000,
    RCC_BDCR__RTCSEL_MASK = 0x00000300,
    RCC_BDCR__LSEBYP = 0x00000004,
    RCC_BDCR__LSERDY = 0x00000002,
    RCC_BDCR__LSEON = 0x00000001,
};

enum
{
    RCC_CSR__LPWRRSTF = 0x80000000,
    RCC_CSR__WWDGRSTF = 0x40000000,
    RCC_CSR__IWDGRSTF = 0x20000000,
    RCC_CSR__SFTRSTF = 0x10000000,
    RCC_CSR__PORRSTF = 0x08000000,
    RCC_CSR__PINRSTF = 0x04000000,
    RCC_CSR__BORRSTF = 0x02000000,
    RCC_CSR__RMVF = 0x01000000,
    RCC_CSR__LSIRDY = 0x00000002,
    RCC_CSR__LSION = 0x00000001
};

#endif /* RCC_REGISTERS_H_ */
