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
 * rtc_registers.h
 *
 *  Created on: May 28, 2013
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef RTC_REGISTERS_H_
#define RTC_REGISTERS_H_

#include "memmap.h"

inline static volatile uint32_t *rtc_get_CRH()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_CRH_OFFSET);
}

inline static volatile uint32_t *rtc_get_CRL()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_CRL_OFFSET);
}

inline static volatile uint32_t *rtc_get_PRLH()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_PRLH_OFFSET);
}

inline static volatile uint32_t *rtc_get_PRLL()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_PRLL_OFFSET);
}

inline static volatile uint32_t *rtc_get_DIVH()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_DIVH_OFFSET);
}

inline static volatile uint32_t *rtc_get_DIVL()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_DIVL_OFFSET);
}

inline static volatile uint32_t *rtc_get_CNTH()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_CNTH_OFFSET);
}

inline static volatile uint32_t *rtc_get_CNTL()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_CNTL_OFFSET);
}

inline static volatile uint32_t *rtc_get_ALRH()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_ALRH_OFFSET);
}

inline static volatile uint32_t *rtc_get_ALRL()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_ALRL_OFFSET);
}

enum
{
    RTC_CRL__RTOFF = 0x0020,
    RTC_CRL__CNF = 0x0010,
    RTC_CRL__RSF = 0x0008,
    RTC_CRL__OWF = 0x0004,
    RTC_CRL__ALRF = 0x0002,
    RTC_CRL__SECF = 0x0001,
};

enum
{
    RTC_CRH__OWIE = 0x0004,
    RTC_CRH__ALRIE = 0x0002,
    RTC_CRH__SECIE = 0x0001,
};
#endif /* RTC_REGISTERS_H_ */
