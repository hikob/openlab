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
 * rcc_rtc.c
 *
 *  Created on: Jul 7, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include "rcc_registers.h"
#include "rcc_rtc.h"

/* RTC class implementation */
void rcc_rtc_reset()
{
    uint32_t csr = *rcc_get_CSR();

    // Set the RTCRST bit in CSR without altering the flags
    *rcc_get_CSR() = RCC_CSR__RTCEN_MASK | RCC_CSR__RTCRST_MASK
                     | (csr & 0xFFFF);

    // Clear the RTCRST bit in CSR
    *rcc_get_CSR() = RCC_CSR__RTCEN_MASK | (csr & 0xFFFF);
}

void rcc_rtc_set_clock_source(rcc_rtc_clock_t clock)
{
    // Clear the Clock in RCC_CSR
    *rcc_get_CSR() &= ~RCC_CSR__RTCSEL_MASK;

    // Set chosen Clock Source
    *rcc_get_CSR() |= clock << 16;
}

void rcc_rtc_enable()
{
    // Set the RTCEN bit in CSR to enable the RTC clock
    *rcc_get_CSR() |= RCC_CSR__RTCEN_MASK;
}
void rcc_rtc_disable()
{
    // Clear the RTCEN bit in CSR to enable the RTC clock
    *rcc_get_CSR() &= ~RCC_CSR__RTCEN_MASK;
}

uint32_t rcc_rtc_is_enabled()
{
    // Check if the RTCEN bit in CSR is set
    return (*rcc_get_CSR() & RCC_CSR__RTCEN_MASK);
}
