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
 * lsm303dlhc.h
 *
 *  Created on: Aug 3, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef LSM303DLHC_H_
#define LSM303DLHC_H_

#include <stdint.h>

/**
 * This type defines an accelerometer
 */
typedef void *lsm303dlhc_t;

typedef enum
{
    LSM303DLHC_ACC_1HZ    = 0x10,
    LSM303DLHC_ACC_10HZ   = 0x20,
    LSM303DLHC_ACC_25HZ   = 0x30,
    LSM303DLHC_ACC_50HZ   = 0x40,
    LSM303DLHC_ACC_100HZ  = 0x50,
    LSM303DLHC_ACC_200HZ  = 0x60,
    LSM303DLHC_ACC_400HZ  = 0x70,
    LSM303DLHC_ACC_1620HZ = 0x80,
    LSM303DLHC_ACC_5376HZ = 0x90
} lsm303dlhc_acc_datarate_t;

typedef enum
{
    LSM303DLHC_MAG_0_75HZ = 0x00,
    LSM303DLHC_MAG_1_5HZ  = 0x04,
    LSM303DLHC_MAG_3HZ    = 0x08,
    LSM303DLHC_MAG_7_5HZ  = 0x0C,
    LSM303DLHC_MAG_15HZ   = 0x10,
    LSM303DLHC_MAG_30HZ   = 0x14,
    LSM303DLHC_MAG_75HZ   = 0x18,
    LSM303DLHC_MAG_220HZ  = 0x1C
} lsm303dlhc_mag_datarate_t;

typedef enum
{
    LSM303DLHC_ACC_2G  = 0x00,
    LSM303DLHC_ACC_4G  = 0x10,
    LSM303DLHC_ACC_8G  = 0x20,
    LSM303DLHC_ACC_16G = 0x30
} lsm303dlhc_acc_scale_t;

typedef enum
{
    LSM303DLHC_MAG_1_3GAUSS = 0x20,
    LSM303DLHC_MAG_1_9GAUSS = 0x40,
    LSM303DLHC_MAG_2_5GAUSS = 0x60,
    LSM303DLHC_MAG_4_0GAUSS = 0x80,
    LSM303DLHC_MAG_4_7GAUSS = 0xA0,
    LSM303DLHC_MAG_5_6GAUSS = 0xC0,
    LSM303DLHC_MAG_8_1GAUSS = 0xE0
} lsm303dlhc_mag_scale_t;

/* Switch off sensor */
void lsm303dlhc_powerdown(lsm303dlhc_t accmag);

/* Data rate configuration */
void lsm303dlhc_set_acc_datarate(lsm303dlhc_t accmag, lsm303dlhc_acc_datarate_t datarate);
void lsm303dlhc_set_mag_datarate(lsm303dlhc_t accmag, lsm303dlhc_mag_datarate_t datarate);

/* Scale configuration */
void lsm303dlhc_set_acc_scale(lsm303dlhc_t accmag, lsm303dlhc_acc_scale_t scale);
void lsm303dlhc_set_mag_scale(lsm303dlhc_t accmag, lsm303dlhc_mag_scale_t scale);

/* Acceleration reading */
void lsm303dlhc_read_acc(lsm303dlhc_t accmag, int16_t *acc);

/* Magnetic field reading */
void lsm303dlhc_read_mag(lsm303dlhc_t accmag, int16_t *mag);

/* Temperature reading */
void lsm303dlhc_read_temp(lsm303dlhc_t accmag, int16_t *temp);

#endif /* LSM303DLHC_H_ */
