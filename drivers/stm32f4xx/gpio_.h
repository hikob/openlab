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
 * gpio_.h
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef GPIO__H_
#define GPIO__H_

#include "gpio.h"
#include "rcc.h"

typedef struct
{
    uint32_t base_address;
    rcc_ahb_bit_t ahb_bit;
} _gpio_t;

typedef enum
{
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT = 0x1,
    GPIO_MODE_ALTERNATE = 0x2,
    GPIO_MODE_ANALOG = 0x3,
} gpio_pin_mode_t;

typedef enum
{
    GPIO_AF_0 = 0, // SYS
    GPIO_AF_1 = 1, // TIM 1/2
    GPIO_AF_2 = 2, // TIM 3/4/5
    GPIO_AF_3 = 3, // TIM 8/9/10/11
    GPIO_AF_4 = 4, // I2C 1/2/3
    GPIO_AF_5 = 5, // SPI 1/2 I2S 1/2
    GPIO_AF_6 = 6, // SPI 3 I2S 3
    GPIO_AF_7 = 7, // USART 1/2/3
    GPIO_AF_8 = 8, // USART 4/5/6
    GPIO_AF_9 = 9, // CAN 1/2 TIM 12/13/14
    GPIO_AF_10 = 10, // OTG
    GPIO_AF_11 = 11, // ETH
    GPIO_AF_12 = 12, // FSMC SDIO
    GPIO_AF_13 = 13, // DCMI
    GPIO_AF_14 = 14, //
    GPIO_AF_15 = 15, // EVENTOUT
} gpio_af_t;

typedef enum
{
    GPIO_SPEED_VERY_LOW = 0,
    GPIO_SPEED_LOW = 1,
    GPIO_SPEED_MEDIUM = 2,
    GPIO_SPEED_HIGH = 3,
} gpio_speed_t;

static inline void gpio_init(_gpio_t* _gpio, uint32_t base_address,
        rcc_ahb_bit_t ahb_bit)
{
    _gpio->base_address = base_address;
    _gpio->ahb_bit = ahb_bit;
}

#define GPIO_INIT(name, ba, ab) \
        const _gpio_t name = { .base_address = (ba), .ahb_bit = (ab) }


void gpio_set_alternate_function(const _gpio_t *_gpio, gpio_pin_t pin, gpio_af_t af);
void gpio_set_speed(const _gpio_t *_gpio, gpio_pin_t pin, gpio_speed_t speed);

#endif /* GPIO__H_ */
