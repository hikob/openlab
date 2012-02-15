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
 * net_timer.h
 *
 *  Created on: Jul 20, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef NET_TIMER_H_
#define NET_TIMER_H_

#include "event.h"
#include "timer.h"

#define NET_TIMER_FREQUENCY 32768
#define NET_TIMER_US_TO_TICKS(us) \
    ((int32_t) ((int32_t) (us) * NET_TIMER_FREQUENCY / 1000000))
#define NET_TIMER_MS_TO_TICKS(ms) \
    ((int32_t) ((int32_t) (ms) * NET_TIMER_FREQUENCY / 1000))
#define NET_TIMER_TICKS_TO_US(ticks) \
    ((int32_t) ((int32_t) (ticks) * 1000000 / NET_TIMER_FREQUENCY))
#define NET_TIMER_TICKS_TO_MS(ticks) \
    ((int32_t) ((int32_t) (ticks) * 1000 / NET_TIMER_FREQUENCY))

/**
 * Initialize the NET timer library.
 *
 * \param timer a timer, the one also used by the radio chip, it should be
 * configured to have a 32kHz tick;
 */
void net_timer_init(timer_t timer);

/**
 * Get the actual time of the NET timer.
 * \return the time.
 */
uint32_t net_timer_time();

/**
 * Convert a 16bit timer time, to a 32bit time, based on the actual time.
 * \return the time.
 */
uint32_t net_timer_convert_time(uint16_t time16);

#endif /* NET_TIMER_H_ */
