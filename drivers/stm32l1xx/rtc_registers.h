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
 * rtc_registers.h
 *
 *  Created on: Jul 7, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef RTC_REGISTERS_H_
#define RTC_REGISTERS_H_

#include "memmap.h"

inline static volatile uint32_t *rtc_get_TR()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_TR_OFFSET);
}
inline static volatile uint32_t *rtc_get_DR()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_DR_OFFSET);
}
inline static volatile uint32_t *rtc_get_CR()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_CR_OFFSET);
}
inline static volatile uint32_t *rtc_get_ISR()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_ISR_OFFSET);
}
inline static volatile uint32_t *rtc_get_PRER()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_PRER_OFFSET);
}
inline static volatile uint32_t *rtc_get_WUTR()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_WUTR_OFFSET);
}
inline static volatile uint32_t *rtc_get_WPR()
{
    return mem_get_reg32(RTC_BASE_ADDRESS + RTC_WPR_OFFSET);
}
inline static void rtc_unlock_write_protection()
{
    // Write first key
    *rtc_get_WPR() = 0xCA;
    // Write second key
    *rtc_get_WPR() = 0x53;
    // Write protection is off
}
inline static void rtc_lock_write_protection()
{
    // Write dummy key
    *rtc_get_WPR() = 0xFF;
}
enum
{
    RTC_CR__COE = 0x00800000,
    RTC_CR__OSEL_MASK = 0x00600000,
    RTC_CR__POL = 0x00100000,
    RTC_CR__BKP = 0x00040000,
    RTC_CR__SUB1H = 0x00020000,
    RTC_CR__ADD1H = 0x00010000,
    RTC_CR__TSIE = 0x00008000,
    RTC_CR__WUTIE = 0x00004000,
    RTC_CR__ALRBIE = 0x00002000,
    RTC_CR__ALRAIE = 0x00001000,
    RTC_CR__TSE = 0x00000800,
    RTC_CR__WUTE = 0x00000400,
    RTC_CR__ALRBE = 0x00000200,
    RTC_CR__ALRAE = 0x00000100,
    RTC_CR__DCE = 0x00000080,
    RTC_CR__FMT = 0x00000040,
    RTC_CR__REFCKON = 0x00000010,
    RTC_CR__TSEDGE = 0x00000008,
    RTC_CR__WUCKSEL_MASK = 0x00000007,
};
enum
{
    RTC_ISR__TAMP1F = 0x2000,
    RTC_ISR__TSOVF = 0x1000,
    RTC_ISR__TSF = 0x0800,
    RTC_ISR__WUTF = 0x0400,
    RTC_ISR__ALRBF = 0x0200,
    RTC_ISR__ALRAF = 0x0100,
    RTC_ISR__INIT = 0x0080,
    RTC_ISR__INITF = 0x0040,
    RTC_ISR__RSF = 0x0020,
    RTC_ISR__INITS = 0x0010,
    RTC_ISR__WUTWF = 0x0004,
    RTC_ISR__ALRBWF = 0x0002,
    RTC_ISR__ARLAWF = 0x0001,
};

#endif /* RTC_REGISTERS_H_ */
