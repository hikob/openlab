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
 * afio.h
 *
 *  Created on: Jul 19, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef AFIO_H_
#define AFIO_H_

#include "gpio.h"
#include "exti.h"

typedef enum
{
    AFIO_PORT_A = 0,
    AFIO_PORT_B = 1,
    AFIO_PORT_C = 2,
    AFIO_PORT_D = 3,
    AFIO_PORT_E = 4,
    AFIO_PORT_H = 5,
} afio_port_t;

/**
 * Configures a EXTI line corresponding to a GPIO pin to a GPIO port.
 */
void afio_select_exti_pin(exti_line_t line, afio_port_t port);

#endif /* AFIO_H_ */
