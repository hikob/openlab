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

/** \example example_lsm303dlhc_polling.c
 * This file is an example of the use of L3G4200D gyroscope sensor using periodical
 * sensor measure (polling).
 * \date Sep 27, 2012
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include <math.h>
#include "platform.h"
#include "printf.h"
#include "lsm303dlhc.h"

static void app_task(void *);

int main()
{
    // Initialize the platform
    platform_init();

    // Create a task for the application
    xTaskCreate(app_task, (const signed char * const) "lsm303", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Run
    platform_run();
    return 0;
}

#define ACC_RES (12e-3)  // The resolution is 12mg for the +/-16g scale
#define MAG_RES (1/670.) // The resolution is 1/670 for the +/-2.5gauss scale

static void app_task(void *param)
{
    int16_t a[3], m[3], t;

    printf("# Testing LSM303DLHC\n");

    printf("# Initializing LSM303DLHC...\n");
    lsm303dlhc_powerdown();

    printf("# Setting LSM303DLHC accelerometer\n");
    lsm303dlhc_acc_config(LSM303DLHC_ACC_RATE_400HZ, LSM303DLHC_ACC_SCALE_16G, LSM303DLHC_ACC_UPDATE_ON_READ);

    printf("# Setting LSM303DLHC magnetometer\n");
    lsm303dlhc_mag_config(LSM303DLHC_MAG_RATE_220HZ,
                          LSM303DLHC_MAG_SCALE_2_5GAUSS, LSM303DLHC_MAG_MODE_CONTINUOUS,
                          LSM303DLHC_TEMP_MODE_ON);

    while (1)
    {
        lsm303dlhc_read_acc(a);
        lsm303dlhc_read_mag(m);
        lsm303dlhc_read_temp(&t);

        printf("A = (%f\t%f\t%f)\n", a[0] * ACC_RES, a[1] * ACC_RES, a[2] * ACC_RES);
        printf("M = (%f\t%f\t%f)\n", m[0] * MAG_RES, m[1] * MAG_RES, m[2] * MAG_RES);
        printf("T = %f\n", t / 256.);

        vTaskDelay(configTICK_RATE_HZ / 10);
    }
}
