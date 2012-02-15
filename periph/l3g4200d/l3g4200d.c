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
 * l3g4200d.c
 *
 *  Created on: Jul 31, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "i2c.h"
#include "l3g4200d.h"
#include "l3g4200d_.h"

#define L3G4200D_ADDRESS   0xD0

#define WHO_AMI            0x0F
#define CTRL_REG1          0x20
#define CTRL_REG2          0x21
#define CTRL_REG3          0x22
#define CTRL_REG4          0x23
#define CTRL_REG5          0x24
#define REFERENCE          0x25
#define OUT_TEMP           0x26
#define OUT_X_L            0x28
#define OUT_X_H            0x29
#define OUT_Y_L            0x2A
#define OUT_Y_H            0x2B
#define OUT_Z_L            0x2C
#define OUT_Z_H            0x2D
#define FIFO_CTRL_REG      0x2E
#define FIFO_SRC_REG       0x2F
#define INT1_CFG           0x30
#define INT1_SRC           0x31
#define INT1_THS_XH        0x32
#define INT1_THS_XL        0x33
#define INT1_THS_YH        0x34
#define INT1_THS_YL        0x35
#define INT1_THS_ZH        0x36
#define INT1_THS_ZL        0x37
#define INT1_DURATION      0x38

#define READ_MULTIPLE_BYTE 0x80

uint8_t l3g4200d_read_whoami(l3g4200d_t gyro)
{
    uint8_t reg = WHO_AMI;
    uint8_t tmp;
    _l3g4200d_t *_gyro = gyro;

    i2c_tx_rx(_gyro->i2c, L3G4200D_ADDRESS, &reg, 1, &tmp, 1);

    return tmp;
}

uint8_t l3g4200d_read_crtl_reg(l3g4200d_t gyro, uint8_t i)
{
    uint8_t reg = CTRL_REG1 + i - 1;
    uint8_t tmp;
    _l3g4200d_t *_gyro = gyro;

    i2c_tx_rx(_gyro->i2c, L3G4200D_ADDRESS, &reg, 1, &tmp, 1);

    return tmp;
}

void l3g4200d_powerdown(l3g4200d_t gyro)
{
    uint8_t buf[] = {CTRL_REG1, 0x00};
    _l3g4200d_t *_gyro = gyro;

    i2c_tx(_gyro->i2c, L3G4200D_ADDRESS, buf, 2);
}

void l3g4200d_set_datarate(l3g4200d_t gyro, l3g4200d_datarate_t datarate)
{
    uint8_t buf[] = {CTRL_REG1, datarate | 0x3F};
    _l3g4200d_t *_gyro = gyro;

    i2c_tx(_gyro->i2c, L3G4200D_ADDRESS, buf, 2);
}

void l3g4200d_set_scale(l3g4200d_t gyro, l3g4200d_scale_t scale)
{
    uint8_t buf[] = {CTRL_REG4, scale};
    _l3g4200d_t *_gyro = gyro;

    i2c_tx(_gyro->i2c, L3G4200D_ADDRESS, buf, 2);
}

void l3g4200d_read_rot_speed(l3g4200d_t gyro, int16_t *rot_speed)
{
    uint8_t reg = READ_MULTIPLE_BYTE | OUT_X_L;
    _l3g4200d_t *_gyro = gyro;

    i2c_tx_rx(_gyro->i2c, L3G4200D_ADDRESS, &reg, 1, (uint8_t *)rot_speed, 6);
}

void l3g4200d_read_temp(l3g4200d_t gyro, uint8_t *temp)
{
    uint8_t reg = OUT_TEMP;
    _l3g4200d_t *_gyro = gyro;

    i2c_tx_rx(_gyro->i2c, L3G4200D_ADDRESS, &reg, 1, temp, 1);
}
