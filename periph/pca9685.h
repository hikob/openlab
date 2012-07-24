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
 * pca9685.h
 *
 *  Created on: Mar 30, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef PCA9685_H_
#define PCA9685_H_

#include <stdint.h>

typedef void *pca9685_t;

/**
 * Get the number of registered PCA drivers
 * \return the number
 */
uint32_t pca9685_get_number();
/**
 * Get a pointer to a given driver
 *
 * \param index the drivers's index
 * \return a pointer to this driver, or NULL
 */
pca9685_t pca9685_get(uint32_t index);

/**
 * Enable the LED driver
 */
void pca9685_enable(pca9685_t pca);
/**
 * Set a LED PWM value
 *
 * \param led the led (0-15)
 * \param value the PWM value (0-4096)
 */
void pca9685_set_led(pca9685_t pca, uint8_t led, uint16_t value);
/**
 * Set all the LEDs values;
 *
 * \param value the PWM value (0-4096)
 */
void pca9685_set_all_leds(pca9685_t pca, uint16_t value);

void pca9685_test(pca9685_t pca);

#endif /* PCA9685_H_ */
