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
 * lsm303dlhc_.h
 *
 *  Created on: Jul 31, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef LSM303DLHC__H_
#define LSM303DLHC__H_

#include "i2c.h"

typedef struct
{
    // We need an I2C link
    i2c_t i2c;
} _lsm303dlhc_t;

/* Configuration */
static inline void lsm303dlhc_config(_lsm303dlhc_t *accmag, i2c_t i2c)
{
    accmag->i2c = i2c;
}

#endif /* LSM303DLHC__H_ */
