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
 * agilefox.h
 *
 *  Created on: Jul 8, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef AGILEFOX_H_
#define AGILEFOX_H_

#include "gpio.h"
#include "timer.h"
#include "spi.h"
#include "i2c.h"
#include "dma.h"
#include "sdio.h"
#include "rf2xx.h"
#include "l3g4200d.h"
#include "lsm303dlhc.h"

/* Drivers */
extern gpio_t gpioA, gpioB, gpioC, gpioD, gpioE, gpioF, gpioG;
extern timer_t tim2, tim3, tim4, tim6, tim7;
extern spi_t spi2;
extern i2c_t i2c1;
extern sdio_t sdio;
extern dma_t dma2_ch4;

/* Peripherals */
extern rf2xx_t rf231;
extern l3g4200d_t l3g4200d;
extern lsm303dlhc_t lsm303dlhc;

void platform_drivers_setup();
void platform_leds_setup();
void platform_button_setup();
void platform_periph_setup();
void platform_lib_setup();
void platform_net_setup();

#endif /* AGILEFOX_H_ */
