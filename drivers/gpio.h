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
 * \file gpio.h
 *
 * \date Jul 8, 2011
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef GPIO_H_
#define GPIO_H_

/**
 * \addtogroup drivers
 * @{
 */

/**
 * \defgroup GPIO GPIO driver
 *
 * This driver provides all functions required to control input and output pins,
 * but not the interrupts associated to it, see \ref EXTI for this.
 *
 *@{
 */

#include <stdint.h>

/** Abstract type defining a GPIO driver */
typedef const void *gpio_t;

/**
 * Available pins on each GPIO.
 *
 * \note Depending on the STM32 package, all available values may not be
 * mapped to real hardware pins.
 */
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

/**
 * Available values for output type.
 */
typedef enum
{
    GPIO_TYPE_PUSH_PULL  = 0,
    GPIO_TYPE_OPEN_DRAIN = 1
} gpio_type_t;

/** Available values for pull-up/pull-down configuration */
typedef enum
{
    GPIO_PULL_DISABLED = 0,
    GPIO_PULL_UP       = 1,
    GPIO_PULL_DOWN     = 2
} gpio_pull_up_down_t;

/**
 * Enable a GPIO driver
 *
 * \param gpio the GPIO driver to enable
 */
void gpio_enable(gpio_t gpio);
/**
 * Disable a GPIO driver.
 *
 * Once disabled, the GPIO pins remain configured in the same mode, but can't be
 * further changed until enabled back.
 *
 * \param gpio the GPIO driver to disable
 */
void gpio_disable(gpio_t gpio);

/**
 * Enable a GPIO driver in SLEEP mode.
 *
 * \param gpio the GPIO driver to enable
 */
void gpio_lp_ensable(gpio_t gpio);
/**
 * Disable a GPIO driver in SLEEP mode.
 *
 * \param gpio the GPIO driver to disable
 */
void gpio_lp_disable(gpio_t gpio);
/**
 * Configure a pin in digital input mode.
 *
 * The value of the pin may be read with \ref gpio_pin_read once configured in input.
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 */
void gpio_set_input(gpio_t gpio, gpio_pin_t pin);
/**
 * Configure a pin in digital output mode.
 *
 * The value of the pin may be set with \ref gpio_pin_set and cleared with
 * \ref gpio_pin_clear once configured in output.
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 */
void gpio_set_output(gpio_t gpio, gpio_pin_t pin);
/**
 * Configure a pin in analog mode.
 *
 * All unused pins should be configured in analog mode to save power, as well
 * as those used for ADC or DAC.
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 */
void gpio_set_analog(gpio_t gpio, gpio_pin_t pin);

/**
 * Select the output type of a PIN.
 *
 * \note The pin MUST be configured in output type before calling this method,
 * with \ref gpio_set_output.
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 * \param type the output type to configure
 */
void gpio_config_output_type(gpio_t gpio, gpio_pin_t pin, gpio_type_t type);
/**
 * Configure a pull-up or pull-down of a PIN.
 *
 * \note The pin MUST be configured in output type before calling this method,
 * with \ref gpio_set_output.
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 * \param mode the output pull-up/pull-down configuration to set
 */
void gpio_config_pull_up_down(gpio_t gpio, gpio_pin_t pin, gpio_pull_up_down_t mode);

/**
 * Set an PIN output value to logical 1.
 *
 * \note The pin MUST be configured in output mode with \ref gpio_set_output before
 * calling this method.
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 */
void gpio_pin_set(gpio_t gpio, gpio_pin_t pin);
/**
 * Set an PIN output value to logical 0.
 *
 * \note The pin MUST be configured in output mode with \ref gpio_set_output before
 * calling this method.
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 */
void gpio_pin_clear(gpio_t gpio, gpio_pin_t pin);
/**
 * Invert the PIN output value.
 *
 * \note The pin MUST be configured in output mode with \ref gpio_set_output before
 * calling this method.
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 */
void gpio_pin_toggle(gpio_t gpio, gpio_pin_t pin);
/**
 * Read the PIN input value.
 *
 * \note The pin MUST be configured in input mode with \ref gpio_set_input before
 * calling this method.
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 * \return the logical input value, 0 or 1
 */
uint32_t gpio_pin_read(gpio_t gpio, gpio_pin_t pin);

// Specific Configurations
/**
 * Configure a pin for SPI CLK mode.
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 */
void gpio_set_spi_clk(gpio_t gpio, gpio_pin_t pin);
/**
 * Configure a pin for SPI MISO mode.
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 */
void gpio_set_spi_miso(gpio_t gpio, gpio_pin_t pin);
/**
 * Configure a pin for SPI MOSI mode.
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 */
void gpio_set_spi_mosi(gpio_t gpio, gpio_pin_t pin);
/**
 * Configure a pin for UART RX mode.
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 */
void gpio_set_uart_rx(gpio_t gpio, gpio_pin_t pin);
/**
 * Configure a pin for UART TX mode.
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 */
void gpio_set_uart_tx(gpio_t gpio, gpio_pin_t pin);
/**
 * Configure a pin for I2C SDA mode.
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 */
void gpio_set_i2c_sda(gpio_t gpio, gpio_pin_t pin);
/**
 * Configure a pin for I2C SCL mode.
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 */
void gpio_set_i2c_scl(gpio_t gpio, gpio_pin_t pin);

/**
 * Configure a pin for Timer Output Compare
 *
 * \param gpio the GPIO of the pin
 * \param pin the pin number of the pin
 * \param alternate a number for timer selection, if relevant
 */
void gpio_set_timer_output(gpio_t gpio, gpio_pin_t pin, uint32_t alternate);

/**
 * @}
 * @}
 */

#endif /* GPIO_H_ */
