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
 * i2c.h
 *
 *  Created on: Aug 3, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

/**
 * Abstract representation of a I2C driver.
 */
typedef void *i2c_t;

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
 * Send a given amount of bytes with an I2C driver.
 *
 * \param i2c the I2C driver to use;
 * \param addr the I2C slave address;
 * \param tx_buffer a pointer to the buffer to send;
 * \param length the number of bytes to transmit;
 * \return 0 if the transfer succeeded, >0 if an error occurred.
 */
uint32_t i2c_tx(i2c_t i2c, uint8_t addr, const uint8_t *tx_buffer, uint16_t length);

/**
 * Receive a given amount of bytes with an I2C driver.
 *
 * \param i2c the I2C driver to use;
 * \param addr the I2C slave address;
 * \param rx_buffer a pointer to the buffer to store the received bytes;
 * \param length the number of bytes to transmit;
 * \return 0 if the transfer succeeded, >0 if an error occurred.
 */
uint32_t i2c_rx(i2c_t i2c, uint8_t addr, uint8_t *rx_buffer, uint16_t length);

/**
 * Send then receive a given amount of bytes with an I2C driver.
 *
 * \param i2c the I2C driver to use;
 * \param addr the I2C slave address;
 * \param tx_buffer a pointer to the buffer to send;
 * \param tx_length the number of bytes to transmit;
 * \param rx_buffer a pointer to the buffer to store the received bytes;
 * \param rx_length the number of bytes to receive.
 * \return 0 if the transfer succeeded, >0 if an error occurred.
 */
uint32_t i2c_tx_rx(i2c_t i2c, uint8_t addr, const uint8_t *tx_buffer,
                   uint16_t tx_length, uint8_t *rx_buffer, uint16_t rx_length);

#endif /* I2C_H_ */
