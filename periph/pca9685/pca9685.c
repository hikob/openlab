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
 * pca9685.c
 *
 *  Created on: Mar 30, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "pca9685_.h"

#include "printf.h"
#include "debug.h"

enum
{
    REG_MODE1 = 0x00,
    REG_MODE2 = 0x01,
    REG_SUBADR1 = 0x02,
    REG_SUBADR2 = 0x03,
    REG_SUBADR3 = 0x04,
    REG_ALLCALLADR = 0x05,

    REG_LED0_ON_L = 0x06,
    REG_LED0_ON_H = 0x07,
    REG_LED0_OFF_L = 0x08,
    REG_LED0_OFF_H = 0x09,

    REG_ALL_LED_ON_L = 0xFA,
    REG_ALL_LED_ON_H = 0xFB,
    REG_ALL_LED_OFF_L = 0xFC,
    REG_ALL_LED_OFF_H = 0xFD,

    REG_PRE_SCALE = 0xFE,
};

enum
{
    REG_MODE1__RESTART = 0x80,
    REG_MODE1__EXTCLK = 0x40,
    REG_MODE1__AI = 0x20,
    REG_MODE1__SLEEP = 0x10,
    REG_MODE1__SUB1 = 0x08,
    REG_MODE1__SUB2 = 0x04,
    REG_MODE1__SUB3 = 0x02,
    REG_MODE1__ALLCALL = 0x01,
};

enum
{
    REG_MODE2__INVRT = 0x10,
    REG_MODE2__OCH = 0x08,
    REG_MODE2__OUTDRV = 0x04,
    REG_MODE2__OUTNE_MASK = 0x03,
};

static uint8_t get_led_on_l(uint8_t led)
{
    return REG_LED0_ON_L + (4 * led);
}

#if 0

/*
 * commented out functions to fix defined but not used warnings
 *
 */ 

static uint8_t get_led_on_h(uint8_t led)
{
    return REG_LED0_ON_H + (4 * led);
}
static uint8_t get_led_off_l(uint8_t led)
{
    return REG_LED0_OFF_L + (4 * led);
}
static uint8_t get_led_off_h(uint8_t led)
{
    return REG_LED0_OFF_H + (4 * led);
}
#endif

static struct
{
    _pca9685_t *first;
    uint32_t num;
} pca_config;

void pca9685_init(_pca9685_t *first_pca, uint32_t num_pca)
{
    pca_config.first = first_pca;
    pca_config.num = num_pca;

    // Send 0x6 to SWRST ADDRESS to reset to default values
    uint32_t i;

    for (i = 0; i < num_pca; i++)
    {
        uint8_t value = 0x6;
        i2c_tx(first_pca[i].i2c, 0x0, &value, 1);
    }
}
uint32_t pca9685_get_number()
{
    return pca_config.num;
}

pca9685_t pca9685_get(uint32_t index)
{
    return index >= pca_config.num ? NULL : pca_config.first + index;
}

void pca9685_enable(pca9685_t pca)
{
    _pca9685_t *_pca = pca;

    // Clear SLEEP mode from MODE1
    uint8_t buf[2];
    buf[0] = REG_MODE1;
    buf[1] = REG_MODE1__AI + REG_MODE1__ALLCALL;
    i2c_tx(_pca->i2c, _pca->addr, buf, 2);

    // Set INVRT=1 OUTDRV=0 in MODE2
    buf[0] = REG_MODE2;
    buf[1] = REG_MODE2__INVRT;
    i2c_tx(_pca->i2c, _pca->addr, buf, 2);
}

void pca9685_set_led(pca9685_t pca, uint8_t led, uint16_t value)
{
    _pca9685_t *_pca = pca;

    uint8_t buf[5];

    // Get first register
    buf[0] = get_led_on_l(led & 0xF);

    // Set ON
    if (value >= 4096)
    {
        buf[1] = 0;
        buf[2] = 0x10;
    }
    else
    {
        buf[1] = 0;
        buf[2] = 0;
    }

    // Set OFF
    if (value == 0)
    {
        buf[3] = 0;
        buf[4] = 0x10;
    }
    else
    {
        buf[3] = value & 0xFF;
        buf[4] = (value >> 8) & 0x0F;
    }

    // Apply
    i2c_tx(_pca->i2c, _pca->addr, buf, 5);
}

void pca9685_set_all_leds(pca9685_t pca, uint16_t value)
{
    _pca9685_t *_pca = pca;

    uint8_t buf[5];

    // Get ALL LEDS first register
    buf[0] = REG_ALL_LED_ON_L;

    // Set ON
    if (value >= 4096)
    {
        buf[1] = 0;
        buf[2] = 0x10;
    }
    else
    {
        buf[1] = 0;
        buf[2] = 0;
    }

    // Set OFF
    if (value == 0)
    {
        buf[3] = 0;
        buf[4] = 0x10;
    }
    else
    {
        buf[3] = value & 0xFF;
        buf[4] = (value >> 8) & 0x0F;
    }

    // Apply
    i2c_tx(_pca->i2c, _pca->addr, buf, 5);
}

