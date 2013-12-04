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
 * platform.c
 *
 *  Created on: Feb 22, 2013
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "platform.h"
#include "FreeRTOS.h"

#include "debug.h"

__attribute__((weak)) uart_t uart_external = NULL;

platform_reset_cause_t platform_reset_cause;

__attribute__((weak)) void platform_run()
{
    log_printf("FreeRTOS Heap Free: %u\n", xPortGetFreeHeapSize());

    /* Start the scheduler. */
    vTaskStartScheduler();
}
__attribute__((weak)) void platform_prevent_low_power()
{
}
__attribute__((weak)) void platform_release_low_power()
{
}
__attribute__((weak)) uint32_t platform_get_high_power_duration()
{
    return 0;
}
__attribute__((weak)) struct
{
    platform_idle_handler_t handler;
    handler_arg_t arg;
} platform_idle_data =
{ NULL, NULL };

__attribute__((weak)) void platform_set_idle_handler(
        platform_idle_handler_t handler, handler_arg_t arg)
{
    platform_idle_data.handler = handler;
    platform_idle_data.arg = arg;
}

__attribute__((weak)) void vApplicationIdleHook(xTaskHandle *pxTask,
        signed portCHAR *pcTaskName)
{
    // Call handler if any
    if (platform_idle_data.handler)
    {
        if (platform_idle_data.handler(platform_idle_data.arg))
        {
            // Do not halt the CPU
            return;
        }
    }

    // Enter SLEEP mode
    asm volatile("wfi");
}
__attribute__((weak)) void xputc(char c)
{
    uart_transfer(uart_print, (const uint8_t *) &c, 1);
}

__attribute__((weak)) void vApplicationStackOverflowHook(xTaskHandle *pxTask,
        signed portCHAR *pcTaskName)
{
    log_error("Task '%s' has stack OVERFLOWED!", pcTaskName);
    HALT();
}

static int critical_count = 0;
__attribute__((weak)) void platform_enter_critical()
{
    asm volatile ("cpsid i");
    critical_count++;
}
__attribute__((weak)) void platform_exit_critical()
{
    critical_count--;
    if (critical_count == 0)
    {
        asm volatile("cpsie i");
    }
}

__attribute__((weak)) uint32_t button_state()
{
    return 0;
}

__attribute__((weak)) void button_set_handler(handler_t handler, handler_arg_t handler_arg)
{
}
