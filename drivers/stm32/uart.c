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
 * uart.c
 *
 *  Created on: Jul 7, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include <stddef.h>

#include "uart.h"
#include "uart_.h"
#include "uart_registers.h"
#include "rcc.h"
#include "gpio_.h"
#include "nvic_.h"

#include "platform.h"

static inline void tx_dma(const _uart_t *_uart, const uint8_t *tx_buffer,
                          uint16_t length);
static inline void tx_interrupt(const _uart_t *_uart, const uint8_t *tx_buffer,
                                uint16_t length);
static void tx_done(const _uart_t *_uart);

void uart_enable(uart_t uart, uint32_t baudrate)
{
    const _uart_t *_uart = uart;

    // Enable the Clock for this peripheral
    rcc_apb_enable(_uart->apb_bus, _uart->apb_bit);

    // Clear all registers
    *uart_get_CR1(_uart) = 0;
    *uart_get_CR2(_uart) = 0;
    *uart_get_CR3(_uart) = 0;

    // Read SR and DR to clear the flags
    (void) *uart_get_SR(_uart);
    (void) *uart_get_DR(_uart);

    /**
     *  If L1 family, use 8 samples, if F1 use 16 samples
     *
     *  To test this, try to write the OVER8 bit, and check if it is written.
     */
    *uart_get_CR1(_uart) = UART_CR1__OVER8;
    uint16_t over8 = (*uart_get_CR1(_uart) & UART_CR1__OVER8);

    // Set Baudrate, depending on the PCLKx speed, we're using OVER8=1
    // Hence depending on the bus
    uint16_t baud;
    baud = (over8 ? 2 : 1) * rcc_sysclk_get_clock_frequency(
               _uart->apb_bus == 1 ? RCC_SYSCLK_CLOCK_PCLK1
               : RCC_SYSCLK_CLOCK_PCLK2) / baudrate;

    // If OVER8 is set, shift the last 4 bits to the right, clearing new bit3
    if (over8)
    {
        baud = (baud & 0xFFF0) | ((baud >> 1) & 0x7);
    }

    *uart_get_BRR(_uart) = baud;

    // Enable USART, TX and RX, set 8bit data, no parity, no interrupt, OVER8 if enabled
    *uart_get_CR1(_uart) = (over8 ? UART_CR1__OVER8 : 0) | UART_CR1__UE
                           | UART_CR1__TE | UART_CR1__RE;

    // Set 1 stop bit, no CLOCK
    *uart_get_CR2(_uart) = 0;

    // Enable the uart interrupt line in the NVIC
    nvic_enable_interrupt_line(_uart->irq_line);

    // Enable the DMA if set
    if (_uart->data->dma_channel_tx)
    {
        dma_enable(_uart->data->dma_channel_tx);
    }
}

void uart_disable(uart_t uart)
{
    const _uart_t *_uart = uart;

    // Disable USART, TX and RX
    *uart_get_CR1(_uart) = 0;

    // Disable the Clock for this peripheral
    rcc_apb_disable(_uart->apb_bus, _uart->apb_bit);

    // Disable the USART interrupt line in the NVIC
    nvic_disable_interrupt_line(_uart->irq_line);
}
void uart_set_rx_handler(uart_t uart, uart_handler_t handler, handler_arg_t arg)
{
    const _uart_t *_uart = uart;

    // Store the handler and arg
    _uart->data->rx_handler = handler;
    _uart->data->rx_handler_arg = arg;

    // Enable RX interrupt if required
    if (handler)
    {
        *uart_get_CR1(_uart) |= UART_CR1__RXNEIE;
    }
    else
    {
        *uart_get_CR1(_uart) &= ~UART_CR1__RXNEIE;
    }

    // Read SR and DR to clear all the flags
    (void) *uart_get_SR(_uart);
    (void) *uart_get_DR(_uart);
}

void uart_set_irq_priority(uart_t uart, uint8_t priority)
{
    const _uart_t *_uart = uart;

    // Set the priority in the NVIC
    nvic_set_priority(_uart->irq_line, priority);
}
void uart_transfer(uart_t uart, const uint8_t *tx_buffer, uint16_t length)
{
    const _uart_t *_uart = uart;

    uint32_t i;

    for (i = 0; i < length; i++)
    {
        // Wait for TX data register to be ready
        while (!(*uart_get_SR(_uart) & UART_SR__TXE))
        {
        }

        // Write data to the DR register
        *uart_get_DR(_uart) = tx_buffer[i];
    }

    // Wait for transmission to complete
    while (!(*uart_get_SR(_uart) & UART_SR__TC))
    {
    }

}

void uart_transfer_async(uart_t uart, const uint8_t *tx_buffer,
                         uint16_t length, handler_t handler, handler_arg_t handler_arg)
{
    const _uart_t *_uart = uart;

    // Store the handlers
    _uart->data->tx_handler = handler;
    _uart->data->tx_handler_arg = handler_arg;

    // Notify of ongoing underground transfer
    platform_prevent_low_power();

    // Wait for TX data register to be ready
    while (!(*uart_get_SR(_uart) & UART_SR__TXE))
    {
    }

    // Check if DMA is enabled
    if (_uart->data->dma_channel_tx)
    {
        // Yes, Send with DMA
        tx_dma(_uart, tx_buffer, length);
    }
    else
    {
        // No, send using interrupt
        tx_interrupt(_uart, tx_buffer, length);
    }

}

static inline void tx_dma(const _uart_t *_uart, const uint8_t *tx_buffer,
                          uint16_t length)
{
    // Configure the TX DMA channel
    dma_config(
        // The DMA channel
        _uart->data->dma_channel_tx,
        // the Peripheral Address (UART Data Register)
        (uint32_t) uart_get_DR(_uart),
        // The Memory address: if rx_buffer is provided, use it
        (uint32_t) tx_buffer,
        // The length
        length,
        // The Transfer size: 8bit
        DMA_SIZE_8bit,
        // Direction: from peripheral to memory
        DMA_DIRECTION_TO_PERIPHERAL,
        // Increment memory address
        DMA_INCREMENT_ON);

    // Start the TX DMA, with the Uart handler
    dma_start(_uart->data->dma_channel_tx, (handler_t) tx_done, (handler_arg_t) (uint32_t) _uart);

    // Enable the DMA trigger generation
    *uart_get_CR3(_uart) |= UART_CR3__DMAT;
}
static inline void tx_interrupt(const _uart_t *_uart, const uint8_t *tx_buffer,
                                uint16_t length)
{
    // Store tx pointer and length
    _uart->data->isr_tx = tx_buffer;
    _uart->data->isr_count = length;

    // Enable the TXE interrupt
    *uart_get_CR1(_uart) |= UART_CR1__TXEIE;
}

static void tx_done(const _uart_t *_uart)
{
    // Notify of ongoing underground transfer terminated
    platform_release_low_power();

    // Call handler if any
    if (_uart->data->tx_handler)
    {
        _uart->data->tx_handler(_uart->data->tx_handler_arg);
    }
}
void uart_handle_interrupt(const _uart_t *_uart)
{
    uint32_t sr = *uart_get_SR(_uart);

    // Check if RX interrupt happened and was enabled
    if (sr & UART_SR__RXNE)
    {
        // Read the received char
        uint8_t c = *uart_get_DR(_uart);

        // Check if handler
        if (_uart->data->rx_handler)
        {
            // Call the handler
            _uart->data->rx_handler(_uart->data->rx_handler_arg, c);
        }
    }
    else

        // Check for error
        if (sr & (UART_SR__ORE | UART_SR__NF | UART_SR__FE | UART_SR__PE))
        {
            // Make sure to read DR to clear the flags
            (void) *uart_get_DR(_uart);
        }
        else

            // Check if TX interrupt happened and was enabled
            if ((sr & UART_SR__TXE) && (*uart_get_CR1(_uart) & UART_CR1__TXEIE))
            {
                // Check if there are some more bytes to send
                if (_uart->data->isr_count)
                {
                    // Yes, send next byte
                    *uart_get_DR(_uart) = *_uart->data->isr_tx;

                    // Increment pointer
                    _uart->data->isr_tx++;
                    // Decrement count
                    _uart->data->isr_count--;
                }
                else
                {
                    // No everything is sent, disable interrupt.
                    *uart_get_CR1(_uart) &= ~(UART_CR1__TXEIE);

                    // Call handler
                    tx_done(_uart);
                }
            }
}
