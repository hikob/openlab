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
 * agilefox.h
 *
 *  Created on: Jul 8, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef AGILEFOX_H_
#define AGILEFOX_H_

#include "gpio_.h"
#include "timer_.h"
#include "uart_.h"
#include "spi_.h"
#include "uart_.h"
#include "i2c_.h"
#include "sdio.h"
#include "rf2xx.h"
#include "l3g4200d.h"
#include "lsm303dlhc.h"
#include "lps331.h"

/* Drivers */
extern _gpio_t _gpioA, _gpioB, _gpioC, _gpioD, _gpioE, _gpioF, _gpioG;
extern _timer_t _tim2, _tim3, _tim4, _tim6, _tim7;
extern _uart_t _uart2;
extern _spi_t _spi2;
extern _i2c_t _i2c1;
extern sdio_t sdio;

#define gpioA (&_gpioA)
#define gpioB (&_gpioB)
#define gpioC (&_gpioC)
#define gpioD (&_gpioD)
#define gpioE (&_gpioE)
#define gpioF (&_gpioF)
#define gpioG (&_gpioG)
#define gpioH (&_gpioH)

#define tim2 (&_tim2)
#define tim3 (&_tim3)
#define tim4 (&_tim4)
#define tim6 (&_tim6)
#define tim7 (&_tim7)

#define uart2 (&_uart2)
#define spi2 (&_spi2)
#define i2c1 (&_i2c1)

/* Peripherals */
extern rf2xx_t rf231;

void platform_drivers_setup();
void platform_drivers_restart_timers();
void platform_leds_setup();
void platform_button_setup();
void platform_periph_setup();
void platform_lib_setup();
void platform_net_setup();

#endif /* AGILEFOX_H_ */
