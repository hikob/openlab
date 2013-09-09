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
 * l3g4200d.c
 *
 *  Created on: Jul 31, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "gpio.h"
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
#define STATUS_REG         0x27
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

static struct
{
    // We need an I2C link
    i2c_t i2c;
    exti_line_t data_ready_line;
    gpio_t drdy_gpio;
    gpio_pin_t drdy_gpio_pin;
} l3g4200d;

uint8_t l3g4200d_config(i2c_t i2c, exti_line_t data_ready_line,
                       gpio_t drdy_gpio,
                       gpio_pin_t drdy_gpio_pin)
{
    l3g4200d.i2c = i2c;
    
    l3g4200d.data_ready_line = data_ready_line;
    l3g4200d.drdy_gpio = drdy_gpio;
    l3g4200d.drdy_gpio_pin = drdy_gpio_pin;

    return l3g4200d_powerdown();
}

bool l3g4200d_read_drdy()
{
    return gpio_pin_read(l3g4200d.drdy_gpio, l3g4200d.drdy_gpio_pin);
}

void l3g4200d_set_drdy_int(handler_t data_ready_handler,
                                 handler_arg_t data_ready_arg)
{
    uint8_t buf[] = {CTRL_REG3, 0x08};

    // Enable interrupt generation if required
    if (data_ready_handler)
    {
        exti_set_handler(l3g4200d.data_ready_line, data_ready_handler, data_ready_arg);
        exti_enable_interrupt_line(l3g4200d.data_ready_line, EXTI_TRIGGER_RISING);
    
        i2c_tx(l3g4200d.i2c, L3G4200D_ADDRESS, buf, 2);
    }
    else
    {
        exti_disable_interrupt_line(l3g4200d.data_ready_line);
    }
}

uint8_t l3g4200d_read_whoami()
{
    uint8_t reg = WHO_AMI;
    uint8_t tmp;

    i2c_tx_rx(l3g4200d.i2c, L3G4200D_ADDRESS, &reg, 1, &tmp, 1);

    return tmp;
}

uint8_t l3g4200d_read_crtl_reg(uint8_t i)
{
    uint8_t reg = CTRL_REG1 + i - 1;
    uint8_t tmp;

    i2c_tx_rx(l3g4200d.i2c, L3G4200D_ADDRESS, &reg, 1, &tmp, 1);

    return tmp;
}

uint8_t l3g4200d_read_status_reg()
{
    uint8_t reg = STATUS_REG;
    uint8_t tmp;

    i2c_tx_rx(l3g4200d.i2c, L3G4200D_ADDRESS, &reg, 1, &tmp, 1);

    return tmp;
}

uint8_t l3g4200d_powerdown()
{
    uint8_t buf[] = {CTRL_REG1, 0x00};

    return i2c_tx(l3g4200d.i2c, L3G4200D_ADDRESS, buf, 2);
}


uint8_t l3g4200d_gyr_config(l3g4200d_datarate_t datarate, l3g4200d_scale_t scale, bool bdu)
{
    uint8_t buf[] = {CTRL_REG1, datarate | 0x3F};

    i2c_tx(l3g4200d.i2c, L3G4200D_ADDRESS, buf, 2);

    buf[0] = CTRL_REG4;
    buf[1] = scale | (bdu ? 0x80 : 0x00);

    return i2c_tx(l3g4200d.i2c, L3G4200D_ADDRESS, buf, 2);
}

unsigned l3g4200d_read_rot_speed_async(int16_t *rot_speed, result_handler_t handler, handler_arg_t arg)
{
    static const uint8_t reg = READ_MULTIPLE_BYTE | OUT_X_L;

    return i2c_tx_rx_async(l3g4200d.i2c, L3G4200D_ADDRESS, &reg, 1, (uint8_t *)rot_speed, 6, handler, arg);
}

uint8_t l3g4200d_read_temp(uint8_t *temp)
{
    uint8_t reg = OUT_TEMP;

    return i2c_tx_rx(l3g4200d.i2c, L3G4200D_ADDRESS, &reg, 1, temp, 1);
}

void l3g4200d_set_bw(uint8_t bw)
{
    uint8_t buf[2];
    buf[0] = CTRL_REG1;

    buf[1] = l3g4200d_read_crtl_reg(1);
    buf[1] = (buf[1] & 0xCF) | (bw & 0x30);

    i2c_tx(l3g4200d.i2c, L3G4200D_ADDRESS, buf, 2);
}

void l3g4200d_set_out(uint8_t out)
{
    uint8_t buf[2];
    buf[0] = CTRL_REG5;

    buf[1] = l3g4200d_read_crtl_reg(5);
    buf[1] = (buf[1] & 0xFC) | (out & 0x03);
    i2c_tx(l3g4200d.i2c, L3G4200D_ADDRESS, buf, 2);
}
