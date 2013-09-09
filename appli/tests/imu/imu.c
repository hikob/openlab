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
 * l3g4200d.c
 *
 *  Created on: Jul 31, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"

#include "l3g4200d.h"
#include "lsm303dlhc.h"

#include "printf.h"
#include "debug.h"

static int16_t gyro_values[3];
static void gyro_data(handler_arg_t arg);
static void read_gyro(handler_arg_t arg);

static int16_t mag_values[3];
static void mag_data(handler_arg_t arg);
static void read_mag(handler_arg_t arg);

#if ACC_ENABLED
static int16_t acc_values[3];
static void acc_data(handler_arg_t arg);
static void read_acc(handler_arg_t arg);
#endif

int main()
{
    // Initialize the platform
    platform_init();

    printf("# Testing L3G4200D AND LSM303DLHC both on interrupts\n");

    printf("# Setting L3G4200D datarate and scale...\n");
    l3g4200d_gyr_config(L3G4200D_200HZ, L3G4200D_2000DPS, true);

    lsm303dlhc_powerdown();
    printf("# Setting LSM303DLHC accelerometer...\n");
    lsm303dlhc_acc_config(LSM303DLHC_ACC_RATE_10HZ, LSM303DLHC_ACC_SCALE_8G, LSM303DLHC_ACC_UPDATE_ON_READ);
    printf("# Setting LSM303DLHC magnetometer...\n");
    lsm303dlhc_mag_config(LSM303DLHC_MAG_RATE_0_75HZ,
            LSM303DLHC_MAG_SCALE_2_5GAUSS, LSM303DLHC_MAG_MODE_CONTINUOUS
            , LSM303DLHC_TEMP_MODE_OFF);

    printf("# Setting LSM303DLHC interrupt...\n");
    lsm303dlhc_mag_set_drdy_int(mag_data, NULL);
    read_mag(NULL);

#if ACC_ENABLED
    lsm303dlhc_acc_set_drdy_int2(acc_data, NULL);
    read_acc(NULL);
#endif

    printf("# Setting L3G4200D interrupt...\n");
    l3g4200d_set_drdy_int(gyro_data, NULL);
    read_gyro(NULL);

    platform_run();

    return 0;
}

static void gyro_data(handler_arg_t arg)
{
    event_post(EVENT_QUEUE_APPLI, read_gyro, arg);
}

static void read_gyro(handler_arg_t arg)
{
    static int i = 0;
    l3g4200d_read_rot_speed(gyro_values);
    i++;
    if (i == 11)
    {
        printf("G: %d %d %d\n", gyro_values[0], gyro_values[1], gyro_values[2]);
        i = 0;
    }
}

static void mag_data(handler_arg_t arg)
{
    event_post(EVENT_QUEUE_APPLI, read_mag, arg);
}

static void read_mag(handler_arg_t arg)
{

    lsm303dlhc_read_mag(mag_values);
    printf("M: %d %d %d\n", mag_values[0], mag_values[1], mag_values[2]);
}

#if ACC_ENABLED

static void acc_data(handler_arg_t arg)
{
    event_post(EVENT_QUEUE_APPLI, read_acc, arg);
}

static void read_acc(handler_arg_t arg)
{

    lsm303dlhc_read_acc(acc_values);
    printf("A: %d %d %d\n", acc_values[0], acc_values[1], acc_values[2]);
}

#endif



