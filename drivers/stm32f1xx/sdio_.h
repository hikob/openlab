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
 * sdio_.h
 *
 *  Created on: Jul 26, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef SDIO__H_
#define SDIO__H_

#include <stdbool.h>
#include "rcc.h"
#include "gpio.h"
#include "dma.h"
#include "sdio.h"
#include "handler.h"

typedef struct
{
    uint8_t read_bl_len;
    uint32_t c_size;
    uint8_t c_size_mult;
} sd_card_description_t;

typedef enum
{
    SD_STATE_UNKNOWN = -1,
    SD_STATE_IDLE    = 0,
    SD_STATE_READY   = 1,
    SD_STATE_IDENT   = 2,
    SD_STATE_STBY    = 3,
    SD_STATE_TRAN    = 4,
    SD_STATE_DATA    = 5,
    SD_STATE_RCV     = 6,
    SD_STATE_PRG     = 7,
    SD_STATE_DIS     = 8
} sd_state_t;

typedef struct
{
    // SDIO pinout
    gpio_t gpio[6];
    gpio_pin_t pin[6];

    gpio_t regu_gpio;
    gpio_pin_t regu_pin;

    // DMA transfer
    dma_t dma_channel;
    handler_t transfer_handler;

    // Card state and description
    sd_card_description_t sd_desc;
    sd_card_type_t sd_type;
    sd_state_t sd_state;
    uint32_t CID[4], CSD[4], rca;
    bool sd_card_locked, sd_app_cmd;
} _sdio_t;

static inline void sdio_init(_sdio_t *_sdio, gpio_t *gpio, gpio_pin_t *pin, gpio_t regu_gpio,
                             gpio_pin_t regu_pin, dma_t dma_channel)
{
    uint8_t i;

    for (i = 0; i < 6; i++)
    {
        _sdio->gpio[i] = gpio[i];
        _sdio->pin[i] = pin[i];
    }

    _sdio->regu_gpio = regu_gpio;
    _sdio->regu_pin = regu_pin;

    _sdio->dma_channel = dma_channel;
    _sdio->transfer_handler = NULL;

    _sdio->sd_state = SD_STATE_UNKNOWN;
    _sdio->rca = 0;
    _sdio->sd_card_locked = false;
    _sdio->sd_app_cmd = false;
}

#endif /* SDIO__H_ */
