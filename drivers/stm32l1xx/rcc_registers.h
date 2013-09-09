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
 * rcc_registers.h
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef RCC_REGISTERS_H_
#define RCC_REGISTERS_H_
#include "memmap.h"

inline static volatile uint32_t *rcc_get_CR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_CR_OFFSET);
}

inline static volatile uint32_t *rcc_get_ICSCR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_ICSCR_OFFSET);
}

inline static volatile uint32_t *rcc_get_CFGR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_CFGR_OFFSET);
}

inline static volatile uint32_t *rcc_get_CIR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_CIR_OFFSET);
}

inline static volatile uint32_t *rcc_get_AHBRSTR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_AHBRSTR_OFFSET);
}

inline static volatile uint32_t *rcc_get_APB2RSTR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_APB2RSTR_OFFSET);
}

inline static volatile uint32_t *rcc_get_APB1RSTR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_APB1RSTR_OFFSET);
}

inline static volatile uint32_t *getAPBxRSTR(uint8_t bus)
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_APB2RSTR_OFFSET + (bus == 1 ? 4
                         : 0));
}

inline static volatile uint32_t *rcc_get_AHBENR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_AHBENR_OFFSET);
}

inline static volatile uint32_t *rcc_get_APB2ENR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_APB2ENR_OFFSET);
}

inline static volatile uint32_t *rcc_get_APB1ENR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_APB1ENR_OFFSET);
}

inline static volatile uint32_t *rcc_get_APBxENR(uint8_t bus)
{
    if (bus == 1)
    {
        return mem_get_reg32(RCC_BASE_ADDRESS + RCC_APB1ENR_OFFSET);
    }
    else
    {
        return mem_get_reg32(RCC_BASE_ADDRESS + RCC_APB2ENR_OFFSET);
    }
}

inline static volatile uint32_t *rcc_get_AHBLPENR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_AHBLPENR_OFFSET);
}

inline static volatile uint32_t *rcc_get_APB2LPENR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_APB2LPENR_OFFSET);
}

inline static volatile uint32_t *rcc_get_APB1LPENR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_APB1LPENR_OFFSET);
}

inline static volatile uint32_t *rcc_get_APBxLPENR(uint8_t bus)
{
    if (bus == 1)
    {
        return mem_get_reg32(RCC_BASE_ADDRESS + RCC_APB1LPENR_OFFSET);
    }
    else
    {
        return mem_get_reg32(RCC_BASE_ADDRESS + RCC_APB2LPENR_OFFSET);
    }
}

inline static volatile uint32_t *rcc_get_CSR()
{
    return mem_get_reg32(RCC_BASE_ADDRESS + RCC_CSR_OFFSET);
}

inline static volatile uint32_t *rcc_get_DBGMCU_CR()
{
    return mem_get_reg32(DBGMCU_CR);
}
inline static volatile uint32_t *rcc_get_DBGMCU_APB1_FZ()
{
    return mem_get_reg32(DBGMCU_APB1_FZ);
}

inline static volatile uint32_t *rcc_get_DBGMCU_APB2_FZ()
{
    return mem_get_reg32(DBGMCU_APB2_FZ);
}

enum
{
    RCC_CR__RTCPRE_MASK = 0x60000000,
    RCC_CR__CSSON_MASK  = 0x10000000,
    RCC_CR__PLLRDY_MASK = 0x02000000,
    RCC_CR__PLLON_MASK  = 0x01000000,
    RCC_CR__HSEBYP_MASK = 0x00040000,
    RCC_CR__HSERDY_MASK = 0x00020000,
    RCC_CR__HSEON_MASK  = 0x00010000,
    RCC_CR__MSIRDY_MASK = 0x00000200,
    RCC_CR__MSION_MASK  = 0x00000100,
    RCC_CR__HSIRDY_MASK = 0x00000002,
    RCC_CR__HSION_MASK  = 0x00000001,
};

enum
{
    RCC_ICSCR__MSITRIM_MASK = 0xFF000000,
    RCC_ICSCR__MSICAL_MASK = 0x00FF0000,
    RCC_ICSCR__MSIRANGE_MASK = 0x0000E000,
    RCC_ICSCR__HSITRIM_MASK = 0x00001F00,
    RCC_ICSCR__HSICAL_MASK = 0x000000FF,
};

enum
{
    RCC_CFGR__MCOPRE_MASK = 0x70000000,
    RCC_CFGR__MCOSEL_MASK = 0x07000000,
    RCC_CFGR__PLLDIV_MASK = 0x00C00000,
    RCC_CFGR__PLLMUL_MASK = 0x003C0000,
    RCC_CFGR__PLLSRC_MASK = 0x00010000,
    RCC_CFGR__PPRE2_MASK = 0x00003800,
    RCC_CFGR__PPRE1_MASK = 0x00000700,
    RCC_CFGR__HPRE_MASK = 0x000000F0,
    RCC_CFGR__SWS_MASK = 0x0000000C,
    RCC_CFGR__SW_MASK = 0x00000003,
};

enum
{
    RCC_CSR__LPWRRSTF_MASK = 0x80000000,
    RCC_CSR__WWDGRSTF_MASK = 0x40000000,
    RCC_CSR__IWDGRSTF_MASK = 0x20000000,
    RCC_CSR__SFTRSTF_MASK = 0x10000000,
    RCC_CSR__PORRSTF_MASK = 0x08000000,
    RCC_CSR__PINRSTF_MASK = 0x04000000,
    RCC_CSR__OBLRSTF_MASK = 0x02000000,
    RCC_CSR__RMVF_MASK = 0x01000000,
    RCC_CSR__RTCRST_MASK = 0x00800000,
    RCC_CSR__RTCEN_MASK = 0x00400000,
    RCC_CSR__RTCSEL_MASK = 0x00030000,
    RCC_CSR__LSEBYP_MASK = 0x00000400,
    RCC_CSR__LSERDY_MASK = 0x00000200,
    RCC_CSR__LSEON_MASK = 0x00000100,
    RCC_CSR__LSIRDY_MASK = 0x00000002,
    RCC_CSR__LSION_MASK = 0x00000001,
};

#endif /* RCC_REGISTERS_H_ */
