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

#include "platform.h"
#include "stm32f1xx.h"

#include "sdio.h"
#include "rf2xx.h"

/* Set default uart_print to 5000000Bd */
#ifndef PLATFORM_UART_PRINT_BAUDRATE
#define PLATFORM_UART_PRINT_BAUDRATE 500000
#endif

extern sdio_t sdio;

/* Peripherals */
extern rf2xx_t rf231;

void platform_drivers_setup();
void platform_drivers_restart_timers();
void platform_disable_uart();
void platform_leds_setup();
void platform_button_setup();
void platform_periph_setup();
void platform_lib_setup();
void platform_net_setup();

void platform_daughter_setup();

#endif /* AGILEFOX_H_ */
