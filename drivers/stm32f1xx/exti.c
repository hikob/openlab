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
 * exti.c
 *
 *  Created on: Jul 18, 2011
 *      Author:Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include <stddef.h>

#include "exti.h"
#include "exti_registers.h"

enum
{
    NUMBER_OF_LINES = 20,
};

static handler_t line_handlers[NUMBER_OF_LINES];
static handler_arg_t line_handler_args[NUMBER_OF_LINES];

void exti_set_handler(exti_line_t line, handler_t handler, handler_arg_t arg)
{
    // Store the handler and arg
    line_handlers[line] = handler;
    line_handler_args[line] = arg;
}

void exti_enable_interrupt_line(exti_line_t line, exti_trigger_t trigger)
{
    // Set the trigger(s)
    if (trigger & EXTI_TRIGGER_RISING)
    {
        *exti_get_RTSR() |= BV(line);
    }
    else
    {
        *exti_get_RTSR() &= ~BV(line);
    }

    if (trigger & EXTI_TRIGGER_FALLING)
    {
        *exti_get_FTSR() |= BV(line);
    }
    else
    {
        *exti_get_FTSR() &= ~BV(line);
    }

    // Enable the interrupt line in mask
    *exti_get_IMR() |= BV(line);

    // Write a 1 to PR to clear its value
    *exti_get_PR() = BV(line);
}

void exti_disable_interrupt_line(exti_line_t line)
{
    // Disable the interrupt line in mask
    *exti_get_IMR() &= ~BV(line);

    // Write a 1 to PR to clear its value
    *exti_get_PR() = BV(line);
}

inline static void handle_interrupt(exti_line_t line)
{
    // Clear interrupt bit
    // Write a 1 to PR to clear its value
    *exti_get_PR() = BV(line);

    // Check Handler
    if (line_handlers[line])
    {
        line_handlers[line](line_handler_args[line]);
    }
}

void rtc_alarm_isr()
{
    handle_interrupt(EXTI_LINE_RTC_ALARM);
}

void exti0_isr()
{
    handle_interrupt(EXTI_LINE_Px0);
}

void exti1_isr()
{
    handle_interrupt(EXTI_LINE_Px1);
}

void exti2_isr()
{
    handle_interrupt(EXTI_LINE_Px2);
}

void exti3_isr()
{
    handle_interrupt(EXTI_LINE_Px3);
}

void exti4_isr()
{
    handle_interrupt(EXTI_LINE_Px4);
}

void exti9_5_isr()
{
    // check if EXTI lines 5 to 9 triggered the interrupt
    if (*exti_get_PR() & BV(EXTI_LINE_Px5))
    {
        handle_interrupt(EXTI_LINE_Px5);
    }

    if (*exti_get_PR() & BV(EXTI_LINE_Px6))
    {
        handle_interrupt(EXTI_LINE_Px6);
    }

    if (*exti_get_PR() & BV(EXTI_LINE_Px7))
    {
        handle_interrupt(EXTI_LINE_Px7);
    }

    if (*exti_get_PR() & BV(EXTI_LINE_Px8))
    {
        handle_interrupt(EXTI_LINE_Px8);
    }

    if (*exti_get_PR() & BV(EXTI_LINE_Px9))
    {
        handle_interrupt(EXTI_LINE_Px9);
    }
}

void exti15_10_isr()
{
    // check if EXTI lines 10 to 15 triggered the interrupt
    if (*exti_get_PR() & BV(EXTI_LINE_Px10))
    {
        handle_interrupt(EXTI_LINE_Px10);
    }

    if (*exti_get_PR() & BV(EXTI_LINE_Px11))
    {
        handle_interrupt(EXTI_LINE_Px11);
    }

    if (*exti_get_PR() & BV(EXTI_LINE_Px12))
    {
        handle_interrupt(EXTI_LINE_Px12);
    }

    if (*exti_get_PR() & BV(EXTI_LINE_Px13))
    {
        handle_interrupt(EXTI_LINE_Px13);
    }

    if (*exti_get_PR() & BV(EXTI_LINE_Px14))
    {
        handle_interrupt(EXTI_LINE_Px14);
    }

    if (*exti_get_PR() & BV(EXTI_LINE_Px15))
    {
        handle_interrupt(EXTI_LINE_Px15);
    }
}
