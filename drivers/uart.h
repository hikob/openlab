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
 * uart.h
 *
 *  Created on: Jul 8, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

/**
 * UART driver header file.
 *
 */

#ifndef UART_H_
#define UART_H_

#include "gpio.h"
#include "handler.h"

/**
 * Abstract representation of a UART driver.
 */
typedef void *uart_t;
typedef void (*uart_handler_t)(handler_arg_t arg, uint8_t c);

/**
 * Enable a UART driver.
 *
 * A UART driver should be enabled in order to be operational.
 * The baudrate should not be less than 1/8th of the APB clock frequency.
 *
 *
 * \param uart the UART driver to enable;
 * \param baudrate the desired baudrate of the UART clock;
 */
void uart_enable(uart_t uart, uint32_t baudrate);
/**
 * Disable a UART driver.
 *
 * This allows to disable a driver, hence reducing power consumption.
 * Once disabled, the UART driver must not be used before being enabled
 * back.
 *
 * \param uart the UART driver to disable;
 */
void uart_disable(uart_t uart);

/**
 * Set the RX handler function, to be called on each character received.
 *
 * \param uart the UART driver;
 * \param handler the handler function;
 * \param arg the argument to pass to the handler function
 */
void
uart_set_rx_handler(uart_t uart, uart_handler_t handler, handler_arg_t arg);

/**
 * Transfer a given amount of bytes with a UART driver.
 *
 * The function is blocking (thanks to FreeRTOS), hence it must not
 * be called from an ISR, only from a task context.
 *
 * \param uart the UART driver to use;
 * \param tx_buffer a pointer to the buffer to transmit;
 * \param length the number of bytes to transmit;
 */
void uart_transfer(uart_t uart, const uint8_t *tx_buffer, uint16_t length);

/**
 * Transfer a given amount of bytes with a UART driver, asynchronously.
 *
 * The function is not blocking and the transfer will occur in background.
 * The provided handler function will be called on transfer end.
 *
 * \param uart the UART driver to use;
 * \param tx_buffer a pointer to the buffer to transmit;
 * \param length the number of bytes to transmit;
 * \param handler the handler function to be called once the transfer
 * is ended
 * \param handler_arg the argument to provide to the handler;
 */
void uart_transfer_async(uart_t uart, const uint8_t *tx_buffer, uint16_t length,
                         handler_t handler, handler_arg_t handler_arg);

#endif /* UART_H_ */

