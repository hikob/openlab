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
 * spi.c
 *
 *  Created on: Jul 7, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "printf.h"

#include "spi_.h"
#include "spi_registers.h"
#include "gpio_.h"
#include "nvic_.h"

#include "platform.h"

static inline void transfer_dma(const _spi_t *_spi, const uint8_t *tx_buffer,
                                uint8_t *rx_buffer, uint16_t length);
static inline void transfer_interrupt(const _spi_t *_spi, const uint8_t *tx_buffer,
                                      uint8_t *rx_buffer, uint16_t length);
static inline void cancel_dma(const _spi_t *_spi);
static inline void cancel_interrupt(const _spi_t *_spi);

static void transfer_done(const _spi_t *spi);

void spi_enable(spi_t spi, uint32_t baudrate, spi_clock_mode_t clock_mode)
{
    const _spi_t *_spi = spi;

    // Enable the clock for this peripheral
    rcc_apb_enable(_spi->apb_bus, _spi->apb_bit);

    // Disable the SPI prior to configuration
    *spi_get_CR1(_spi) = 0;

    // Disable all interrupts
    *spi_get_CR2(_spi) = 0;

    // Enable interrupts in NVIC
    nvic_enable_interrupt_line(_spi->irq_line);

    // Compute divider
    uint16_t divider;
    uint32_t pclk;

    if (_spi->apb_bus == 1)
    {
        pclk = rcc_sysclk_get_clock_frequency(RCC_SYSCLK_CLOCK_PCLK1);
    }
    else
    {
        pclk = rcc_sysclk_get_clock_frequency(RCC_SYSCLK_CLOCK_PCLK2);
    }

    for (divider = 0; divider < 8; divider++)
    {
        // Check if clock divided is not too high
        if (pclk / (1 << (divider + 1)) <= baudrate)
        {
            break;
        }
    }

    // Set register value
    divider <<= 3;

    // Set baudrate, clock polarity, and master mode, soft NSS
    *spi_get_CR1(_spi) = SPI_CR1__SSM | SPI_CR1__SSI | divider | SPI_CR1__MSTR
                         | clock_mode;

    // Set CRC 7
    *spi_get_CRCPR(_spi) = 7;

    // Enable SPI
    *spi_get_CR1(_spi) |= SPI_CR1__SPE;
}
void spi_disable(spi_t spi)
{
    const _spi_t *_spi = spi;

    // Disable SPI
    *spi_get_CR1(_spi) = 0;

    // Disable clocks
    rcc_apb_disable(_spi->apb_bus, _spi->apb_bit);
}

uint8_t spi_transfer_single(spi_t spi, uint8_t tx)
{
    const _spi_t *_spi = spi;

    uint8_t rx;

    // Wait until TXE=1
    while (!(*spi_get_SR(_spi) & SPI_SR__TXE))
    {
    }

    // Write the byte to DR
    *spi_get_DR(_spi) = tx;

    // Wait until RXNE=1
    while (!(*spi_get_SR(_spi) & SPI_SR__RXNE))
    {
    }

    // Read the byte from DR
    rx = *spi_get_DR(_spi);

    // Wait until BSY=0
    while (*spi_get_SR(_spi) & SPI_SR__BSY)
    {
    }

    // Return value
    return rx;
}

void spi_transfer(spi_t spi, const uint8_t *tx_buffer, uint8_t *rx_buffer,
                  uint16_t length)
{
    uint32_t i;

    //  For each byte to write
    for (i = 0; i < length; i++)
    {
        uint8_t rx_, tx_;

        if (tx_buffer)
        {
            tx_ = tx_buffer[i];
        }
        else
        {
            tx_ = 0;
        }

        rx_ = spi_transfer_single(spi, tx_);

        if (rx_buffer)
        {
            rx_buffer[i] = rx_;
        }
    }
}
void spi_transfer_async(spi_t spi, const uint8_t *tx_buffer,
                        uint8_t *rx_buffer, uint16_t length, handler_t handler,
                        handler_arg_t handler_arg)
{
    const _spi_t *_spi = spi;

    // Store handlers
    _spi->data->transfer_handler = handler;
    _spi->data->transfer_handler_arg = handler_arg;

    // Notify of ongoing underground transfer
    platform_prevent_low_power();

    // Check if DMA channels are available
    if (_spi->data->dma_channel_rx && _spi->data->dma_channel_tx)
    {
        // Yes, use DMA
        transfer_dma(_spi, tx_buffer, rx_buffer, length);
    }
    else
    {
        // No, use Interrupt
        transfer_interrupt(_spi, tx_buffer, rx_buffer, length);
    }
}

void spi_async_cancel(spi_t spi)
{
    const _spi_t *_spi = spi;

    // Just clear handlers
    _spi->data->transfer_handler = NULL;
    _spi->data->transfer_handler_arg = NULL;

    // Check if DMA channels are available
    if (_spi->data->dma_channel_rx && _spi->data->dma_channel_tx)
    {
        // Yes, use DMA
        cancel_dma(_spi);
    }
    else
    {
        // No, use Interrupt
        cancel_interrupt(_spi);
    }
}

static inline void transfer_dma(const _spi_t *_spi, const uint8_t *tx_buffer,
                                uint8_t *rx_buffer, uint16_t length)
{
    // Set a static rx and tx in case tx_buffer or rx_buffer are not provided
    static uint8_t tx, rx;

    // Configure the RX DMA channel
    dma_config(
        // The DMA channel
        _spi->data->dma_channel_rx,
        // the Peripheral Address (SPI Data Register)
        (uint32_t) spi_get_DR(_spi),
        // The Memory address: if rx_buffer is provided, use it
        (uint32_t)(rx_buffer ? rx_buffer : &rx),
        // The length
        length,
        // The Transfer size: 8bit
        DMA_SIZE_8bit,
        // Direction: from peripheral to memory
        DMA_DIRECTION_FROM_PERIPHERAL,
        // the memory increment: ON if rx_buffer provided
        rx_buffer ? DMA_INCREMENT_ON : DMA_INCREMENT_OFF);

    // Configure the TX DMA channel
    dma_config(
        // The DMA channel
        _spi->data->dma_channel_tx,
        // the Peripheral Address (SPI Data Register)
        (uint32_t) spi_get_DR(_spi),
        // The Memory address: if rx_buffer is provided, use it
        (uint32_t)(tx_buffer ? tx_buffer : &tx),
        // The length
        length,
        // The Transfer size: 8bit
        DMA_SIZE_8bit, DMA_DIRECTION_TO_PERIPHERAL,
        // Direction: from peripheral to memory
        tx_buffer ? DMA_INCREMENT_ON : DMA_INCREMENT_OFF);

    // Start the RX DMA, with transfer done handler HACK for removing const warning
    dma_start(_spi->data->dma_channel_rx, (handler_t) transfer_done, (handler_arg_t) (uint32_t) _spi);

    // Start the TX DMA, with no handler
    dma_start(_spi->data->dma_channel_tx, NULL, NULL);

    // Enable the DMA trigger generation
    *spi_get_CR2(_spi) = SPI_CR2__RXDMAEN | SPI_CR2__TXDMAEN;
}

static inline void transfer_interrupt(const _spi_t *_spi, const uint8_t *tx_buffer,
                                      uint8_t *rx_buffer, uint16_t length)
{
    // Store the length and pointers
    _spi->data->isr_tx = tx_buffer;
    _spi->data->isr_rx = rx_buffer;
    _spi->data->isr_count = length;

    // Clear any pending RX
    (void) *spi_get_DR(_spi);

    // Enable RX and TX interrupt
    *spi_get_CR2(_spi) = SPI_CR2__TXEIE | SPI_CR2__RXNEIE;
}
static inline void cancel_dma(const _spi_t *_spi)
{
    // Disable the DMA trigger generation
    *spi_get_CR2(_spi) = 0;

    // Cancel the transfers
    dma_cancel(_spi->data->dma_channel_rx);

    if (dma_cancel(_spi->data->dma_channel_tx))
    {
        // Release low power if aborted
        platform_release_low_power();
    }
}

static inline void cancel_interrupt(const _spi_t *_spi)
{
    // Disable RX and TX interrupt
    *spi_get_CR2(_spi) = 0;

    // Check if transfer not finished
    if (_spi->data->isr_count != 0)
    {
        platform_release_low_power();
    }
}
static void transfer_done(const _spi_t *_spi)
{
    // Notify of ongoing underground transfer terminated
    platform_release_low_power();

    // Call handler if any
    if (_spi->data->transfer_handler)
    {
        _spi->data->transfer_handler(_spi->data->transfer_handler_arg);
    }
}

void spi_handle_interrupt(const _spi_t *_spi)
{
    // Read SR only once
    uint8_t sr = *spi_get_SR(_spi);

    // Switch on the interrupt source
    if (sr & SPI_SR__RXNE)
    {
        // RX ready, read one byte
        uint8_t rx = *spi_get_DR(_spi);

        // Store if required
        if (_spi->data->isr_rx)
        {
            // Store received byte and increment
            *(_spi->data->isr_rx++) = rx;
        }

        // Decrement transfer count
        _spi->data->isr_count--;

        // Check if transfer done
        if (_spi->data->isr_count == 0)
        {
            // Disable interrupt
            *spi_get_CR2(_spi) = 0;

            // Call transfer done and return
            transfer_done(_spi);
            return;
        }
    }

    if (sr & SPI_SR__TXE)
    {
        // TX empty, write next byte if there are to write
        if (_spi->data->isr_count > 0)
        {
            // Write next byte
            *spi_get_DR(_spi) = _spi->data->isr_tx ? *(_spi->data->isr_tx++) : 0;
        }
        else
        {
            // Disable TX interrupt
            *spi_get_CR2(_spi) &= ~SPI_CR2__TXEIE;
        }
    }
}
