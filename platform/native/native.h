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
 * native.h
 *
 * Created: Sept 2011
 * Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#ifndef _NATIVE_H_
#define _NATIVE_H_

#include "timer.h"

/* Drivers */
extern timer_t tim3, tim6;

void platform_drivers_setup();
void platform_leds_setup();
void platform_periph_setup();
void platform_lib_setup();

#endif /* _NATIVE_H_ */
