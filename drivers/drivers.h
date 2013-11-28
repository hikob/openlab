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
 * \file dma.h
 *
 * \date Aug 19, 2011
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

/**
 * \file drivers.h
 *
 * \date Jan 7, 2013
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef DRIVERS_H_
#define DRIVERS_H_

/**
 * \addtogroup drivers
 * @{
 */

/**
 * \defgroup DirectDrivers Drivers direct access
 *
 * This module provides direct access to all available drivers (GPIO, SPI, UART...),
 * but should be used with care. Normal driver access should be defined in the
 * platform folder, and configured there for use by various peripheral or library
 * modules.
 *
 * All drivers listed here may not exist on your platform! It would then not compile.
 * Note that some peripherals must have their GPIO configured before use (e.g.
 * UART, SPI, I2C) which may only be done in the platform folder.
 *
 * @{
 */

#include "adc.h"
extern const adc_t adc1;

#include "gpio.h"
extern const gpio_t gpioA, gpioB, gpioC, gpioD, gpioE, gpioF, gpioG, gpioH,
        gpioI;

#include "i2c.h"
extern const i2c_t i2c1, i2c2;

#include "spi.h"
extern const spi_t spi1, spi2;

#include "timer.h"
extern const openlab_timer_t tim1, tim2, tim3, tim4, tim5, tim6, tim7, tim8, tim9,
        tim10, tim11, tim12, tim13, tim14, tim15;

#include "uart.h"
extern const uart_t uart1, uart2, uart3;

/**
 *}@
 */

/**
 *}@
 */

#endif /* DRIVERS_H_ */
