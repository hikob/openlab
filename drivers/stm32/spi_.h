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
#include "dma_.h"
#include "handler.h"

typedef struct
{
    uint32_t base_address;
    rcc_apb_bus_t apb_bus;
    rcc_apb_bit_t apb_bit;
    nvic_irq_line_t irq_line;

    // For DMA based asynchronous transfer, record the DMAs.
    _dma_t *dma_channel_rx, *dma_channel_tx;

    // For ISR based asynchronous transfer, record the rx&tx pointers, and length
    uint8_t *isr_rx;
    const uint8_t *isr_tx;
    uint16_t isr_count;

    // Handler for asynchronous transfer
    handler_t transfer_handler;
    handler_arg_t transfer_handler_arg;
} _spi_t;

/**
 * Initialize the SPI object, with all the required parameters.
 *
 * If there is no DMA channels for RX or TX, set them both to NULL
 */
static inline void spi_init(_spi_t *_spi, uint32_t base_address,
                            rcc_apb_bus_t apb_bus, rcc_apb_bit_t apb_bit, nvic_irq_line_t irq_line,
                            gpio_t gpio, gpio_pin_t pin_clk, gpio_pin_t pin_mosi,
                            gpio_pin_t pin_miso, _dma_t *dma_channel_rx, _dma_t *dma_channel_tx)
{
    _spi->base_address = base_address;
    _spi->apb_bus = apb_bus;
    _spi->apb_bit = apb_bit;
    _spi->irq_line = irq_line;
    _spi->dma_channel_rx = dma_channel_rx;
    _spi->dma_channel_tx = dma_channel_tx;

    // Configure the IOs
    gpio_enable(gpio);
    gpio_set_spi_clk(gpio, pin_clk);
    gpio_set_spi_miso(gpio, pin_miso);
    gpio_set_spi_mosi(gpio, pin_mosi);
}

void spi_handle_interrupt(_spi_t *_spi);

#endif /* SPI__H_ */
