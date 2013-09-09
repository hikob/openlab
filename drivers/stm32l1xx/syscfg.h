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
 * syscfg.h
 *
 *  Created on: Jul 12, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef SYSCFG_H_
#define SYSCFG_H_

#include "gpio.h"
#include "exti.h"

typedef enum
{
    SYSCFG_PORT_A = 0,
    SYSCFG_PORT_B = 1,
    SYSCFG_PORT_C = 2,
    SYSCFG_PORT_D = 3,
    SYSCFG_PORT_E = 4,
    SYSCFG_PORT_H = 5,
} syscfg_port_t;

typedef enum
{
    SYSCFG_PMC_USB = 0
} syscfg_dev_t;

/**
 * Configures a EXTI line corresponding to a GPIO pin to a GPIO port.
 */
void syscfg_select_exti_pin(exti_line_t line, syscfg_port_t port);

/**
 * Configures Peripheral Mode Configuration (PMC) register.
 */
void syscfg_pmc_config(syscfg_dev_t dev, int mode);

#endif /* SYSCFG_H_ */
