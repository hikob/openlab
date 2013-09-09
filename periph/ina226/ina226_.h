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
 * ina226_.h
 *
 *  Created on: Jul 12, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef INA226__H_
#define INA226__H_

#include "ina226.h"
#include "i2c.h"
#include "exti.h"

/**
 * Initialize the INA226, specifying its I2C bus and I2C address
 *
 * \param i2c the I2C bus to use
 * \param address the address on the bus
 * \param alert_exti the exti line corresponding to ALERT pin
 */
void ina226_init(i2c_t i2c, uint8_t address, exti_line_t exti_line);

#endif /* INA226__H_ */
