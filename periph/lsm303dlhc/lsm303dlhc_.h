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
 * lsm303dlhc_.h
 *
 *  Created on: Jul 31, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef LSM303DLHC__H_
#define LSM303DLHC__H_

#include "lsm303dlhc.h"
#include "i2c.h"
#include "exti.h"
#include "gpio.h"
#include "timer.h"

/** Configuration */
uint8_t lsm303dlhc_config(i2c_t i2c,
                       exti_line_t mag_data_ready_line,
                       gpio_t mag_drdy_gpio, gpio_pin_t mag_drdy_pin,
                       exti_line_t acc_int1_line,
                       gpio_t acc_int1_gpio, gpio_pin_t acc_int1_pin,
                       exti_line_t acc_int2_line,
                       gpio_t acc_int2_gpio, gpio_pin_t acc_int2_pin);

/** Special config where int1 don't use a standard EXTI but a timer channel interrupt */
void lsm303dlhc_config_acc_int1_uses_timer(openlab_timer_t timer, timer_channel_t channel);

#endif /* LSM303DLHC__H_ */
