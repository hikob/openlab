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
 * test_ina226.c
 *
 *  Created on: Jul 12, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include <math.h>
#include "platform.h"
#include "printf.h"
#include "debug.h"

#include "ina226.h"
#include "soft_timer.h"

static void app_task(void *);

int main()
{
    // Initialize the platform
    platform_init();

    // Create a task for the application
    xTaskCreate(app_task, (const signed char * const) "app",
            configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Run
    platform_run();
    return 0;
}

static void app_task(void *param)
{
    log_printf("# Testing INA226\n");

    ina226_calibrate(1, 100e-3);
    ina226_configure(INA226_PERIOD_1100us, INA226_AVERAGE_128);

    while (1)
    {
        float v, i, p, sv;
        ina226_read(&v, &i, &p, &sv);
        log_printf("U=%f V\tI=%f A\tP=%f W\t ShuntV=%f\n", v, i, p, sv);
        soft_timer_delay_ms(100);
    }
}

