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
 * spi_.h
 *
 *  Created on: Jul 7, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef SPI__H_
#define SPI__H_

#include <stdint.h>

#include "spi.h"
#include "memmap.h"
#include "rcc.h"
#include "nvic.h"
#include "gpio.h"
#include "dma.h"
#include "handler.h"

typedef struct
{
    // For DMA based asynchronous transfer, record the DMAs.
    dma_t dma_channel_rx, dma_channel_tx;

    // For ISR based asynchronous transfer, record the rx&tx pointers, and length
    uint8_t *isr_rx;
    const uint8_t *isr_tx;
    uint16_t isr_count;

    // Handler for asynchronous transfer
    handler_t transfer_handler;
    handler_arg_t transfer_handler_arg;

} _spi_data_t;

typedef struct
{
    uint32_t base_address;
    rcc_apb_bus_t apb_bus;
    rcc_apb_bit_t apb_bit;
    nvic_irq_line_t irq_line;

    _spi_data_t *data;
} _spi_t;

/**
 * Initialize the SPI object, with all the required parameters.
 */
#define SPI_INIT(name, addr, bus, bit, line) \
    static _spi_data_t name##_data; \
    const _spi_t name = { \
    .base_address = addr, \
    .apb_bus = bus, \
    .apb_bit = bit, \
    .irq_line = line, \
    .data = &name##_data \
}

static inline void spi_set_dma(const _spi_t* _spi, dma_t dma_rx, dma_t dma_tx)
{
    _spi->data->dma_channel_rx = dma_rx;
    _spi->data->dma_channel_tx = dma_tx;
}

void spi_handle_interrupt(const _spi_t *_spi);

#endif /* SPI__H_ */
