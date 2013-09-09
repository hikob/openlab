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
 * rcc_rtc.h
 *
 *  Created on: Jul 7, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef RCC_RTC_H_
#define RCC_RTC_H_

typedef enum rcc_rtc_clock
{
    RCC_RTC_CLOCK_LSE = 0x1,
    RCC_RTC_CLOCK_LSI = 0x2
} rcc_rtc_clock_t;

void rcc_rtc_reset();
void rcc_rtc_set_clock_source(rcc_rtc_clock_t clock);
void rcc_rtc_enable();
void rcc_rtc_disable();
uint32_t rcc_rtc_is_enabled();

#endif /* RCC_RTC_H_ */
