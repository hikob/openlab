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
 * soft_timer_delay.h
 *
 *  Created on: Jan 13, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef SOFT_TIMER_DELAY_H_
#define SOFT_TIMER_DELAY_H_

#include <stdint.h>

enum
{
    SOFT_TIMER_FREQUENCY = 32768,
};


static inline int32_t soft_timer_us_to_ticks(int32_t us)
{
    return (us * SOFT_TIMER_FREQUENCY) / 1000000;
}
static inline int32_t soft_timer_ms_to_ticks(int32_t ms)
{
    return (ms * SOFT_TIMER_FREQUENCY) / 1000;
}
static inline int32_t soft_timer_s_to_ticks(int32_t s)
{
    return (s * SOFT_TIMER_FREQUENCY);
}
static inline int32_t soft_timer_min_to_ticks(int32_t min)
{
    return (min * 60 * SOFT_TIMER_FREQUENCY);
}

static inline int32_t soft_timer_ticks_to_us(int32_t ticks)
{
    return (ticks * 1000000) / SOFT_TIMER_FREQUENCY;
}
static inline int32_t soft_timer_ticks_to_ms(int32_t ticks)
{
    return (ticks * 1000) / SOFT_TIMER_FREQUENCY;
}
static inline int32_t soft_timer_ticks_to_s(int32_t ticks)
{
    return ticks / SOFT_TIMER_FREQUENCY;
}
static inline int32_t soft_timer_ticks_to_min(int32_t ticks)
{
    return ticks / (SOFT_TIMER_FREQUENCY * 60);
}


/**
 * Get the current time, in 32kHz ticks
 */
uint32_t soft_timer_time();

/**
 * Get the complete time based on a 16bit timer time.
 */
uint32_t soft_timer_convert_time(uint16_t t);

/**
 * Get the current time, in seconds
 */
uint32_t soft_timer_time_s();

/**
 * Compare two times and return 1 if a is before b
 */
int32_t soft_timer_a_is_before_b(uint32_t a, uint32_t b);


/** Delay a few us */
void soft_timer_delay_us(uint32_t us);
/** Delay a few ms */
void soft_timer_delay_ms(uint32_t ms);
/** Delay a few s */
void soft_timer_delay_s(uint32_t s);


#endif /* SOFT_TIMER_DELAY_H_ */
