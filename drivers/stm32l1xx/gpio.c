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
 * gpio.c
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "gpio.h"
#include "gpio_.h"
#include "gpio_registers.h"

#include "rcc.h"

void gpio_enable(gpio_t gpio)
{
    const _gpio_t *_gpio = gpio;

    // Set the GPIOx_EN bit in the AHB clock enable register
    rcc_ahb_enable(_gpio->ahb_bit);
}

void gpio_disable(gpio_t gpio)
{
    const _gpio_t *_gpio = gpio;

    // Clear the GPIOx_EN bit in the AHB clock enable register
    rcc_ahb_disable(_gpio->ahb_bit);
}

void gpio_lp_enable(gpio_t gpio)
{
    const _gpio_t *_gpio = gpio;

    // Set the GPIOx_EN bit in the AHB clock enable register
    rcc_ahb_lp_enable(_gpio->ahb_bit);
}

void gpio_lp_disable(gpio_t gpio)
{
    const _gpio_t *_gpio = gpio;

    // Clear the GPIOx_EN bit in the AHB clock enable register
    rcc_ahb_lp_disable(_gpio->ahb_bit);
}

void gpio_set_output(gpio_t gpio, gpio_pin_t pin)
{
    // Get the MODER value
    uint32_t moder = *gpio_get_MODER(gpio);

    // Clear the mode and alternate function bits
    moder &= ~(0x3 << (pin * 2));

    // Set the desired mode
    moder |= GPIO_MODE_OUTPUT << (pin * 2);

    // Apply
    *gpio_get_MODER(gpio) = moder;
}
void gpio_set_input(gpio_t gpio, gpio_pin_t pin)
{
    // Get the MODER value
    uint32_t moder = *gpio_get_MODER(gpio);

    // Clear the mode and alternate function bits
    moder &= ~(0x3 << (pin * 2));

    // Set the desired mode
    moder |= GPIO_MODE_INPUT << (pin * 2);

    // Apply
    *gpio_get_MODER(gpio) = moder;
}
void gpio_set_analog(gpio_t gpio, gpio_pin_t pin)
{
    // Get the MODER value
    uint32_t moder = *gpio_get_MODER(gpio);

    // Clear the mode and alternate function bits
    moder &= ~(0x3 << (pin * 2));

    // Set the desired mode
    moder |= GPIO_MODE_ANALOG << (pin * 2);

    // Apply
    *gpio_get_MODER(gpio) = moder;
}

void gpio_set_alternate_function(const _gpio_t *_gpio, gpio_pin_t pin, gpio_af_t af)
{
    // Get the MODER value
    uint32_t moder = *gpio_get_MODER(_gpio);

    // Clear the mode and alternate function bits
    moder &= ~(0x3 << (pin * 2));

    // Set the desired mode
    moder |= GPIO_MODE_ALTERNATE << (pin * 2);

    // Apply
    *gpio_get_MODER(_gpio) = moder;

    // Set the desired alternate function
    if (pin <= GPIO_PIN_7)
    {
        // Clear corresponding 4bits
        *gpio_get_AFRL(_gpio) &= ~(0xF << (pin * 4));
        // Set function
        *gpio_get_AFRL(_gpio) |= af << (pin * 4);
    }
    else
    {
        // Clear corresponding 4bits
        *gpio_get_AFRH(_gpio) &= ~(0xF << ((pin - 8) * 4));
        // Set function
        *gpio_get_AFRH(_gpio) |= af << ((pin - 8) * 4);
    }
}
void gpio_set_speed(const _gpio_t *_gpio, gpio_pin_t pin, gpio_speed_t speed)
{
    // Clear OSPEED value
    *gpio_get_OSPEEDR(_gpio) &= ~(0x3 << (pin * 2));
    // Write new OSPEED value
    *gpio_get_OSPEEDR(_gpio) |= (speed << (pin * 2));
}

void gpio_config_output_type(gpio_t gpio, gpio_pin_t pin, gpio_type_t type)
{
    // Set OTYPE value
    if (type)
    {
        *gpio_get_OTYPER(gpio) |= 1 << pin;
    }
    else
    {
        *gpio_get_OTYPER(gpio) &= ~(1 << pin);
    }
}
void gpio_config_pull_up_down(gpio_t gpio, gpio_pin_t pin,
                              gpio_pull_up_down_t mode)
{
    // Clear PUPD value
    *gpio_get_PUPDR(gpio) &= ~(0x3 << (pin * 2));

    // Write new PUPD value
    *gpio_get_PUPDR(gpio) |= (mode << (pin * 2));
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
    // Set SPI alternate function
    gpio_set_alternate_function(gpio, pin, GPIO_AF_5);

    // Set output speed
    gpio_set_speed(gpio, pin, GPIO_SPEED_LOW);
}
void gpio_set_spi_miso(gpio_t gpio, gpio_pin_t pin)
{
    // Same as SPI CLK
    gpio_set_spi_clk(gpio, pin);

    // Set pull down
    gpio_config_pull_up_down(gpio, pin, GPIO_PULL_DOWN);
}
void gpio_set_spi_mosi(gpio_t gpio, gpio_pin_t pin)
{
    // Same as SPI CLK
    gpio_set_spi_clk(gpio, pin);
}

void gpio_set_uart_rx(gpio_t gpio, gpio_pin_t pin)
{
    // Set UART alternate function
    gpio_set_alternate_function(gpio, pin, GPIO_AF_7);

    // Configure pull up
    gpio_config_pull_up_down(gpio, pin, GPIO_PULL_UP);

    // Set output speed
    gpio_set_speed(gpio, pin, GPIO_SPEED_LOW);

}
void gpio_set_uart_tx(gpio_t gpio, gpio_pin_t pin)
{
    // Same as UART RX
    gpio_set_uart_rx(gpio, pin);
}

void gpio_set_i2c_sda(gpio_t gpio, gpio_pin_t pin)
{
    // Set output speed
    gpio_set_speed(gpio, pin, GPIO_SPEED_LOW);
    gpio_config_output_type(gpio, pin, GPIO_TYPE_OPEN_DRAIN);
    gpio_config_pull_up_down(gpio, pin, GPIO_PULL_UP);

    // Set I2C alternate function
    gpio_set_alternate_function(gpio, pin, GPIO_AF_4);
}
void gpio_set_i2c_scl(gpio_t gpio, gpio_pin_t pin)
{
    // Same as I2C SDA
    gpio_set_i2c_sda(gpio, pin);
}

void gpio_set_timer_output(gpio_t gpio, gpio_pin_t pin, uint32_t alternate)
{
    // Set alternate function
    gpio_set_alternate_function(gpio, pin, alternate);
}
