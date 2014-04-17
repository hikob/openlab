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
 * gpio.c
 *
 *  Created on: Jul 12, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "gpio.h"
#include "gpio_.h"
#include "gpio_registers.h"

#include "rcc.h"

void gpio_enable(gpio_t gpio)
{
    const _gpio_t *_gpio = gpio;

    // Set the GPIOx_EN bit in the APB2 clock enable register
    rcc_apb_enable(RCC_APB2, _gpio->apb2_bit);
}

void gpio_disable(gpio_t gpio)
{
    const _gpio_t *_gpio = gpio;

    // Clear the GPIOx_EN bit in the APB2 clock enable register
    rcc_apb_disable(RCC_APB2, _gpio->apb2_bit);
}

inline static void gpio_set_mode(gpio_t gpio, gpio_pin_t pin, uint32_t mode)
{
    uint32_t res;

    if (pin < 8)
    {
        res = *gpio_get_CRL(gpio);
        res &= ~(0xF << (pin << 2));
        *gpio_get_CRL(gpio) = res | (mode << (pin << 2));
    }
    else
    {
        res = *gpio_get_CRH(gpio);
        res &= ~(0xF << ((pin - 8) << 2));
        *gpio_get_CRH(gpio) = res | (mode << ((pin - 8) << 2));
    }
}

void gpio_set_input(gpio_t gpio, gpio_pin_t pin)
{
    // Input Float
    gpio_set_mode(gpio, pin, 0x4);
}

void gpio_set_output(gpio_t gpio, gpio_pin_t pin)
{
    // Output Push-pull 50MHz
    gpio_set_mode(gpio, pin, 0x3);
}

void gpio_set_analog(gpio_t gpio, gpio_pin_t pin)
{
    // Analog
    gpio_set_mode(gpio, pin, 0x0);
}

void gpio_set_alternate_function(const _gpio_t *_gpio, gpio_pin_t pin)
{
    uint32_t res;

    // Make sure to enable AFIO in the RCC
    rcc_apb_enable(RCC_APB2, RCC_APB_BIT_AFIO);

    if (pin < 8)
    {
        res = *gpio_get_CRL(_gpio);
        res |= (0x8 << (pin << 2));
        *gpio_get_CRL(_gpio) = res;
    }
    else
    {
        res = *gpio_get_CRH(_gpio);
        res |= (0x8 << ((pin - 8) << 2));
        *gpio_get_CRH(_gpio) = res;
    }
}

void gpio_config_output_type(gpio_t gpio, gpio_pin_t pin, gpio_type_t type)
{
    uint32_t res;

    if (pin < 8)
    {
        res = *gpio_get_CRL(gpio);

        if (type)
        {
            res |= (0x4 << (pin << 2));
        }
        else
        {
            res &= ~(0x4 << (pin << 2));
        }

        *gpio_get_CRL(gpio) = res;
    }
    else
    {
        res = *gpio_get_CRH(gpio);

        if (type)
        {
            res |= (0x4 << ((pin - 8) << 2));
        }
        else
        {
            res &= ~(0x4 << ((pin - 8) << 2));
        }

        *gpio_get_CRH(gpio) = res;
    }
}

void gpio_config_pull_up_down(gpio_t gpio, gpio_pin_t pin, gpio_pull_up_down_t mode)
{
    switch (mode)
    {
        case GPIO_PULL_DISABLED:
            gpio_set_mode(gpio, pin, 0x4);
            break;

        case GPIO_PULL_UP:
            gpio_set_mode(gpio, pin, 0x8);
            *gpio_get_ODR(gpio) |= 1 << pin;
            break;

        case GPIO_PULL_DOWN:
            gpio_set_mode(gpio, pin, 0x8);
            *gpio_get_ODR(gpio) &= ~(1 << pin);
            break;
    }
}

void gpio_pin_set(gpio_t gpio, gpio_pin_t pin)
{
    // To set a pin, write 1 to BSy in GPIOxBSRR
    *gpio_get_BSRR(gpio) = BV(pin);
}

void gpio_pin_clear(gpio_t gpio, gpio_pin_t pin)
{
    // To reset a pin, write 1 to BRy in GPIOxBSRR
    *gpio_get_BSRR(gpio) = BV(pin + 16);
}

void gpio_pin_toggle(gpio_t gpio, gpio_pin_t pin)
{
    // Toggle the pin in the ODR register
    *gpio_get_ODR(gpio) ^= BV(pin);
}

uint32_t gpio_pin_read(gpio_t gpio, gpio_pin_t pin)
{
    // Read the pin value in IDR
    return *gpio_get_IDR(gpio) & BV(pin);
}


void gpio_set_spi_clk(gpio_t gpio, gpio_pin_t pin)
{
    // Set output alternate
    gpio_set_output(gpio, pin);
    gpio_config_output_type(gpio, pin, GPIO_TYPE_PUSH_PULL);
    gpio_set_alternate_function(gpio, pin);
}
void gpio_set_spi_miso(gpio_t gpio, gpio_pin_t pin)
{
    // Just set input
    gpio_set_input(gpio, pin);
}
void gpio_set_spi_mosi(gpio_t gpio, gpio_pin_t pin)
{
    // Set output alternate
    gpio_set_output(gpio, pin);
    gpio_config_output_type(gpio, pin, GPIO_TYPE_PUSH_PULL);
    gpio_set_alternate_function(gpio, pin);
}

void gpio_set_uart_rx(gpio_t gpio, gpio_pin_t pin)
{
    // Just set input
    gpio_set_input(gpio, pin);

    // Set pull up
    gpio_set_mode(gpio, pin, 0x8);
    *gpio_get_ODR(gpio) |= 1 << pin;
}
void gpio_set_uart_tx(gpio_t gpio, gpio_pin_t pin)
{
    // Set output in uart idle level (high)
    gpio_pin_set(gpio, pin);
    gpio_set_output(gpio, pin);
    // Set push pull
    gpio_config_output_type(gpio, pin, GPIO_TYPE_PUSH_PULL);
    // Set alternate function to activate UART
    gpio_set_alternate_function(gpio, pin);
}

void gpio_set_i2c_sda(gpio_t gpio, gpio_pin_t pin)
{
    gpio_set_output(gpio, pin);
    gpio_config_output_type(gpio, pin, GPIO_TYPE_OPEN_DRAIN);
    gpio_set_alternate_function(gpio, pin);
}
void gpio_set_i2c_scl(gpio_t gpio, gpio_pin_t pin)
{
    gpio_set_output(gpio, pin);
    gpio_config_output_type(gpio, pin, GPIO_TYPE_OPEN_DRAIN);
    gpio_set_alternate_function(gpio, pin);
}
void gpio_set_timer_output(gpio_t gpio, gpio_pin_t pin, uint32_t alternate)
{
    gpio_set_output(gpio, pin);
    gpio_config_output_type(gpio, pin, GPIO_TYPE_PUSH_PULL);
    gpio_set_alternate_function(gpio, pin);
}
