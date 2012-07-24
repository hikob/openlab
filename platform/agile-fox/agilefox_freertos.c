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
 * agilefox_freertos.c
 *
 *  Created on: Jul 13, 2012
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */


#include <stdint.h>
#include <stddef.h>

/* FreeRTOS include */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "agilefox.h"
#define LOG_LEVEL LOG_LEVEL_INFO
#include "printf.h"

void platform_run()
{
    /* Start the scheduler. */
    vTaskStartScheduler();
}

static struct
{
    platform_idle_handler_t handler;
    handler_arg_t arg;
} idle_data = {NULL, NULL};

void platform_set_idle_handler(platform_idle_handler_t handler, handler_arg_t arg)
{
    idle_data.handler = handler;
    idle_data.arg = arg;
}

void vApplicationIdleHook(xTaskHandle *pxTask, signed portCHAR *pcTaskName)
{
    // Call handler if any
    if (idle_data.handler)
    {
        if (idle_data.handler(idle_data.arg))
        {
            // Do not halt the CPU
            return;
        }
    }
}

void vApplicationStackOverflowHook(xTaskHandle *pxTask,
                                   signed portCHAR *pcTaskName)
{
    log_error("Task '%s' has stack OVERFLOWED!", pcTaskName);
    HALT();
}
