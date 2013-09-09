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

/**
 * \file soft_timer_delay.h
 *
 * \date Jan 13, 2012
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef SOFT_TIMER_DELAY_H_
#define SOFT_TIMER_DELAY_H_

/**
 * \addtogroup lib
 * @{
 */

/**
 * \defgroup soft_timer Software timer
 *
 * The Software Timer library uses a single hardware 16bit timer running with
 * a 32kHz clock to provided extended functionalities such as:
 *   - 32bit timer emulation
 *   - active delays
 *   - as many timer alarms as wished
 *   - use of the \ref event library for executing handler functions
 *
 * The hardware timer used by the library is configured in the initialization
 * section of each platform.
 *
 * When started, the soft timer counts time using its hardware timer. This time
 * value corresponds to an up-time, i.e. the time elapsed since the platform has
 * booted. This time value may be used to timestamp events or data, or to compare
 * time events.
 *
 * \see \ref example_soft_timer_delay.c for an example of using the
 * \ref soft_timer_alarm functions, and \ref example_soft_timer_alarm.c for an example of using
 * the \ref soft_timer_alarm library.
 */

/**
 * \addtogroup soft_timer
 * @{
 */

/**
 * \defgroup soft_timer_delay Soft Timer Delays
 *
 * Provides time conversions and delays.
 *
 * This module provides time access methods, conversions from minutes/seconds/sub-seconds
 * to the internally used 32kHz ticks values (and reverse) methods, and the active
 * delay function that can be used to simply wait for a precise amount of time.
 *
 * \see \ref example_soft_timer_delay.c for an example of using the software
 * timer delay functions.
 *
 * @{
 */

#include <stdint.h>

enum
{
    /** Standard timer frequency, corresponding to a 32kHz clock */
    SOFT_TIMER_FREQUENCY = 32768,
};

/**
 * Convert a duration in microseconds to a number of 32kHz timer ticks.
 *
 * \param us the duration in microseconds
 * \return the corresponding number of 32kHz ticks
 */
static inline int32_t soft_timer_us_to_ticks(int32_t us)
{
    return (((int64_t) us) * SOFT_TIMER_FREQUENCY) / 1000000;
}

/**
 * Convert a duration in milliseconds to a number of 32kHz timer ticks.
 *
 * \param ms the duration in milliseconds
 * \return the corresponding number of 32kHz ticks
 */
static inline int32_t soft_timer_ms_to_ticks(int32_t ms)
{
    return (((int64_t) ms) * SOFT_TIMER_FREQUENCY) / 1000;
}

/**
 * Convert a duration in seconds to a number of 32kHz timer ticks.
 *
 * \param s the duration in seconds
 * \return the corresponding number of 32kHz ticks
 */
static inline int32_t soft_timer_s_to_ticks(int32_t s)
{
    return (s * SOFT_TIMER_FREQUENCY);
}

/**
 * Convert a duration in minutes to a number of 32kHz timer ticks.
 *
 * \param min the duration in minutes
 * \return the corresponding number of 32kHz ticks
 */
static inline int32_t soft_timer_min_to_ticks(int32_t min)
{
    return (min * 60 * SOFT_TIMER_FREQUENCY);
}

/**
 * Convert a number of 32kHz timer ticks to a duration in microseconds.
 *
 * \param ticks the number of 32kHz ticks
 * \return the corresponding duration in microseconds
 */
static inline int32_t soft_timer_ticks_to_us(int32_t ticks)
{
    return (((int64_t) ticks) * 1000000) / SOFT_TIMER_FREQUENCY;
}
/**
 * Convert a number of 32kHz timer ticks to a duration in milliseconds.
 *
 * \param ticks the number of 32kHz ticks
 * \return the corresponding duration in milliseconds
 */
static inline int32_t soft_timer_ticks_to_ms(int32_t ticks)
{
    return (((int64_t) ticks) * 1000) / SOFT_TIMER_FREQUENCY;
}
/**
 * Convert a number of 32kHz timer ticks to a duration in seconds.
 *
 * \param ticks the number of 32kHz ticks
 * \return the corresponding duration in seconds
 */
static inline int32_t soft_timer_ticks_to_s(int32_t ticks)
{
    return ticks / SOFT_TIMER_FREQUENCY;
}
/**
 * Convert a number of 32kHz timer ticks to a duration in minutes.
 *
 * \param ticks the number of 32kHz ticks
 * \return the corresponding duration in minutes
 */
static inline int32_t soft_timer_ticks_to_min(int32_t ticks)
{
    return ticks / (SOFT_TIMER_FREQUENCY * 60);
}


/**
 * Get the current time, in 32kHz ticks as a 32bit value.
 *
 * \return the current time of the Software Timer, in timer ticks
 */
uint32_t soft_timer_time();

/**
 * Get the complete time based on a 16bit timer time.
 *
 * \param t the time of the hardware timer to convert
 * \return the corresponding 32bit time
 */
uint32_t soft_timer_convert_time(uint16_t t);

/**
 * Get the current time, in seconds
 *
 * This allows to get bigger values than the normal \ref soft_timer_time
 *
 * \return the current time, in seconds
 */
uint32_t soft_timer_time_s();

/**
 * Compare two times and return 1 if time a is before time b
 *
 * \param a the first time value to compare
 * \param b the second time value to compare
 * \return 1 if a is before b, 0 otherwise
 */
int32_t soft_timer_a_is_before_b(uint32_t a, uint32_t b);

/**
 * Delay a given number of time ticks.
 *
 * \note This methods actively waits until the delay has elapsed.
 *
 * \param d the number of ticks to wait.
 */
void soft_timer_delay(uint32_t d);

/**
 * Delay a given number of microseconds.
 *
 * \note This methods actively waits until the delay has elapsed.
 *
 * \param us the number of microseconds to wait.
 */
void soft_timer_delay_us(uint32_t us);

/**
 * Delay a given number of milliseconds.
 *
 * \note This methods actively waits until the delay has elapsed.
 *
 * \param us the number of milliseconds to wait.
 */
void soft_timer_delay_ms(uint32_t ms);

/**
 * Delay a given number of seconds.
 *
 * \note This methods actively waits until the delay has elapsed.
 *
 * \param us the number of seconds to wait.
 */
void soft_timer_delay_s(uint32_t s);

/**
 * @}
 * @}
 * @}
 */

#endif /* SOFT_TIMER_DELAY_H_ */
