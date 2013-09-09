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

/**
 * max6x.h
 *
 *  Created on: Feb 27, 2012
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef MAX6X_H_
#define MAX6X_H_

#include <stdint.h>
#include <stdbool.h>
#include "ublox_class.h"
#include "handler.h"
#include "i2c.h"

#define MAX6X_BASE_ADD 0x84 // 7-bit address is 0x42

typedef enum
{
    MAX6X_ANTENNA_PATCH = 0,
    MAX6X_ANTENNA_CONNECTOR,
} max6x_antenna_t;

typedef void (*max6x_tx_ready_handler_t)(void);
typedef void (*max6x_handler_t)(uint8_t *buf, uint16_t length);

bool max6x_init();
bool max6x_reset();
bool max6x_coldstart();
bool max6x_hotstart();
bool max6x_set_ubx_protocol();
bool max6x_send_periodically(uint8_t class, uint8_t id);
bool max6x_set_rate(uint16_t rate);
bool max6x_configure_power_management();
bool max6x_send_msg(uint8_t class, uint8_t id, uint16_t len, uint8_t *msg);
void max6x_select_antenna(max6x_antenna_t ant);

void max6x_start();

void max6x_register_nav_handler(max6x_handler_t hdl);

bool max6x_is_rdy();
void max6x_set_rdy_handler(handler_t handler, handler_arg_t arg);
uint32_t max6x_data_count(uint16_t *count, result_handler_t handler, handler_arg_t arg);
uint32_t max6x_data_read(uint8_t *buf, uint16_t size, result_handler_t handler, handler_arg_t arg);

static inline uint16_t max6x_data_count_unpack(uint16_t count)
{
    uint8_t *raw = (uint8_t *) &count;
    return (((uint16_t) raw[0]) << 8) | raw[1];
}

void max6x_decoder_reset();
void max6x_data_decode(uint8_t *buf, uint16_t size, max6x_handler_t hdl);

#endif
