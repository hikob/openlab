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
 * dangerousnewt.h
 *
 *  Created on: Mar 28, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef DANGEROUSNEWT_H_
#define DANGEROUSNEWT_H_

#include "platform.h"
#include "gpio.h"
#include "timer.h"
#include "spi.h"
#include "i2c.h"
#include "uart.h"
#include "dma.h"
#include "adc.h"

#include "rf2xx.h"

/* Drivers */
extern gpio_t gpioA, gpioB, gpioC, gpioD, gpioE, gpioF, gpioG;
extern timer_t tim2, tim3, tim4, tim6, tim7;
extern spi_t spi2;
extern i2c_t i2c2;
extern uart_t uart1, uart2;
extern adc_t adc1;

/* Peripheral */
extern rf2xx_t rf231;

void platform_drivers_setup();
void platform_leds_setup();
void platform_button_setup();
void platform_periph_setup();
void platform_lib_setup();
void platform_net_setup();

#endif /* DANGEROUSNEWT_H_ */
