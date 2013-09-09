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
 * native_drivers.c
 *
 * Created: Sept 2011
 * Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#include "platform.h"
#include "unique_id.h"
#include "timer_.h"

/* Timers instantiations */
static _timer_t _tim1, _tim8;
static _timer_t _tim2, _tim3, _tim4;
static _timer_t _tim6, _tim7;
static _timer_t _tim9, _tim10, _tim11;

/* Timers declarations */
timer_t TIM_1 = TIM_1, TIM_8 = TIM_8;
timer_t TIM_2 = TIM_2, TIM_3 = TIM_3, TIM_4 = TIM_4;
timer_t TIM_6 = TIM_6, TIM_7 = TIM_7;
timer_t TIM_9 = TIM_9, TIM_10 = TIM_10, TIM_11 = TIM_11;

/* unique ID */
uid_t native_uuid;

void native_uuid_init()
{
    int i;

    for (i = 0; i < sizeof(native_uuid); i++)
    {
        native_uuid.uid8[i] = i;
    }
}

/* ------------------------------------------------------------ */
/*                                                              */
/* ------------------------------------------------------------ */

void platform_drivers_setup()
{
    native_uuid_init();
}

/* ------------------------------------------------------------ */
/*                                                              */
/* ------------------------------------------------------------ */


