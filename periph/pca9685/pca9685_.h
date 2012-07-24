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
 * Copyright (C) 2012 HiKoB.
 */

/*
 * pca9685_.h
 *
 *  Created on: Mar 30, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef PCA9685__H_
#define PCA9685__H_

#include <stdint.h>

#include "i2c.h"
#include "pca9685.h"

typedef struct
{
    i2c_t i2c;
    uint8_t addr;
} _pca9685_t;

/**
 * Configure a single LED driver
 *
 * \param i2c the I2C driver
 * \param addr
 */
static inline void pca9685_config(_pca9685_t *pca, i2c_t i2c, uint8_t addr)
{
    pca->i2c = i2c;
    pca->addr = addr;
}

/**
 * Initialize the PCA module by specifying all the drivers
 *
 * \param first_pca a pointer to the drivers list
 * \param num_pca the number of drivers
 */
void pca9685_init(_pca9685_t *first_pca, uint32_t num_pca);

#endif /* PCA9685__H_ */
