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
 * azurelion.h
 *
 *  Created on: Jul 8, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef AZURELION_H_
#define AZURELION_H_

#include "platform.h"
#include "stm32l1xx.h"

#include "rf2xx.h"

/* Set default uart_print to 5000000Bd */
#ifndef PLATFORM_UART_PRINT_BAUDRATE
#define PLATFORM_UART_PRINT_BAUDRATE 500000
#endif

/* Peripherals */
extern rf2xx_t rf212, rf231;

/** Flag indicating if board is patched for SLP_TR on PC6 controlled by TIM3CH1 */
extern int32_t al04_has_slptr_on_timer;

void platform_drivers_setup();
void platform_drivers_restart_timers();
void platform_drivers_check_timers();
void platform_disable_uart();
void platform_leds_setup();
void platform_periph_setup();
void platform_lib_setup();
void platform_net_setup();
void platform_io_setup();

#endif /* AZURELION_H_ */
