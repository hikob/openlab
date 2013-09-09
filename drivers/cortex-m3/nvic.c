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
 * nvic.c
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "nvic.h"
#include "cm3_nvic_registers.h"
#include "cm3_scb_registers.h"
#include "handler.h"

#include "rcc_sysclk.h"

static handler_t tick_handler;
static handler_arg_t tick_handler_arg;

void nvic_enable_interrupt_line(nvic_irq_line_t line)
{
    uint8_t word, bit;
    // Force the priority to 0xFF (lowest)
    nvic_set_priority(line, 0xFF);

    // Set the bit in the word corresponding to this position
    word = line / 32;
    bit = line % 32;
    *cm3_nvic_get_SET_ENABLE(word) = BV(bit);
}
void nvic_disable_interrupt_line(nvic_irq_line_t line)
{
    uint8_t word, bit;
    // Set the bit in the word corresponding to this position
    word = line / 32;
    bit = line % 32;
    *cm3_nvic_get_CLEAR_ENABLE(word) = BV(bit);

    // Reset the priority to 0
    nvic_set_priority(line, 0x0);

    // Clear the pending bit
    *cm3_nvic_get_CLEAR_PENDING(word) = BV(bit);
}

void nvic_set_priority(nvic_irq_line_t line, uint8_t priority)
{
    volatile uint8_t *priority_reg;

    // Get a pointer to the register
    priority_reg = cm3_nvic_get_PRIORITY(line);

    // Set given value
    *priority_reg = priority;
}
uint8_t nvic_get_priority(nvic_irq_line_t line)
{
    volatile uint8_t *priority_reg;

    // Get a pointer to the register
    priority_reg = cm3_nvic_get_PRIORITY(line);

    return *priority_reg;
}

void nvic_enable_systick(uint32_t freq, handler_t handler, handler_arg_t arg)
{
    // Store handler
    tick_handler = handler;
    tick_handler_arg = arg;

    uint32_t reload_value;
    reload_value = rcc_sysclk_get_clock_frequency(RCC_SYSCLK_CLOCK_SYSTICK_CLK)
                   / freq;

    // Set the SysTick reload value
    *cm3_nvic_get_SYSTICK_RELOAD_VALUE() = reload_value - 1;

    // Clear the current value
    *cm3_nvic_get_SYSTICK_CURRENT_VALUE() = 0;

    // Enable counting and interrupt
    *cm3_nvic_get_SYSTICK_CTRL() = SYSTICK_CTRL__CLKSOURCE
                                   | SYSTICK_CTRL__TICKINT | SYSTICK_CTRL__ENABLE;
}

void nvic_disable_systick()
{
    // Disable counting and interrupt
    *cm3_nvic_get_SYSTICK_CTRL() = 0;
}

void systick_handler()
{
    if (tick_handler)
    {
        tick_handler(tick_handler_arg);
    }
}

void nvic_reset()
{
    // Set the SYSRESETREQ bit in the Application Interrupt and Reset Control Register
    *cm3_scb_get_AIRCR() = 0x05FA0004;
}
