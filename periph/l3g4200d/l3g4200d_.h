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
 * l3g4200d_.h
 *
 *  Created on: Jul 31, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef L3G4200D__H_
#define L3G4200D__H_

#include "i2c.h"

typedef struct
{
    // We need an I2C link
    i2c_t i2c;
} _l3g4200d_t;

/* Configuration */
static inline void l3g4200d_config(_l3g4200d_t *gyro, i2c_t i2c)
{
    gyro->i2c = i2c;
}

#endif /* L3G4200D__H_ */
