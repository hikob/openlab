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
 * \file pca9685.h
 *
 * \date Mar 30, 2012
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef PCA9685_H_
#define PCA9685_H_

/**
 * \addtogroup periph
 * @{
 */

/**
 * \defgroup pca9685 PCA9685 LED driver
 *
 * This provides all the methods and types to control a PCA9685 LEDs driver.
 *
 * The PCA9685 is a 16channel LEDs driver that can control a PWM signal on each
 * of its 16 channels. These PWMs can hence control the power of LEDs for example.
 *
 * Each of channel may be configured with a PWM value between 0 and 4095,
 * corresponding to 0-100% respectively.
 *
 * The platform definition has registered one or several PCA9685 chips to this
 * driver. It is possible to retrieve their number and their references through
 * \ref pca9685_get_number and \reg pca9685_get.
 *
 * @{
 */

#include <stdint.h>

/**
 * Abstract type representing a PCA9685, as provided by the platform.
 *
 * It is used by most of the methods as the first argument.
 */
typedef void *pca9685_t;

/**
 * Get the number of registered PCA9685 drivers
 *
 * The platform registers the available PCA9685 onboard at setup, and they are
 * accessible through \ref pca9685_get. This methods allows to get the number of
 * these.
 *
 * \return the number of available drivers
 */
uint32_t pca9685_get_number();
/**
 * Get a pointer to a given driver, based on its index.
 *
 * The maximum index may be retrieved with \ref pca9685_get_number.
 *
 * \param index the drivers's index
 * \return a pointer to this driver, or NULL if out of bound
 */
pca9685_t pca9685_get(uint32_t index);

/**
 * Enable a LED driver
 *
 * This enables (powers UP) a PCA9685 LED driver.
 *
 * \param pca a reference to the PCA9685 driver to enable
 */
void pca9685_enable(pca9685_t pca);
/**
 * Set the PWM value of a PCA9685 channel.
 *
 * This methods allows individual selection of the channels output, by setting
 * their duty cycle. The selected channel (or LED) can have a value ranging from
 * 0 to 4095 corresponding to a duty cycle of 0-100% respectively.
 *
 * \param pca a reference to the PCA9685 driver to access
 * \param led the LED (or channel), valid values between 0 and 15, to configure
 * \param value the PWM value (0-4095)
 */
void pca9685_set_led(pca9685_t pca, uint8_t led, uint16_t value);

/**
 * Set the PWM value of all the PCA9685 channels at once.;
 *
 * \param pca a reference to the PCA9685 driver to access
 * \param value the PWM value (0-4096)
 */
void pca9685_set_all_leds(pca9685_t pca, uint16_t value);

/**
 * @}
 * @}
 */

#endif /* PCA9685_H_ */
