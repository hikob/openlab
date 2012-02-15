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
 * nvic.h
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef NVIC_H_
#define NVIC_H_

#include <stdint.h>
#include "handler.h"

typedef uint8_t nvic_irq_line_t;

void nvic_enable_interrupt_line(nvic_irq_line_t line);
void nvic_disable_interrupt_line(nvic_irq_line_t line);

void nvic_enable_systick(uint32_t frequency, handler_t handler, handler_arg_t arg);
void nvic_disable_systick();

/**
 * Do a system reset.
 */
void nvic_reset();

/**
 * Do a system reset, no dependency
 */
#define NVIC_RESET() do {*((uint32_t*) 0xE000ED0C) = 0x05FA0004;} while (0)

#endif /* NVIC_H_ */
