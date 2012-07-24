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
 * triled.h
 *
 *  Created on: Dec 22, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef TRILED_H_
#define TRILED_H_

#include <stdint.h>

/** Enable the TRILED, starting the PWM */
void triled_enable();

/** Set the triled color */
void triled_set(uint16_t r, uint16_t g, uint16_t b);

/** Disable the TRILED and , stop the PWM */
void triled_disable();

#endif /* TRILED_H_ */
