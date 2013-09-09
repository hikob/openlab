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
 * batfb_itf.c
 *
 *  Created on: Feb 22, 2013
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "batfb.h"
#include "batfb_.h"
#include "gpio.h"
#include "adc.h"

#include "platform.h"
#include "debug.h"

__attribute__((weak)) float batfb_vcc;
__attribute__((weak)) float batfb_vbat;

__attribute__((weak)) batfb_type_t batfb_get_type()
{
    return BATFB_TYPE_NONE;
}

__attribute__((weak)) void batfb_sample(handler_t handler, handler_arg_t arg)
{
}

__attribute__((weak)) void batfb_sample_sync()
{
}

__attribute__((weak)) int32_t batfb_has_solar()
{
    return 0;
}

__attribute__((weak)) batfb_solar_status_t batfb_get_solar_state()
{
    return 0;
}
