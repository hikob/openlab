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
 * agilefox_lib.c
 *
 *  Created on: Jul 13, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "agilefox.h"

#include "delay.h"
#include "event.h"

void platform_lib_setup()
{
    // Setup the delay library
    delay_init(tim3);

    // Setup the event system
    event_init();
}
