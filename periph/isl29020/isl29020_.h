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
 * isl29020_.h
 *
 *  Created on: Dec 3, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef ISL29020__H_
#define ISL29020__H_

#include <stdint.h>
#include "isl29020.h"
#include "i2c.h"

/**
 * Configure the ISL light sensor.
 *
 * Register the I2C bus in use, as well as the A0 bit of the address (either 1 or 0)
 *
 * \param i2c the i2c driver to use
 * \param a0 the A0 address bit
 */
void isl29020_config(i2c_t i2c, uint32_t a0);


#endif /* ISL29020__H_ */
