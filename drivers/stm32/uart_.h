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
 * uart_.h
 *
 *  Created on: Jul 7, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef UART__H_
#define UART__H_

#include <stdint.h>
#include <stddef.h>

#include "uart.h"
#include "memmap.h"
#include "rcc.h"
#include "nvic.h"
#include "gpio_.h"
#include "dma_.h"
#include "handler.h"

typedef struct
{
    uint32_t base_address;
    rcc_apb_bus_t apb_bus;
    rcc_apb_bit_t apb_bit;
    nvic_irq_line_t irq_line;

    _dma_t *dma_channel_tx;

    // For ISR based asynchronous transfer, record the tx pointer and length
    const uint8_t *isr_tx;
    uint16_t isr_count;

    uart_handler_t rx_handler;
    handler_t tx_handler;
    handler_arg_t rx_handler_arg, tx_handler_arg;
} _uart_t;

static inline void uart_init(_uart_t *_uart, uint32_t base_address,
                             rcc_apb_bus_t apb_bus, rcc_apb_bit_t apb_bit, nvic_irq_line_t irq_line,
                             gpio_t gpio, gpio_pin_t pin_rx, gpio_pin_t pin_tx,
                             _dma_t *dma_channel_tx)
{
    _uart->base_address = base_address;

    _uart->apb_bus = apb_bus;
    _uart->apb_bit = apb_bit;
    _uart->irq_line = irq_line;

    // Configure the IOs
    gpio_enable(gpio);
    gpio_set_uart_rx(gpio, pin_rx);
    gpio_set_uart_tx(gpio, pin_tx);


    _uart->dma_channel_tx = dma_channel_tx;

    _uart->rx_handler = NULL;
    _uart->tx_handler = NULL;
}

void uart_handle_interrupt(_uart_t *_uart);

#endif /* UART__H_ */
