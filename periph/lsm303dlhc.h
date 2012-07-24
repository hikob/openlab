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
 * lsm303dlhc.h
 *
 *  Created on: Aug 3, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef LSM303DLHC_H_
#define LSM303DLHC_H_

#include <stdbool.h>
#include <stdint.h>
#include "handler.h"

typedef enum
{
    LSM303DLHC_ACC_RATE_1HZ = 0x10,
    LSM303DLHC_ACC_RATE_10HZ = 0x20,
    LSM303DLHC_ACC_RATE_25HZ = 0x30,
    LSM303DLHC_ACC_RATE_50HZ = 0x40,
    LSM303DLHC_ACC_RATE_100HZ = 0x50,
    LSM303DLHC_ACC_RATE_200HZ = 0x60,
    LSM303DLHC_ACC_RATE_400HZ = 0x70,
    LSM303DLHC_ACC_RATE_1620HZ = 0x80,
    LSM303DLHC_ACC_RATE_5376HZ = 0x90
} lsm303dlhc_acc_datarate_t;

typedef enum
{
    LSM303DLHC_MAG_RATE_0_75HZ = 0x00,
    LSM303DLHC_MAG_RATE_1_5HZ = 0x04,
    LSM303DLHC_MAG_RATE_3HZ = 0x08,
    LSM303DLHC_MAG_RATE_7_5HZ = 0x0C,
    LSM303DLHC_MAG_RATE_15HZ = 0x10,
    LSM303DLHC_MAG_RATE_30HZ = 0x14,
    LSM303DLHC_MAG_RATE_75HZ = 0x18,
    LSM303DLHC_MAG_RATE_220HZ = 0x1C
} lsm303dlhc_mag_datarate_t;

typedef enum
{
    LSM303DLHC_ACC_SCALE_2G = 0x00,
    LSM303DLHC_ACC_SCALE_4G = 0x10,
    LSM303DLHC_ACC_SCALE_8G = 0x20,
    LSM303DLHC_ACC_SCALE_16G = 0x30
} lsm303dlhc_acc_scale_t;

typedef enum
{
    LSM303DLHC_MAG_SCALE_1_3GAUSS = 0x20,
    LSM303DLHC_MAG_SCALE_1_9GAUSS = 0x40,
    LSM303DLHC_MAG_SCALE_2_5GAUSS = 0x60,
    LSM303DLHC_MAG_SCALE_4_0GAUSS = 0x80,
    LSM303DLHC_MAG_SCALE_4_7GAUSS = 0xA0,
    LSM303DLHC_MAG_SCALE_5_6GAUSS = 0xC0,
    LSM303DLHC_MAG_SCALE_8_1GAUSS = 0xE0
} lsm303dlhc_mag_scale_t;

typedef enum
{
    LSM303DLHC_MAG_MODE_CONTINUOUS = 0x00,
    LSM303DLHC_MAG_MODE_SINGLE = 0x01,
    LSM303DLHC_MAG_MODE_OFF = 0x02,
} lsm303dlhc_mag_mode_t;

typedef enum
{
    LSM303DLHC_TEMP_MODE_ON = 0x80,
    LSM303DLHC_TEMP_MODE_OFF = 0x00,
} lsm303dlhc_temp_mode_t;


/* Switch off the sensor */
void lsm303dlhc_powerdown();

/** Magnetometer and temperature sensor configuration */
void lsm303dlhc_mag_config(lsm303dlhc_mag_datarate_t datarate,
                           lsm303dlhc_mag_scale_t scale, lsm303dlhc_mag_mode_t mode,
                           lsm303dlhc_temp_mode_t temp_mode);

/** Configure the data ready interrupt */
void lsm303dlhc_mag_set_drdy_int(handler_t data_ready_handler,
                                 handler_arg_t data_ready_arg);

/** Request a single conversion */
void lsm303dlhc_mag_sample_single();

/** Compute the closest valid data rate to a given sampling frequency */
lsm303dlhc_mag_datarate_t lsm303dlhc_mag_compute_datarate(uint32_t freq);

/** Accelerometer configuration */
void lsm303dlhc_acc_config(lsm303dlhc_acc_datarate_t datarate,
                           lsm303dlhc_acc_scale_t scale);

/** Configure the data ready interrupt (XXX doesn't work yet) */
void lsm303dlhc_acc_set_drdy_int(handler_t data_ready_handler,
                                 handler_arg_t data_ready_arg);

/* Acceleration reading */
void lsm303dlhc_read_acc(int16_t *acc);

/* Magnetic field reading */
void lsm303dlhc_read_mag(int16_t *mag);

/* Temperature reading */
void lsm303dlhc_read_temp(int16_t *temp);

#endif /* LSM303DLHC_H_ */
