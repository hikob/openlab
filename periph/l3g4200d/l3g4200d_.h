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
 * l3g4200d_.h
 *
 *  Created on: Jul 31, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef L3G4200D__H_
#define L3G4200D__H_

#include "l3g4200d.h"
#include "i2c.h"
#include "exti.h"

/* Configuration */
uint8_t l3g4200d_config(i2c_t i2c, exti_line_t data_ready_line,
                       gpio_t drdy_gpio,
                       gpio_pin_t drdy_gpio_pin);

#endif /* L3G4200D__H_ */
