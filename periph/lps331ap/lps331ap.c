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
 * lps331ap.c
 *
 *  Created on: Jan 25, 2012
 *      Author: braillon
 */

#include "i2c.h"
#include "lps331ap.h"
#include "lps331ap_.h"

#define LPS331AP_ADDRESS     0xB8

#define WHO_AMI            0x0F
#define CTRL_REG1          0x20
#define CTRL_REG2          0x21
#define STATUS_REG         0x27
#define PRES_OUT_XL        0x28
#define PRES_OUT_L         0x29
#define PRES_OUT_H         0x2A
#define TEMP_OUT_L         0x2B
#define TEMP_OUT_H         0x2C

#define READ_MULTIPLE_BYTE 0x80

static struct
{
    // We need an I2C link
    i2c_t i2c;

    exti_line_t data_ready_line;
} lps331ap;

uint8_t lps331ap_config(i2c_t i2c, exti_line_t data_ready_line)
{
    uint8_t buf[2];

    lps331ap.i2c = i2c;
    lps331ap.data_ready_line = data_ready_line;

    // Reboot memory content
    buf[0] = CTRL_REG2;
    buf[1] = 0x80;
    
    return i2c_tx(lps331ap.i2c, LPS331AP_ADDRESS, buf, 2) || lps331ap_powerdown();
}

uint8_t lps331ap_read_whoami()
{
	uint8_t reg = WHO_AMI;
	uint8_t tmp;

	return i2c_tx_rx(lps331ap.i2c, LPS331AP_ADDRESS, &reg, 1, &tmp, 1) ? 0xFF : tmp;
}

uint8_t lps331ap_read_crtl_reg(uint8_t i)
{
	uint8_t reg = CTRL_REG1 + i - 1;
	uint8_t tmp;

	return i2c_tx_rx(lps331ap.i2c, LPS331AP_ADDRESS, &reg, 1, &tmp, 1) ? 0xFF : tmp;
}

uint8_t lps331ap_powerdown()
{
	uint8_t buf[] = {CTRL_REG1, 0x00};

    return i2c_tx(lps331ap.i2c, LPS331AP_ADDRESS, buf, 2);
}

uint8_t lps331ap_set_datarate(lps331ap_datarate_t datarate)
{
	uint8_t buf[] = {CTRL_REG1, datarate | 0x84};

    return i2c_tx(lps331ap.i2c, LPS331AP_ADDRESS, buf, 2);
}

uint8_t lps331ap_read_pres(uint32_t *p)
{
	uint8_t reg = READ_MULTIPLE_BYTE | PRES_OUT_XL;
	
    *p = 0;
	return i2c_tx_rx(lps331ap.i2c, LPS331AP_ADDRESS, &reg, 1, (uint8_t*)p, 3);
}

uint8_t lps331ap_read_temp(int16_t *temp)
{
	uint8_t reg = READ_MULTIPLE_BYTE | TEMP_OUT_L;

	return i2c_tx_rx(lps331ap.i2c, LPS331AP_ADDRESS, &reg, 1, (uint8_t*)temp, 2);
}

void lps331ap_set_drdy_int(handler_t data_ready_handler, handler_arg_t data_ready_arg)
{
    if(data_ready_handler)
    {
        exti_set_handler(lps331ap.data_ready_line, data_ready_handler, data_ready_arg);
        exti_enable_interrupt_line(lps331ap.data_ready_line, EXTI_TRIGGER_RISING);
    }
    else
    {
        exti_disable_interrupt_line(lps331ap.data_ready_line);
    }
}
