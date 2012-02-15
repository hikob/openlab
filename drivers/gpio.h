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
 * gpio.h
 *
 *  Created on: Jul 8, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

/*
 * GPIO driver header file.
 *
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>
typedef void *gpio_t;

typedef enum
{
    GPIO_PIN_0  = 0,
    GPIO_PIN_1  = 1,
    GPIO_PIN_2  = 2,
    GPIO_PIN_3  = 3,
    GPIO_PIN_4  = 4,
    GPIO_PIN_5  = 5,
    GPIO_PIN_6  = 6,
    GPIO_PIN_7  = 7,
    GPIO_PIN_8  = 8,
    GPIO_PIN_9  = 9,
    GPIO_PIN_10 = 10,
    GPIO_PIN_11 = 11,
    GPIO_PIN_12 = 12,
    GPIO_PIN_13 = 13,
    GPIO_PIN_14 = 14,
    GPIO_PIN_15 = 15
} gpio_pin_t;

typedef enum
{
    GPIO_TYPE_PUSH_PULL  = 0,
    GPIO_TYPE_OPEN_DRAIN = 1
} gpio_type_t;

typedef enum
{
    GPIO_PULL_DISABLED = 0,
    GPIO_PULL_UP       = 1,
    GPIO_PULL_DOWN     = 2
} gpio_pull_up_down_t;

void gpio_enable(gpio_t gpio);
void gpio_disable(gpio_t gpio);

void gpio_lp_ensable(gpio_t gpio);
void gpio_lp_disable(gpio_t gpio);

void gpio_set_input(gpio_t gpio, gpio_pin_t pin);
void gpio_set_output(gpio_t gpio, gpio_pin_t pin);
void gpio_set_analog(gpio_t gpio, gpio_pin_t pin);

void gpio_config_output_type(gpio_t gpio, gpio_pin_t pin, gpio_type_t type);
void gpio_config_pull_up_down(gpio_t gpio, gpio_pin_t pin, gpio_pull_up_down_t mode);

void gpio_pin_set(gpio_t gpio, gpio_pin_t pin);
void gpio_pin_clear(gpio_t gpio, gpio_pin_t pin);
void gpio_pin_toggle(gpio_t gpio, gpio_pin_t pin);

uint32_t gpio_pin_read(gpio_t gpio, gpio_pin_t pin);

// Specific Configurations
void gpio_set_spi_clk(gpio_t gpio, gpio_pin_t pin);
void gpio_set_spi_miso(gpio_t gpio, gpio_pin_t pin);
void gpio_set_spi_mosi(gpio_t gpio, gpio_pin_t pin);

void gpio_set_uart_rx(gpio_t gpio, gpio_pin_t pin);
void gpio_set_uart_tx(gpio_t gpio, gpio_pin_t pin);

void gpio_set_i2c_sda(gpio_t gpio, gpio_pin_t pin);
void gpio_set_i2c_scl(gpio_t gpio, gpio_pin_t pin);

#endif /* GPIO_H_ */
