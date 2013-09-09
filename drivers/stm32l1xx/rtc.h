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

/**
 * \file rtc.h
 *
 * \date Jul 7, 2011
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 *
 * This driver provides the interface to the STM32L internal RTC.
 */

#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>
#include "handler.h"

/**
 * Available clock frequencies for use by the WakeUp block.
 */
typedef enum
{
    RTC_WAKEUP_CLOCK_2048Hz = 0x0,
    RTC_WAKEUP_CLOCK_4096Hz = 0x1,
    RTC_WAKEUP_CLOCK_8192Hz = 0x2,
    RTC_WAKEUP_CLOCK_16384Hz = 0x3,
    RTC_WAKEUP_CLOCK_1Hz = 0x4,
} rtc_wakeup_clock_t;

/**
 * Enable the RTC, setting its 32kHz clock.
 */
void rtc_enable();
/**
 * Disable the RTC.
 */
void rtc_disable();

/**
 * Initialize the Calendar.
 *
 * Specifies the actual date and time to start from.
 *
 * \param year the current year
 * \param month the current month
 * \param day the current day
 * \param hours the current hour
 * \param minutes the current minute
 * \param seconds the current second
 */
void rtc_calendar_init(uint16_t year, uint8_t month, uint8_t day,
                       uint8_t hours, uint8_t minutes, uint8_t seconds);

/**
 * Configure the RTC to generate a periodic interrupt using the 32kHz clock
 *
 * \param clock the base clock speed to use
 * \param period the period of the interrupt, in clock ticks
 * \param handler the handler function to call at each interrupt
 * \param arg the argument to pass to the handler function
 *
 */
void rtc_set_periodic_wakeup(rtc_wakeup_clock_t clock, uint16_t period,
                             handler_t handler, handler_arg_t arg);

/**
 * Get the tens digit of the current seconds value.
 *
 * \return the tens digit of the seconds
 */
uint8_t rtc_get_seconds_tens();
/**
 * Get the units digit of the current seconds value.
 *
 * \return the units digit of the seconds
 */
uint8_t rtc_get_seconds_units();

/**
 * Get the tens digit of the current minutes value.
 *
 * \return the tens digit of the minutes
 */
uint8_t rtc_get_minutes_tens();
/**
 * Get the units digit of the current minutes value.
 *
 * \return the units digit of the minutes
 */
uint8_t rtc_get_minutes_units();

/**
 * Get the tens digit of the current hours value.
 *
 * \return the tens digit of the hours
 */
uint8_t rtc_get_hours_tens();
/**
 * Get the units digit of the current hours value.
 *
 * \return the units digit of the hours
 */
uint8_t rtc_get_hours_units();

#endif /* RTC_H_ */
