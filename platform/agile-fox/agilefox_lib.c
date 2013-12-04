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
 * agilefox_lib.c
 *
 *  Created on: Jul 13, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */
#include "platform.h"
#include "agilefox.h"
#include "softtimer/soft_timer_.h"

#if !defined(PLATFORM_OS) || (PLATFORM_OS == FREERTOS)
#include "event.h"
#endif

void platform_lib_setup()
{
    // Setup the software timer
    soft_timer_config(TIM_3, TIMER_CHANNEL_1);
    timer_start(TIM_3, 0xFFFF, soft_timer_update, NULL);

    // Setup the event system
    event_init();
}

