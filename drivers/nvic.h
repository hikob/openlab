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

/**
 * \file nvic.h
 *
 * \date Jul 6, 2011
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef NVIC_H_
#define NVIC_H_

/**
 * \addtogroup drivers
 * @{
 */

/**
 * \defgroup NVIC Cortex-M3 NVIC driver
 *
 * This driver provides the required function to control the NVIC (Nested Vector
 * Interrupt Controller) of the Cortex-M3 core.
 *
 * It allows enabling and disabling the different interrupt lines, as well as
 * controlling the internal SysTick timer for generating periodic tick interrupts.
 *
 *@{
 */

#include <stdint.h>
#include "handler.h"

/**
 * Base type for an interrupt line. The real definition is per STM32 type, but
 * manipulation of interrupt lines is done the same way across all.
 */
typedef uint8_t nvic_irq_line_t;

/**
 * Enable an interrupt line.
 *
 * Once enabled, the interrupt vector will be called on interrupt for this line.
 *
 * \note By default, the interrupt line is enabled with a priority of 0xFF, i.e.
 * the lowest.
 *
 * \param line the interrupt line to enable
 */
void nvic_enable_interrupt_line(nvic_irq_line_t line);

/**
 * Disable an interrupt line.
 *
 * \param line the interrupt line to disable
 */
void nvic_disable_interrupt_line(nvic_irq_line_t line);
/**
 * Set the priority of an enabled interrupt line.
 *
 * By default the priority of an interrupt line is set to 0xFF. It can be changed
 * with this method.
 *
 * \param line the interrupt line to update
 * \param priority a 8bit priority value to set
 */
void nvic_set_priority(nvic_irq_line_t line, uint8_t priority);
/**
 * Get the priority of an enabled interrupt line.
 *
 * \param line the interrupt line to update
 * \return the priority
 */
uint8_t nvic_get_priority(nvic_irq_line_t line);

/**
 * \name SysTick timer
 * @{
 */
/**
 * Enable the Cortex-M3 SysTick timer.
 *
 * This enables and starts the SysTick timer, at a given frequency.
 *
 * \param frequency the desired frequency, in hertz, of the interrupt generation
 * \param handler the handler function to be called on each interrupt
 * \param arg an argument to pass to the handler
 */
void nvic_enable_systick(uint32_t frequency, handler_t handler,
        handler_arg_t arg);

/**
 * Disable the Cortex-M3 SysTick timer.
 */
void nvic_disable_systick();

/**
 * @}
 */

/**
 * Do a system reset.
 */
void nvic_reset();

/**
 * Do a system reset, no dependency
 */
#define NVIC_RESET() do {*((uint32_t*) 0xE000ED0C) = 0x05FA0004;} while (0)

/**
 * @}
 * @}
 */

#endif /* NVIC_H_ */
