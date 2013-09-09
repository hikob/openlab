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
 * max6x_.h
 *
 *  Created on: Apr 18, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef MAX6X__H_
#define MAX6X__H_

#include "max6x.h"
#include "i2c.h"
#include "gpio.h"
#include "exti.h"
#include "nvic.h"

typedef struct
{
    /** The I2C to use */
    i2c_t i2c;
    /** The GPIO config for Regulator */
    gpio_t reg_gpio;
    gpio_pin_t reg_pin;
    /** The GPIO config for GPS RESET*/
    gpio_t rst_gpio;
    gpio_pin_t rst_pin;
    /** The GPIO config for GPS EN */
    gpio_t en_gpio;
    gpio_pin_t en_pin;
    /** The AFIO config for DATA READY */
    gpio_t ready_gpio;
    gpio_pin_t ready_pin;
    exti_line_t ready_exti_line;
    uint8_t ready_afio_port;
    nvic_irq_line_t ready_nvic_line;
    /** The GPIO config for GPS PULSE */
    gpio_t pulse_gpio;
    gpio_pin_t pulse_pin;
    /** The GPIO config for antenna selection */
    gpio_t antsel_gpio;
    gpio_pin_t antsel_pin;
	/** Is the reset high or low */
	bool rst_high;
} max6x_config_t;

/** The configuration structure to be provided by the platform */
extern const max6x_config_t max6x_config;

#endif /* MAX6X__H_ */
