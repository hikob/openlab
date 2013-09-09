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
 * watchdog.h
 *
 * \date Nov 25, 2011
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

/**
 * \addtogroup drivers
 * @{
 */

/**
 * \defgroup Watchdoog Watchdog Timer
 *
 * This hardware watchdog timer executes a hardware reset if not cleared for a given period.
 * This allows do reset the system in case of a software lock.
 *
 *@{
 */

#include <stdint.h>

typedef enum
{
    WATCHDOG_DIVIDER_4 = 0,
    WATCHDOG_DIVIDER_8 = 1,
    WATCHDOG_DIVIDER_16 = 2,
    WATCHDOG_DIVIDER_32 = 3,
    WATCHDOG_DIVIDER_64 = 4,
    WATCHDOG_DIVIDER_128 = 5,
    WATCHDOG_DIVIDER_256 = 6,
} watchdog_divider_t;

/**
 * Start the Independent Watchdog, with the given divider and reload value.
 * Once started it can't be stopped!
 *
 * The Watchdog is clocked from the LSI oscillator.
 *
 * \param div the divider to use;
 * \param reload the reload value to use (from which it downcounts)
 */
void watchdog_enable(watchdog_divider_t div, uint16_t reload);

/**
 * Clear the watchdog.
 */
void watchdog_clear();

/**
 * @}
 * @}
 */

#endif /* WATCHDOG_H_ */
