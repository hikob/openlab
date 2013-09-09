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

#include <stdint.h>
#include <math.h>
#include "platform.h"
#include "printf.h"

#include "lsm303dlhc.h"

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
    printf("# Testing LSM303DLHC\n");

    printf("# Initializing LSM303DLHC...\n");
    lsm303dlhc_powerdown();

    printf("# Setting LSM303DLHC accelerometer\n");
    // Note: it is unclear if 'continuous update' is safe, the 4 lsb of each axe might be invalid
    // To read latest and safe data, use 'on-read update' and read accelerometer value twice.
    // (the first read will trigger an update if possible)
    lsm303dlhc_acc_config(LSM303DLHC_ACC_RATE_400HZ, LSM303DLHC_ACC_SCALE_16G, LSM303DLHC_ACC_UPDATE_CONTINUOUS);

    printf("# Setting LSM303DLHC magnetometer\n");
    lsm303dlhc_mag_config(LSM303DLHC_MAG_RATE_220HZ,
                          LSM303DLHC_MAG_SCALE_2_5GAUSS, LSM303DLHC_MAG_MODE_CONTINUOUS,
                          LSM303DLHC_TEMP_MODE_ON);

    while (1)
    {
        int16_t a[3], m[3], t;
        double tilt, tilt_x, tilt_y;

        uint32_t ta = soft_timer_time();
        lsm303dlhc_read_acc(a);
        lsm303dlhc_read_mag(m);
        lsm303dlhc_read_temp(&t);
        uint32_t tb = soft_timer_time();

        tilt = 180 * acos(a[2] / sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2])) / M_PI;
        tilt_x = 180 * atan2(a[1], a[2]) / M_PI;
        tilt_y = 180 * atan2(-a[0], hypot(a[1], a[2])) / M_PI;

        printf("%5d\t%5d\t%5d\t%5d\t%5d\t%5d\t%5d\t%5d\t%5d\t%5d => %d\n", a[0], a[1], a[2], m[0],
               m[1], m[2], t, (int16_t)tilt, (int16_t)tilt_x, (int16_t)tilt_y, tb-ta);

        soft_timer_delay_ms(500);
    }
}
