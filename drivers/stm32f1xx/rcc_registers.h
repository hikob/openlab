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
 * rcc_registers.h
 *
 *  Created on: Jul 15, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef RCC_REGISTERS_H_
#define RCC_REGISTERS_H_

#include "memmap.h"

inline static volatile uint32_t *rcc_get_CR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_CR_OFFSET);
}

inline static volatile uint32_t *rcc_get_CFGR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_CFGR_OFFSET);
}

inline static volatile uint32_t *rcc_get_CIR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_CIR_OFFSET);
}

inline static volatile uint32_t *rcc_get_APBxRSTR(uint8_t bus)
{
    return mem_get_reg32(RCC_BASE_ADDRESS + (bus == 2 ? RCC_APB2RSTR_OFFSET : RCC_APB1RSTR_OFFSET));
}

inline static volatile uint32_t *rcc_get_AHBENR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_AHBENR_OFFSET);
}

inline static volatile uint32_t *rcc_get_APBxENR(uint8_t bus)
{
    return mem_get_reg32(RCC_BASE_ADDRESS + (bus == 2 ? RCC_APB2ENR_OFFSET : RCC_APB1ENR_OFFSET));
}

inline static volatile uint32_t *rcc_get_BDCR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_BDCR_OFFSET);
}

inline static volatile uint32_t *rcc_get_CSR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_CSR_OFFSET);
}

inline static volatile uint32_t *rcc_get_AHBRSTR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_AHBRSTR_OFFSET);
}

inline static volatile uint32_t *rcc_get_CFGR2()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_CFGR2_OFFSET);
}


enum
{
    RCC_CR__PLLRDY_MASK  = 0x02000000,
    RCC_CR__PLLON_MASK   = 0x01000000,
    RCC_CR__CSSON_MASK   = 0x00080000,
    RCC_CR__HSEBYP_MASK  = 0x00040000,
    RCC_CR__HSERDY_MASK  = 0x00020000,
    RCC_CR__HSEON_MASK   = 0x00010000,
    RCC_CR__HSICAL_MASK  = 0x0000FF00,
    RCC_CR__HSITRIM_MASK = 0x000000F8,
    RCC_CR__HSIRDY_MASK  = 0x00000002,
    RCC_CR__HSION_MASK   = 0x00000001
};

enum
{
    RCC_CFGR__MCOSEL_MASK   = 0x07000000,
    RCC_CFGR__USBPRE_MASK   = 0x00400000,
    RCC_CFGR__PLLMUL_MASK   = 0x003C0000,
    RCC_CFGR__PLLXTPRE_MASK = 0x00020000,
    RCC_CFGR__PLLSRC_MASK   = 0x00010000,
    RCC_CFGR__ADCPRE_MASK   = 0x0000C000,
    RCC_CFGR__PPRE2_MASK    = 0x00003800,
    RCC_CFGR__PPRE1_MASK    = 0x00000700,
    RCC_CFGR__HPRE_MASK     = 0x000000F0,
    RCC_CFGR__SWS_MASK      = 0x0000000C,
    RCC_CFGR__SW_MASK       = 0x00000003
};


enum
{
    RCC_CSR__LPWRRSTF_MASK = 0x80000000,
    RCC_CSR__WWDGRSTF_MASK = 0x40000000,
    RCC_CSR__IWDGRSTF_MASK = 0x20000000,
    RCC_CSR__SFTRSTF_MASK  = 0x10000000,
    RCC_CSR__PORRSTF_MASK  = 0x08000000,
    RCC_CSR__PINRSTF_MASK  = 0x04000000,
    RCC_CSR__RMVF_MASK     = 0x01000000,
    RCC_CSR__LSIRDY_MASK   = 0x00000002,
    RCC_CSR__LSION_MASK    = 0x00000001
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

#endif /* RCC_REGISTERS_H_ */
