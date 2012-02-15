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
 * Copyright (C) 2011 HiKoB.
 */

/*
 * lsm303dlhc.c
 *
 *  Created on: Aug 3, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "printf.h"

#include "lsm303dlhc.h"

int main()
{
    int i;

    // Initialize the platform
    platform_init();

    printf("# Testing LSM303DLHC\n");

    printf("# Initializing LSM303DLHC...\n");
    lsm303dlhc_powerdown(lsm303dlhc);

    printf("# Setting LSM303DLHC accelerometer datarate...\n");
    lsm303dlhc_set_acc_datarate(lsm303dlhc, LSM303DLHC_ACC_400HZ);

    printf("# Setting LSM303DLHC accelerometer scale...\n");
    lsm303dlhc_set_acc_scale(lsm303dlhc, LSM303DLHC_ACC_16G);

    printf("# Setting LSM303DLHC magnetometer datarate...\n");
    lsm303dlhc_set_mag_datarate(lsm303dlhc, LSM303DLHC_MAG_220HZ);

    printf("# Setting LSM303DLHC magnetometer scale...\n");
    lsm303dlhc_set_mag_scale(lsm303dlhc, LSM303DLHC_MAG_1_3GAUSS);

    while(1)
    {
        int16_t a[3], m[3], t;

        lsm303dlhc_read_acc(lsm303dlhc, a);
        lsm303dlhc_read_mag(lsm303dlhc, m);
        lsm303dlhc_read_temp(lsm303dlhc, &t);

        printf("%5d\t%5d\t%5d\t%5d\t%5d\t%5d\t%5d\n", a[0], a[1], a[2], m[0], m[1], m[2], t);

        for(i = 0; i < 0x8000; i++)
        {
            __asm__("nop");
        }
    }

    return 0;
}
