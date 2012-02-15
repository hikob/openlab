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

#include "i2c.h"
#include "lsm303dlhc.h"
#include "lsm303dlhc_.h"

#define LSM303DLHC_ACC_ADDRESS   0x32

#define CTRL_REG1_A              0x20
#define CTRL_REG2_A              0x21
#define CTRL_REG3_A              0x22
#define CTRL_REG4_A              0x23
#define CTRL_REG5_A              0x24
#define CTRL_REG6_A              0x25
#define REFERENCE_A              0x26
#define STATUS_REG_A             0x27
#define OUT_X_L_A                0x28
#define OUT_X_H_A                0x29
#define OUT_Y_L_A                0x2A
#define OUT_Y_H_A                0x2B
#define OUT_Z_L_A                0x2C
#define OUT_Z_H_A                0x2D
#define FIFO_CTRL_REG_A          0x2E
#define FIFO_SRC_REG_A           0x2F
#define INT1_CFG_A               0x30
#define INT1_SRC_A               0x31
#define INT1_THS_A               0x32
#define INT1_DURATION_A          0x33
#define INT2_CFG_A               0x34
#define INT2_SRC_A               0x35
#define INT2_THS_A               0x36
#define INT2_DURATION_A          0x37
#define CLICK_CFG_A              0x38
#define CLICK_SRC_A              0x39
#define CLICK_THR_A              0x3A
#define TIME_LIMIT_A             0x3B
#define TIME_LATENCY_A           0x3C
#define TIME_WINDOW_A            0x3D

#define LSM303DLHC_MAG_ADDRESS   0x3C

#define CRA_REG_M                0x00
#define CRB_REG_M                0x01
#define MR_REG_M                 0x02
#define OUT_X_H_M                0x03
#define OUT_X_L_M                0x04
#define OUT_Z_H_M                0x05
#define OUT_Z_L_M                0x06
#define OUT_Y_H_M                0x07
#define OUT_Y_L_M                0x08
#define SR_REG_M                 0x09
#define IRA_REG_M                0x0A
#define IRB_REG_M                0x0B
#define IRC_REG_M                0x0C
#define TEMP_OUT_H_M             0x31
#define TEMP_OUT_L_M             0x32

#define READ_MULTIPLE_BYTE       0x80

void lsm303dlhc_powerdown(lsm303dlhc_t accmag)
{
    uint8_t buf0[] = {CTRL_REG1_A, 0x00};
    uint8_t buf1[] = {MR_REG_M,    0x03};
    uint8_t buf2[] = {CRA_REG_M,   0x00};
    _lsm303dlhc_t *_accmag = accmag;

    i2c_tx(_accmag->i2c, LSM303DLHC_ACC_ADDRESS, buf0, 2);
    i2c_tx(_accmag->i2c, LSM303DLHC_MAG_ADDRESS, buf1, 2);
    i2c_tx(_accmag->i2c, LSM303DLHC_MAG_ADDRESS, buf2, 2);
}

void lsm303dlhc_set_acc_datarate(lsm303dlhc_t accmag, lsm303dlhc_acc_datarate_t datarate)
{
    uint8_t buf[] = {CTRL_REG1_A, datarate | 0x7};
    _lsm303dlhc_t *_accmag = accmag;

    i2c_tx(_accmag->i2c, LSM303DLHC_ACC_ADDRESS, buf, 2);
}

void lsm303dlhc_set_mag_datarate(lsm303dlhc_t accmag, lsm303dlhc_mag_datarate_t datarate)
{
    uint8_t buf1[] = {MR_REG_M, 0};
    uint8_t buf2[] = {CRA_REG_M, datarate | 0x80};
    _lsm303dlhc_t *_accmag = accmag;

    i2c_tx(_accmag->i2c, LSM303DLHC_MAG_ADDRESS, buf1, 2);
    i2c_tx(_accmag->i2c, LSM303DLHC_MAG_ADDRESS, buf2, 2);
}

void lsm303dlhc_set_acc_scale(lsm303dlhc_t accmag, lsm303dlhc_acc_scale_t scale)
{
    uint8_t buf[2] = {CTRL_REG4_A, scale | 0x88};

    _lsm303dlhc_t *_accmag = accmag;

    i2c_tx(_accmag->i2c, LSM303DLHC_ACC_ADDRESS, buf, 2);
}

void lsm303dlhc_set_mag_scale(lsm303dlhc_t accmag, lsm303dlhc_mag_scale_t scale)
{
    uint8_t buf[] = {CRB_REG_M, scale};
    _lsm303dlhc_t *_accmag = accmag;

    i2c_tx(_accmag->i2c, LSM303DLHC_MAG_ADDRESS, buf, 2);
}

void lsm303dlhc_read_acc(lsm303dlhc_t accmag, int16_t *acc)
{
    uint8_t reg = READ_MULTIPLE_BYTE | OUT_X_L_A;
    _lsm303dlhc_t *_accmag = accmag;

    i2c_tx_rx(_accmag->i2c, LSM303DLHC_ACC_ADDRESS, &reg, 1, (uint8_t *)acc, 6);
    acc[0] >>= 4;
    acc[1] >>= 4;
    acc[2] >>= 4;
}

static inline void swap(int16_t *x)
{
    int16_t t = ((*x) >> 8) & 0xFF;

    *x = t | (*x << 8);
}

void lsm303dlhc_read_mag(lsm303dlhc_t accmag, int16_t *mag)
{
    uint8_t reg = OUT_X_H_M;
    int16_t t;
    _lsm303dlhc_t *_accmag = accmag;

    i2c_tx_rx(_accmag->i2c, LSM303DLHC_MAG_ADDRESS, &reg, 1, (uint8_t *)mag, 6);

    swap(&(mag[0]));
    swap(&(mag[1]));
    swap(&(mag[2]));

    t = mag[2];
    mag[2] = mag[1];
    mag[1] = t;
}

void lsm303dlhc_read_temp(lsm303dlhc_t accmag, int16_t *temp)
{
    uint8_t reg = TEMP_OUT_H_M;
    _lsm303dlhc_t *_accmag = accmag;

    i2c_tx_rx(_accmag->i2c, LSM303DLHC_MAG_ADDRESS, &reg, 1, (uint8_t *)temp, 2);

    swap(temp);
}
