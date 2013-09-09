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

/**
 * \file spi.h
 * \date Jul 7, 2011
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef SPI_H_
#define SPI_H_

/**
 * \addtogroup drivers
 * @{
 */

/**
 * \defgroup SPI SPI master driver
 *
 * This driver provides all functions required to perform SPI data
 * transfers, either synchronously or asynchronously, to a connected slave.
 *
 *@{
 */

#include "handler.h"

/**
 * Abstract representation of a SPI driver.
 */
typedef const void *spi_t;

/**
 * The possible SPI clock modes.
 */
typedef enum
{
    SPI_CLOCK_MODE_IDLE_LOW_RISING = 0,
    SPI_CLOCK_MODE_IDLE_LOW_FALLING = 1,
    SPI_CLOCK_MODE_IDLE_HIGH_RISING = 2,
    SPI_CLOCK_MODE_IDLE_HIGH_FALLING = 3,
} spi_clock_mode_t;

/**
 * Enable a SPI driver.
 *
 * Once configured, a SPI driver should be enabled in order to be
 * operationnal.
 *
 * \param spi the SPI driver to enable;
 * \param baudrate the desired baudrate of the SPI clock;
 * \param clock_mode the desired clock mode;
 */
void spi_enable(spi_t spi, uint32_t baudrate, spi_clock_mode_t clock_mode);
/**
 * Disable a SPI driver.
 *
 * This allows to disable a driver, hence reducing power consumption.
 * Once disabled, the SPI driver must not be used before being enabled
 * back.
 *
 * \param spi the SPI driver to disable;
 */
void spi_disable(spi_t spi);

/**
 * Transfer a single byte over a SPI.
 *
 * \param spi the SPI driver to use;
 * \param tx the byte to send;
 * \return the byte received
 */
uint8_t spi_transfer_single(spi_t spi, uint8_t tx);

/**
 * Transfer a given amount of bytes with a SPI driver.
 *
 * The function is blocking (thanks to FreeRTOS), hence it must not
 * be called from an ISR, only from a task context.
 *
 * \param spi the SPI driver to use;
 * \param tx_buffer a pointer to the buffer to transmit, set it to NULL
 * if the data to transmit is not relevant;
 * \param rx_buffer a pointer to the buffer to store the received data,
 * set it to NULL if the received data is to be discarded;
 * \param length the number of bytes to transmit;
 */
void spi_transfer(spi_t spi, const uint8_t *tx_buffer, uint8_t *rx_buffer,
                  uint16_t length);

/**
 * Transfer a given amount of bytes with a SPI driver, asynchronously.
 *
 * The function is not blocking and the transfer will occur in background.
 * The provided handler function will be called on transfer end.
 *
 * \param spi the SPI driver to use;
 * \param tx_buffer a pointer to the buffer to transmit, set it to NULL
 * if the data to transmit is not relevant;
 * \param rx_buffer a pointer to the buffer to store the received data,
 * set it to NULL if the received data is to be discarded;
 * \param length the number of bytes to transmit;
 * \param handler the handler function to be called once the transfer
 * is ended
 * \param handler_arg the argument to provide to the handler;
 */
void spi_transfer_async(spi_t spi, const uint8_t *tx_buffer,
                        uint8_t *rx_buffer, uint16_t length, handler_t handler,
                        handler_arg_t handler_arg);

/**
 * Cancel an asynchronous transfer
 *
 * \param spi the SPI driver to use
 */
void spi_async_cancel(spi_t spi);

/**
 * @}
 * @}
 */

#endif /* SPI_H_ */
