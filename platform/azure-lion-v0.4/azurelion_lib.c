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
 * azurelion_lib.c
 *
 *  Created on: Jul 12, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include "azurelion.h"

#include "softtimer/soft_timer_.h"
#include "event.h"

void platform_lib_setup()
{
    // Setup the software timer
    soft_timer_config(TIM_11, TIMER_CHANNEL_1);
    timer_start(TIM_11, 0xFFFF, soft_timer_update, NULL);

    // Setup the event library
    event_init();
}
