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
 * native.c
 *
 * Created: Sept 2011
 * Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#include "platform.h"
#include "unique_id.h"
#include "random.h"
#include "printf.h"
#include "debug.h"

/* ------------------------------------------------------------ */
/*                                                              */
/* ------------------------------------------------------------ */

void platform_init(void)
{
    // Setup the drivers
    platform_drivers_setup();

    // Setup the LEDs
    platform_leds_setup();

    // Setup the peripherals
    platform_periph_setup();

    // Setup the libraries
    platform_lib_setup();

    // Feed the random number generator
    random_init(uid->uid32[2]);
}

/* ------------------------------------------------------------ */
/*                                                              */
/* ------------------------------------------------------------ */

void platform_run()
{
    /* Start the scheduler. */
    vTaskStartScheduler();
}
void platform_prevent_low_power() {}
void platform_release_low_power() {}



/* ------------------------------------------------------------ */
/*                                                              */
/* ------------------------------------------------------------ */

/* We cannot #include <stdio.h> due to local printf.h definitions
 * Since we only need the putchar() prototype, here it is ...
 */
int putchar(int c);

void xputc(char c)
{
    putchar(c);
}

/* ------------------------------------------------------------ */
/*                                                              */
/* ------------------------------------------------------------ */

void vApplicationStackOverflowHook(xTaskHandle *pxTask,
                                   signed portCHAR *pcTaskName)
{
    log_error("Task '%s' has stack OVERFLOWED!", pcTaskName);

    while (1)
    {
        asm volatile("nop");
    }
}

/* ------------------------------------------------------------ */
/*                                                              */
/* ------------------------------------------------------------ */
