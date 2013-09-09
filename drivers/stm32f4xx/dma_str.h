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
 * dma_.h
 *
 *  Created on: Nov 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef DMA_STR_H_
#define DMA_STR_H_

#include <stdint.h>
#include "handler.h"
#include "rcc.h"
#include "nvic.h"
#include "dma.h"

/**
 * The DMA streams
 */
typedef enum
{
    DMA_STREAM_0 = 0,
    DMA_STREAM_1 = 1,
    DMA_STREAM_2 = 2,
    DMA_STREAM_3 = 3,
    DMA_STREAM_4 = 4,
    DMA_STREAM_5 = 5,
    DMA_STREAM_6 = 6,
    DMA_STREAM_7 = 7
} dma_stream_t;

/**
 * The DMA trigger channel
 */
typedef enum
{
    DMA_TRIGGER_CHANNEL_0 = 0,
    DMA_TRIGGER_CHANNEL_1 = 1,
    DMA_TRIGGER_CHANNEL_2 = 2,
    DMA_TRIGGER_CHANNEL_3 = 3,
    DMA_TRIGGER_CHANNEL_4 = 4,
    DMA_TRIGGER_CHANNEL_5 = 5,
    DMA_TRIGGER_CHANNEL_6 = 6,
    DMA_TRIGGER_CHANNEL_7 = 7
} dma_trigger_channel_t;

typedef struct
{
    // The channel for DMA trigger
    dma_trigger_channel_t trigger_channel;

    // The handler for transfer done
    handler_t handler;
    handler_arg_t handler_arg;
} _dma_data_t;

typedef struct
{
    // DMA base address
    uint32_t base_address;

    // DMA RCC line
    rcc_ahb_bit_t dma_bit;

    // The DMA stream
    dma_stream_t stream;

    // DMA channel IRQ line
    nvic_irq_line_t irq_line;

    // The data
    _dma_data_t *data;
} _dma_t;

/**
 * Initialize a DMA object.
 *
 * \param dma the DMA object to initialize.
 * \param channel the DMA channel of this object.
 */
#define DMA_INIT(name, addr, bit, str, line) \
    static _dma_data_t name##_data = { \
        .trigger_channel = 0, \
        .handler = NULL, \
        .handler_arg = NULL, \
    }; \
    const _dma_t name = { \
    .base_address = addr, \
    .dma_bit = bit, \
    .stream = str, \
    .irq_line = line, \
    .data = &name##_data \
}

/** Set the channel of a DMA stream */
static inline void dma_stream_set_channel(const _dma_t *_dma, uint8_t channel)
{
    _dma->data->trigger_channel = channel;
}

/**
 * Handle a DMA channel interrupt.
 */
void dma_handle_interrupt(dma_t dma);

#endif /* DMA_STR_H_ */
