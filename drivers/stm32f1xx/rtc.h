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
 * rtc.h
 *
 *  Created on: May 28, 2013
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>
#include "handler.h"

/**
 * Enable the RTC, setting its 32kHz clock.
 */
void rtc_enable();

/**
 * Disable the RTC.
 */
void rtc_disable();

/**
 * Set the RTC current 32bit time counter
 */
void rtc_set_time(uint32_t time);

/**
 * Get the RTC current 32bit time counter
 */
uint32_t rtc_time();

/**
 * Set an alarm at a specific time
 *
 * \param time the RTC time to set the alarm at
 * \param handler the handler to call on alarm interrupt
 * \param arg the argument to provide to the handler
 */
void rtc_set_alarm(uint32_t time, handler_t handler, handler_arg_t arg);

void rtc_enable_second_interrupt(handler_t handler, handler_arg_t arg);

#endif /* RTC_H_ */
