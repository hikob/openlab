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
 * i2c_.h
 *
 *  Created on: Jul 27, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 *      Author: Damien Hedde <damien.hedde.at.hikob.com>
 */

#ifndef I2C__H_
#define I2C__H_

#include "i2c.h"
#include "memmap.h"
#include "rcc.h"
#include "nvic.h"
#include "gpio.h"
#include "handler.h"

#ifdef I2C__SLAVE_SUPPORT
#include "i2c_slave.h"
#endif

typedef enum
{
    I2C_IDLE = 0,
    I2C_SENDING_START = 1,
    I2C_SENDING_ADDRESS = 2,
    I2C_SENDING_DATA = 3,
    I2C_RECEIVING_DATA = 4,
    I2C_SENDING_RESTART = 5,
    I2C_ERROR = 6,
#ifdef I2C__SLAVE_SUPPORT
    I2C_SL_TX,
    I2C_SL_RX,
#endif
} i2c_state_t;

typedef struct
{
    // State
    volatile i2c_state_t state;
    // Address to send or receive
    uint8_t address;
    // Send buffer
    uint32_t len_send;
    uint32_t cpt_send;
    const uint8_t *buf_send;
    // Receive buffer
    uint32_t len_recv;
    uint32_t cpt_recv;
    uint8_t *buf_recv;
    // Non-blocking end-transfer handler
    result_handler_t transfer_handler;
    handler_arg_t transfer_handler_arg;
#ifdef I2C__SLAVE_SUPPORT
    i2c_slave_handler_t slave_handler;
#endif
} _i2c_data_t;

typedef struct
{
    uint32_t base_address;
    rcc_apb_bus_t apb_bus;
    rcc_apb_bit_t apb_bit;
    nvic_irq_line_t irq_line_ev, irq_line_er;
    _i2c_data_t *data;
} _i2c_t;

#define I2C_INIT(name, addr, bus, bit, ev_line, er_line) \
    static _i2c_data_t name##_data; \
    const _i2c_t name = { \
    .base_address = addr, \
    .apb_bus = bus, \
    .apb_bit = bit, \
    .irq_line_ev = ev_line, \
    .irq_line_er = er_line, \
    .data = &name##_data \
}

void i2c_handle_ev_interrupt(const _i2c_t *_i2c);
void i2c_handle_er_interrupt(const _i2c_t *_i2c);

#endif
