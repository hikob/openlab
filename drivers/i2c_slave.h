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
 * Copyright (C) 2013 HiKoB.
 */

/**
 * \file i2c.c
 *
 * \date Nov 13, 2013
 * \author Damien Hedde <damien.hedde.at.hikob.com>
 */

#ifndef I2C_SLAVE_H_
#define I2C_SLAVE_H_

/**
 * \addtogroup drivers
 * @{
 */

#include "i2c.h"

/**
 * \defgroup I2C_slave I2C bus driver slave extension
 *
 * This extension add support for i2c slave functionalities.
 *
 *@{
 */

/**
 * I2C slave event type
 */
typedef enum
{
    I2C_SLAVE_EV_STOP = 0,
    I2C_SLAVE_EV_TX_START,
    I2C_SLAVE_EV_RX_START,
    I2C_SLAVE_EV_TX_BYTE,
    I2C_SLAVE_EV_RX_BYTE,

    I2C_SLAVE_EV_ERROR = -1,
} i2c_slave_event_t;

/**
 * I2C slave handler type
 * event:
 * + STOP: i2c_stop condition
 * + TX/RX_START: i2c_start/restart condition
 *                TX when slave will send data
 *                RX when slave will receive data
 * + TX_BYTE: slave has to send a byte, it should be put in the given pointed location 'byte'
 * + RX_BYTE: slave received a byte, it is in the given pointed location 'byte'
 */
typedef void (*i2c_slave_handler_t)(i2c_slave_event_t ev, uint8_t *byte);

/**
 * Configure, enable/disable the slave part I2C driver
 * If handler is NULL, disable the I2C slave.
 * If handler is not NULL, enable the I2C slave.
 * Handler will be called at each i2c slave event.
 *
 * \param i2c the I2C driver to configure.
 * \param handler the handler to use.
 */
void i2c_slave_configure(i2c_t i2c, i2c_slave_handler_t handler);

/**
 * Configure the slave address of I2C driver.
 * The lsb of address is ignored.
 * A 0x00 address disable it.
 * \param i2c the I2C driver to configure.
 * \param addr the i2c device address
 */
void i2c_slave_set_address(i2c_t i2c, uint8_t addr);

/**
 * @}
 * @}
 */

#endif /* I2C_SLAVE_H_ */
