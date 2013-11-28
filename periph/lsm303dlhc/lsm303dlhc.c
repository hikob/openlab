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

/**
 * @file lsm303dlhc.c
 * @date Aug 3, 2011
 * @author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "gpio.h"
#include "i2c.h"
#include "lsm303dlhc.h"
#include "lsm303dlhc_.h"
#include "platform.h"
#include "debug.h"

enum
{
    LSM303DLHC_ACC_ADDRESS = 0x32,
    LSM303DLHC_MAG_ADDRESS = 0x3c,
    LSM303DLHC_ACC_MULTIBYTES = 0x80
};

static struct
{
    i2c_t i2c;
    exti_line_t mag_data_ready_line;
    openlab_timer_t acc_int1_timer;
    union {
        exti_line_t line;
        timer_channel_t channel;
    } acc_int1;
    exti_line_t acc_int2_line;
    gpio_t mag_int_gpio;
    gpio_t acc_int1_gpio;
    gpio_t acc_int2_gpio;
    gpio_pin_t mag_int_pin;
    gpio_pin_t acc_int1_pin;
    gpio_pin_t acc_int2_pin;
    lsm303dlhc_mag_scale_t scale;
} lsm303;

uint8_t lsm303dlhc_config(i2c_t i2c,
                       exti_line_t mag_data_ready_line,
                       gpio_t mag_drdy_gpio, gpio_pin_t mag_drdy_pin,
                       exti_line_t acc_int1_line,
                       gpio_t acc_int1_gpio, gpio_pin_t acc_int1_pin,
                       exti_line_t acc_int2_line,
                       gpio_t acc_int2_gpio, gpio_pin_t acc_int2_pin)
{
    uint8_t buf[2], r;

    // Store the values
    lsm303.i2c = i2c;
    lsm303.mag_data_ready_line = mag_data_ready_line;
    lsm303.acc_int1_timer = NULL;
    lsm303.acc_int1.line = acc_int1_line;
    lsm303.acc_int2_line = acc_int2_line;

    lsm303.mag_int_gpio = mag_drdy_gpio;
    lsm303.mag_int_pin = mag_drdy_pin;

    lsm303.acc_int1_gpio = acc_int1_gpio;
    lsm303.acc_int2_gpio = acc_int2_gpio;
    lsm303.acc_int1_pin = acc_int1_pin;
    lsm303.acc_int2_pin = acc_int2_pin;
    lsm303.scale = LSM303DLHC_MAG_SCALE_1_3GAUSS;

    // Reboot memory content
    buf[0] = LSM303DLHC_REG_CTRL_REG5_A;
    buf[1] = 0x80;
    r = i2c_tx(lsm303.i2c, LSM303DLHC_ACC_ADDRESS, buf, 2);

    // Set powerdown
    return r || lsm303dlhc_powerdown();
}

void lsm303dlhc_config_acc_int1_uses_timer(openlab_timer_t timer, timer_channel_t channel)
{
    lsm303.acc_int1_timer = timer;
    lsm303.acc_int1.channel = channel;
}

uint8_t lsm303dlhc_powerdown()
{
    uint8_t buf[2], r;

    // Stop accelero
    buf[0] = LSM303DLHC_REG_CTRL_REG1_A;
    buf[1] = 0x00;
    r = i2c_tx(lsm303.i2c, LSM303DLHC_ACC_ADDRESS, buf, 2);

    // Stop magneto
    buf[0] = LSM303DLHC_REG_MR_REG_M;
    buf[1] = LSM303DLHC_MAG_MODE_OFF;
    r = r || i2c_tx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, buf, 2);

    // Stop temperature sensor
    buf[0] = LSM303DLHC_REG_CRA_REG_M;
    buf[1] = 0x00;
    return r || i2c_tx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, buf, 2);
}

unsigned lsm303dlhc_mag_reg_read(uint8_t reg, uint8_t *val)
{
    return i2c_tx_rx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, &reg, 1, val, 1);
}

unsigned lsm303dlhc_mag_reg_write(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg, val};
    return i2c_tx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, buf, 2);
}

unsigned lsm303dlhc_acc_reg_read(uint8_t reg, uint8_t *val)
{
    return i2c_tx_rx(lsm303.i2c, LSM303DLHC_ACC_ADDRESS, &reg, 1, val, 1);
}

unsigned lsm303dlhc_acc_reg_write(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg, val};
    return i2c_tx(lsm303.i2c, LSM303DLHC_ACC_ADDRESS, buf, 2);
}

uint8_t lsm303dlhc_mag_config(lsm303dlhc_mag_datarate_t datarate,
                           lsm303dlhc_mag_scale_t scale, lsm303dlhc_mag_mode_t mode,
                           lsm303dlhc_temp_mode_t temp_mode)
{
    uint8_t buf[2], r;

    // Set MAG datarate and temperature sensor
    buf[0] = LSM303DLHC_REG_CRA_REG_M;
    buf[1] = datarate | temp_mode;
    r = i2c_tx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, buf, 2);

    // Set MAG scale
    buf[0] = LSM303DLHC_REG_CRB_REG_M;
    buf[1] = scale;
    r = r || i2c_tx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, buf, 2);
    lsm303.scale = scale;

    // Set MAG mode
    buf[0] = LSM303DLHC_REG_MR_REG_M;
    buf[1] = mode;
    return r || i2c_tx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, buf, 2);
}

int lsm303dlhc_mag_get_drdy_pin_value()
{
    return gpio_pin_read(lsm303.mag_int_gpio, lsm303.mag_int_pin);
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

uint8_t lsm303dlhc_mag_sample_single()
{
    uint8_t buf[2];

    // Set MAG mode to single
    buf[0] = LSM303DLHC_REG_MR_REG_M;
    buf[1] = LSM303DLHC_MAG_MODE_SINGLE;
    return i2c_tx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, buf, 2);
}

uint8_t lsm303dlhc_acc_config(lsm303dlhc_acc_datarate_t datarate,
        lsm303dlhc_acc_scale_t scale, lsm303dlhc_acc_update_t update)
{
    uint8_t buf[2], r;

    // Set the data rate
    buf[0] = LSM303DLHC_REG_CTRL_REG1_A;
    buf[1] = datarate | 0x7;
    r = i2c_tx(lsm303.i2c, LSM303DLHC_ACC_ADDRESS, buf, 2);

    // Set the scale
    buf[0] = LSM303DLHC_REG_CTRL_REG4_A;
    buf[1] = scale | update | 0x08;
    return r || i2c_tx(lsm303.i2c, LSM303DLHC_ACC_ADDRESS, buf, 2);
}

void lsm303dlhc_acc_set_drdy_int1(handler_t data_ready_handler,
                                 handler_arg_t data_ready_arg)
{
    uint8_t buf[2];

    // Enable interrupt generation if required
    if (data_ready_handler)
    {
        if (lsm303.acc_int1_timer)
        {
            timer_set_channel_capture(lsm303.acc_int1_timer, lsm303.acc_int1.channel,
                    TIMER_CAPTURE_EDGE_RISING, (timer_handler_t) data_ready_handler, NULL);
        }
        else
        {
            exti_set_handler(lsm303.acc_int1.line, data_ready_handler, data_ready_arg);
            exti_enable_interrupt_line(lsm303.acc_int1.line, EXTI_TRIGGER_RISING);
        }

        buf[0] = LSM303DLHC_REG_CTRL_REG3_A;
        buf[1] = 0x10;
        i2c_tx(lsm303.i2c, LSM303DLHC_ACC_ADDRESS, buf, 2);
    }
    else
    {
        buf[0] = LSM303DLHC_REG_CTRL_REG3_A;
        buf[1] = 0x00;
        i2c_tx(lsm303.i2c, LSM303DLHC_ACC_ADDRESS, buf, 2);

        if (lsm303.acc_int1_timer)
        {
            timer_set_channel_capture(lsm303.acc_int1_timer, lsm303.acc_int1.channel,
                    TIMER_CAPTURE_EDGE_RISING, NULL, NULL);
        }
        else
        {
            exti_disable_interrupt_line(lsm303.acc_int1.line);
        }
    }
}

int lsm303dlhc_acc_get_drdy_int1_pin_value()
{
    return gpio_pin_read(lsm303.acc_int1_gpio, lsm303.acc_int1_pin);
}

void lsm303dlhc_acc_set_drdy_int2(handler_t data_ready_handler,
                                 handler_arg_t data_ready_arg)
{
    // Enable interrupt generation if required
    if (data_ready_handler)
    {
        exti_set_handler(lsm303.acc_int2_line, data_ready_handler, data_ready_arg);
        exti_enable_interrupt_line(lsm303.acc_int2_line, EXTI_TRIGGER_RISING);
    }
    else
    {
        exti_disable_interrupt_line(lsm303.acc_int2_line);
    }
}

static inline void acc_finalize(int16_t *acc)
{
    acc[0] >>= 4;
    acc[1] >>= 4;
    acc[2] >>= 4;
}

void lsm303dlhc_read_acc_async_finalize(int16_t *acc)
{
    acc_finalize(acc);
}

unsigned lsm303dlhc_read_acc_async(int16_t *acc, result_handler_t handler, handler_arg_t handler_arg)
{
    static const uint8_t reg = LSM303DLHC_ACC_MULTIBYTES | LSM303DLHC_REG_OUT_X_L_A;
    unsigned res;

    // Do the transfer
    res = i2c_tx_rx_async(lsm303.i2c, LSM303DLHC_ACC_ADDRESS,
            &reg, 1, (uint8_t *) acc, 6, handler, handler_arg);

    // Finalize data if blocking transfer
    if (handler == NULL)
    {
        acc_finalize(acc);
    }

    return res;
}

static inline void swap(int16_t *x)
{
    int16_t t = ((*x) >> 8) & 0xFF;

    *x = t | (*x << 8);
}

void lsm303dlhc_read_mag_async_finalize(int16_t *mag)
{
    int16_t t;
    // Swap them to fit the host endianness (little endian)
    swap(&(mag[0]));
    swap(&(mag[1]));
    swap(&(mag[2]));

    // Place them in order (X, Y, Z)
    t = mag[2];
    mag[2] = mag[1];
    mag[1] = t;

    // Compensate for Z different sensitivity
    switch(lsm303.scale)
    {
        case LSM303DLHC_MAG_SCALE_1_3GAUSS:
            mag[2] = (((int32_t) mag[2]) * 1100) / 980;
            break;

        case LSM303DLHC_MAG_SCALE_1_9GAUSS:
            mag[2] = (((int32_t) mag[2]) * 855) / 760;
            break;

        case LSM303DLHC_MAG_SCALE_2_5GAUSS:
            mag[2] = (((int32_t) mag[2]) * 670) / 600;
            break;

        case LSM303DLHC_MAG_SCALE_4_0GAUSS:
            mag[2] = (((int32_t) mag[2]) * 450) / 400;
            break;

        case LSM303DLHC_MAG_SCALE_4_7GAUSS:
            mag[2] = (((int32_t) mag[2]) * 400) / 355;
            break;

        case LSM303DLHC_MAG_SCALE_5_6GAUSS:
            mag[2] = (((int32_t) mag[2]) * 330) / 295;
            break;

        case LSM303DLHC_MAG_SCALE_8_1GAUSS:
            mag[2] = (((int32_t) mag[2]) * 230) / 205;
            break;
    }
}

unsigned lsm303dlhc_read_mag_async(int16_t *mag, result_handler_t handler, handler_arg_t handler_arg)
{
    static const uint8_t reg = LSM303DLHC_REG_OUT_X_H_M;
    unsigned res;

    // Do the transfer
    res = i2c_tx_rx_async(lsm303.i2c, LSM303DLHC_MAG_ADDRESS,
            &reg, 1, (uint8_t *) mag, 6, handler, handler_arg);

    // If blocking, finalize the array
    if (handler == NULL)
    {
        lsm303dlhc_read_mag_async_finalize(mag);
    }

    return res;
}

uint8_t lsm303dlhc_read_temp(int16_t *temp)
{
    uint8_t r, reg = LSM303DLHC_REG_TEMP_OUT_H_M;

    r = i2c_tx_rx(lsm303.i2c, LSM303DLHC_MAG_ADDRESS, &reg, 1, (uint8_t *) temp, 2);

    swap(temp);

    return r;
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
