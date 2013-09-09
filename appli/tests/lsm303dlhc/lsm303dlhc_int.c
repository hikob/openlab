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
 * lsm303dlhc.c
 *
 *  Created on: Aug 3, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */
#include "nvic.h"
#include "rcc.h"
#include "afio.h"
#include "gpio_.h"
#include "nvic_.h"

#include <stdint.h>
#include <math.h>
#include "platform.h"
#include "printf.h"

#include "lsm303dlhc.h"

#include "soft_timer.h"
#include "event.h"

static void app_task(void *);
static int16_t a[3], m[3], t = 0;

int main()
{
    // Initialize the platform
    platform_init();

    printf("Wait 2 seconds");
    soft_timer_delay_s(2);

    // Create a task for the application
    xTaskCreate(app_task, (const signed char * const) "app", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    platform_run();
    return 0;
}

static void read_mag_acc()
{
    lsm303dlhc_read_acc(a);
    lsm303dlhc_read_mag(m);
    lsm303dlhc_read_temp(&t);
}

static void magneto_data()
{
    event_post(EVENT_QUEUE_APPLI, read_mag_acc, NULL);
}


static void app_task(void *param)
{
    printf("# Testing LSM303DLHC\n");

    printf("# Initializing LSM303DLHC...\n");
    lsm303dlhc_powerdown();

    printf("# Setting LSM303DLHC accelerometer\n");
    lsm303dlhc_acc_config(LSM303DLHC_ACC_RATE_100HZ, LSM303DLHC_ACC_SCALE_16G, LSM303DLHC_ACC_UPDATE_ON_READ);

    printf("# Setting LSM303DLHC magnetometer\n");
    lsm303dlhc_mag_config(LSM303DLHC_MAG_RATE_220HZ,
                          LSM303DLHC_MAG_SCALE_2_5GAUSS, LSM303DLHC_MAG_MODE_CONTINUOUS,
                          LSM303DLHC_TEMP_MODE_ON);

    lsm303dlhc_mag_set_drdy_int(magneto_data, NULL);

    lsm303dlhc_read_acc(a);
    lsm303dlhc_read_mag(m);
    lsm303dlhc_read_temp(&t);

    while (1)
    {

        printf("%5d\t%5d\t%5d\t%5d\t%5d\t%5d\t%5d\n", a[0], a[1], a[2], m[0],
               m[1], m[2], t);

        soft_timer_delay_ms(500);
        lsm303dlhc_mag_set_drdy_int(magneto_data, NULL);
    }
}
