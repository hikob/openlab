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

static struct
{
    i2c_t i2c;
    exti_line_t mag_data_ready_line;
    exti_line_t acc_int1_line;
    exti_line_t acc_int2_line;
} lsm303;

void lsm303dlhc_config(i2c_t i2c, exti_line_t mag_data_ready_line,
                       exti_line_t acc_int1_line, exti_line_t acc_int2_line)
{
    // Store the values
    lsm303.i2c = i2c;
    lsm303.mag_data_ready_line = mag_data_ready_line;
    lsm303.acc_int1_line = acc_int1_line;
    lsm303.acc_int2_line = acc_int2_line;

    // Reboot memory content
    uint8_t buf[2];
    buf[0] = CTRL_REG5_A;
    buf[1] = 0x80;
    i2c_tx(lsm303.i2c, LSM303DLHC_ACC_ADDRESS, buf, 2);

    // Set powerdown
    lsm303dlhc_powerdown();
}

void lsm303dlhc_powerdown()
{
    // Stop accelero
    uint8_t buf[2];
    buf[0] = CTRL_REG1_A;
    buf[1] = 0x00;
    i2c_tx(lsm303.i2c, LSM303DLHC_ACC_ADDRESS, buf, 2);

    // Stop magneto
    buf[0] = MR_REG_M;
    buf[1] = 0x03;
    i2c_tx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, buf, 2);

    // Stop temperature sensor
    buf[0] = CRA_REG_M;
    buf[1] = 0x00;
    i2c_tx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, buf, 2);
}

void lsm303dlhc_mag_config(lsm303dlhc_mag_datarate_t datarate,
                           lsm303dlhc_mag_scale_t scale, lsm303dlhc_mag_mode_t mode,
                           lsm303dlhc_temp_mode_t temp_mode)
{
    uint8_t buf[2];

    // Set MAG datarate and temperature sensor
    buf[0] = CRA_REG_M;
    buf[1] = datarate | temp_mode;
    i2c_tx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, buf, 2);

    // Set MAG scale
    buf[0] = CRB_REG_M;
    buf[1] = scale;
    i2c_tx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, buf, 2);

    // Set MAG mode
    buf[0] = MR_REG_M;
    buf[1] = mode;
    i2c_tx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, buf, 2);
}

void lsm303dlhc_mag_set_drdy_int(handler_t data_ready_handler,
                                 handler_arg_t data_ready_arg)
{
    // Enable interrupt generation if required
    if (data_ready_handler)
    {
        exti_set_handler(lsm303.mag_data_ready_line, data_ready_handler,
                         data_ready_arg);
        exti_enable_interrupt_line(lsm303.mag_data_ready_line,
                                   EXTI_TRIGGER_RISING);
    }
    else
    {
        exti_disable_interrupt_line(lsm303.mag_data_ready_line);
    }
}

void lsm303dlhc_mag_sample_single()
{
    uint8_t buf[2];

    // Set MAG mode to single
    buf[0] = MR_REG_M;
    buf[1] = 0x01;
    i2c_tx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, buf, 2);
}

void lsm303dlhc_acc_config(lsm303dlhc_acc_datarate_t datarate,
                           lsm303dlhc_acc_scale_t scale)
{
    uint8_t buf[2];

    // Set the data rate
    buf[0] = CTRL_REG1_A;
    buf[1] = datarate | 0x7;
    i2c_tx(lsm303.i2c, LSM303DLHC_ACC_ADDRESS, buf, 2);

    // Set the scale
    buf[0] = CTRL_REG4_A;
    buf[1] = scale | 0x88;
    i2c_tx(lsm303.i2c, LSM303DLHC_ACC_ADDRESS, buf, 2);

    // Enable data ready interrupt
    buf[0] = CTRL_REG3_A;
    buf[1] = 0x18;
    i2c_tx(lsm303.i2c, LSM303DLHC_ACC_ADDRESS, buf, 2);
}

void lsm303dlhc_acc_set_drdy_int(handler_t data_ready_handler,
                                 handler_arg_t data_ready_arg)
{
    // Enable interrupt generation if required
    if (data_ready_handler)
    {
        exti_set_handler(lsm303.acc_int1_line, data_ready_handler,
                         data_ready_arg);
        exti_enable_interrupt_line(lsm303.acc_int1_line, EXTI_TRIGGER_RISING);
    }
    else
    {
        exti_disable_interrupt_line(lsm303.acc_int1_line);
    }
}

void lsm303dlhc_read_acc(int16_t *acc)
{
    uint8_t reg = READ_MULTIPLE_BYTE | OUT_X_L_A;

    i2c_tx_rx(lsm303.i2c, LSM303DLHC_ACC_ADDRESS, &reg, 1, (uint8_t *) acc, 6);
    acc[0] >>= 4;
    acc[1] >>= 4;
    acc[2] >>= 4;
}

static inline void swap(int16_t *x)
{
    int16_t t = ((*x) >> 8) & 0xFF;

    *x = t | (*x << 8);
}

void lsm303dlhc_read_mag(int16_t *mag)
{
    uint8_t reg = OUT_X_H_M;
    int16_t t;

    // Read the 6 bytes for the 3 values
    i2c_tx_rx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, &reg, 1, (uint8_t *) mag, 6);

    // Swap them to fit the host endianness (little endian)
    swap(&(mag[0]));
    swap(&(mag[1]));
    swap(&(mag[2]));

    // Place them in order (X, Y, Z)
    t = mag[2];
    mag[2] = mag[1];
    mag[1] = t;

    // Compensate for Z different sensitivity
    mag[2] *= 670. / 600;
}

void lsm303dlhc_read_temp(int16_t *temp)
{
    uint8_t reg = TEMP_OUT_H_M;

    i2c_tx_rx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, &reg, 1, (uint8_t *) temp, 2);

    swap(temp);
}

lsm303dlhc_mag_datarate_t lsm303dlhc_mag_compute_datarate(uint32_t freq)
{
    if (freq <= 1)
    {
        return LSM303DLHC_MAG_RATE_1_5HZ;
    }
    else if (freq <= 3)
    {
        return LSM303DLHC_MAG_RATE_3HZ;
    }
    else if (freq <= 7)
    {
        return LSM303DLHC_MAG_RATE_7_5HZ;
    }
    else if (freq <= 15)
    {
        return LSM303DLHC_MAG_RATE_15HZ;
    }
    else if (freq <= 30)
    {
        return LSM303DLHC_MAG_RATE_30HZ;
    }
    else if (freq <= 75)
    {
        return LSM303DLHC_MAG_RATE_75HZ;
    }

    // All other cases, return max
    return LSM303DLHC_MAG_RATE_220HZ;
}
