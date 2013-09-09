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
 * \file i2c.h
 *
 * \date Aug 3, 2011
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef I2C_H_
#define I2C_H_

/**
 * \addtogroup drivers
 * @{
 */

/**
 * \defgroup I2C I2C bus master driver
 *
 * This driver provides all methods required to transfer data with slaves over
 * a I2C bus.
 *
 *@{
 */

#include <stdint.h>

#include "handler.h"

/**
 * Abstract representation of a I2C driver.
 */
typedef const void *i2c_t;

typedef enum
{
    I2C_CLOCK_MODE_STANDARD,
    I2C_CLOCK_MODE_FAST,
} i2c_clock_mode_t;

/**
 * Enable a I2C driver
 *
 * Once configured, a I2C driver should be enabled in order to be
 * operationnal.
 *
 * \param i2c the I2C driver to enable;
 * \param bitrate the desired bitrate
 */
void i2c_enable(i2c_t i2c, i2c_clock_mode_t mode);

/**
 * Disable an I2C driver.
 *
 * This allows to disable a driver, hence reducing power consumption.
 * Once disabled, the I2C driver must not be used before being enabled
 * back.
 *
 * \param i2c the I2C driver to disable;
 */
void i2c_disable(i2c_t i2c);

/**
 * Send then receive a given amount of bytes with an I2C driver.
 * Non-blocking call if the given handler is non-NULL.
 * It is then called when the transfer is completed.
 *
 * Note: the handler is called from interrupt context.
 *
 * \param i2c the I2C driver to use;
 * \param addr the I2C slave address;
 * \param tx_buffer a pointer to the buffer to send;
 * \param tx_length the number of bytes to transmit;
 * \param rx_buffer a pointer to the buffer to store the received bytes;
 * \param rx_length the number of bytes to receive;
 * \param handler the handler called when transfer completes;
 * \param arg an argument to be given to the handler.
 * \return >0 if an error occurred.
 */
unsigned i2c_tx_rx_async(i2c_t i2c, uint8_t addr, const uint8_t *tx_buffer,
        uint16_t tx_length, uint8_t *rx_buffer, uint16_t rx_length,
        result_handler_t handler, handler_arg_t arg);

/**
 * Send a given amount of bytes with an I2C driver.
 * Non-blocking call if the given handler is non-NULL.
 * It is then called when the transfer is completed.
 *
 * Note: the handler is called from interrupt context.
 *
 * \param i2c the I2C driver to use;
 * \param addr the I2C slave address;
 * \param tx_buffer a pointer to the buffer to send;
 * \param tx_length the number of bytes to transmit;
 * \param handler the handler called when transfer completes;
 * \param arg an argument to be given to the handler.
 * \return >0 if an error occurred.
 */
static inline unsigned i2c_tx_async(i2c_t i2c, uint8_t addr,
        const uint8_t *tx_buffer, uint16_t length,
        result_handler_t handler, handler_arg_t arg)
{
    return i2c_tx_rx_async(i2c, addr, tx_buffer, length,
            NULL, 0, handler, arg);
}

/**
 * Receive a given amount of bytes with an I2C driver.
 * Non-blocking call if the given handler is non-NULL.
 * It is then called when the transfer is completed.
 *
 * Note: the handler is called from interrupt context.
 *
 * \param i2c the I2C driver to use;
 * \param addr the I2C slave address;
 * \param rx_buffer a pointer to the buffer to store the received bytes;
 * \param rx_length the number of bytes to receive;
 * \param handler the handler called when transfer completes;
 * \param arg an argument to be given to the handler.
 * \return >0 if an error occurred.
 */
static inline unsigned i2c_rx_async(i2c_t i2c, uint8_t addr,
        uint8_t *rx_buffer, uint16_t length,
        result_handler_t handler, handler_arg_t arg)
{
    return i2c_tx_rx_async(i2c, addr, NULL, 0,
            rx_buffer, length, handler, arg);
}

/**
 * Send a given amount of bytes with an I2C driver.
 * Blocking call.
 *
 * \param i2c the I2C driver to use;
 * \param addr the I2C slave address;
 * \param tx_buffer a pointer to the buffer to send;
 * \param length the number of bytes to transmit;
 * \return 0 if the transfer succeeded, >0 if an error occurred.
 */
static inline unsigned i2c_tx(i2c_t i2c, uint8_t addr,
        const uint8_t *tx_buffer, uint16_t length)
{
    return i2c_tx_rx_async(i2c, addr, tx_buffer, length,
            NULL, 0, NULL, NULL);
}

/**
 * Receive a given amount of bytes with an I2C driver.
 * Blocking call.
 *
 * \param i2c the I2C driver to use;
 * \param addr the I2C slave address;
 * \param rx_buffer a pointer to the buffer to store the received bytes;
 * \param length the number of bytes to transmit;
 * \return 0 if the transfer succeeded, >0 if an error occurred.
 */
static inline unsigned i2c_rx(i2c_t i2c, uint8_t addr,
        uint8_t *rx_buffer, uint16_t length)
{
    return i2c_tx_rx_async(i2c, addr, NULL, 0,
            rx_buffer, length, NULL, NULL);
}

/**
 * Send then receive a given amount of bytes with an I2C driver.
 * Blocking call.
 *
 * \param i2c the I2C driver to use;
 * \param addr the I2C slave address;
 * \param tx_buffer a pointer to the buffer to send;
 * \param tx_length the number of bytes to transmit;
 * \param rx_buffer a pointer to the buffer to store the received bytes;
 * \param rx_length the number of bytes to receive.
 * \return 0 if the transfer succeeded, >0 if an error occurred.
 */
static inline unsigned i2c_tx_rx(i2c_t i2c, uint8_t addr,
        const uint8_t *tx_buffer, uint16_t tx_length,
        uint8_t *rx_buffer, uint16_t rx_length)
{
    return i2c_tx_rx_async(i2c, addr, tx_buffer, tx_length,
            rx_buffer, rx_length, NULL, NULL);
}

/**
 * @}
 * @}
 */

#endif /* I2C_H_ */
