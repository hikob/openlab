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
 * watchdog.c
 *
 *  Created on: Nov 25, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "watchdog.h"
#include "iwdg_registers.h"

void watchdog_enable(watchdog_divider_t div, uint16_t reload)
{
    // Enable write register access
    *iwdg_get_KR() = IWDG_KR__ACCESS;

    // Wait until PVU bit is cleared to set new prescaler
    while (*iwdg_get_SR() & IWDG_SR__PVU)
    {
        ;
    }

    // Set new value
    *iwdg_get_PR() = div;

    // Wait until RVU bit is cleared to set new reload value
    while (*iwdg_get_SR() & IWDG_SR__RVU)
    {
        ;
    }

    // Set new value
    *iwdg_get_RLR() = (reload > 0xFFF) ? 0xFFF : reload;

    // Write protect and start
    *iwdg_get_KR() = IWDG_KR__START;

    // Reload
    watchdog_clear();
}

void watchdog_clear()
{
    // Clear
    *iwdg_get_KR() = IWDG_KR__CLEAR;
}
