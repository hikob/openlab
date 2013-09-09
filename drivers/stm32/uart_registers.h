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
 * uart_registers.h
 *
 *  Created on: Jul 7, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef UART_REGISTERS_H_
#define UART_REGISTERS_H_

#include "uart_.h"
#include "memmap.h"

static inline volatile uint16_t *uart_get_SR(const _uart_t *uart)
{
    return mem_get_reg16(uart->base_address + USARTx_SR_OFFSET);
}
static inline volatile uint16_t *uart_get_DR(const _uart_t *uart)
{
    return mem_get_reg16(uart->base_address + USARTx_DR_OFFSET);
}
static inline volatile uint16_t *uart_get_BRR(const _uart_t *uart)
{
    return mem_get_reg16(uart->base_address + USARTx_BRR_OFFSET);
}
static inline volatile uint16_t *uart_get_CR1(const _uart_t *uart)
{
    return mem_get_reg16(uart->base_address + USARTx_CR1_OFFSET);
}
static inline volatile uint16_t *uart_get_CR2(const _uart_t *uart)
{
    return mem_get_reg16(uart->base_address + USARTx_CR2_OFFSET);
}
static inline volatile uint16_t *uart_get_CR3(const _uart_t *uart)
{
    return mem_get_reg16(uart->base_address + USARTx_CR3_OFFSET);
}
static inline volatile uint16_t *uart_get_GTPR(const _uart_t *uart)
{
    return mem_get_reg16(uart->base_address + USARTx_GTPR_OFFSET);
}

enum
{
    UART_CR1__OVER8 = 0x8000,
    UART_CR1__UE = 0x2000,
    UART_CR1__M = 0x1000,
    UART_CR1__WAKE = 0x800,
    UART_CR1__PCE = 0x400,
    UART_CR1__PS = 0x200,
    UART_CR1__PEIE = 0x100,
    UART_CR1__TXEIE = 0x80,
    UART_CR1__TCIE = 0x40,
    UART_CR1__RXNEIE = 0x20,
    UART_CR1__IDLEIE = 0x10,
    UART_CR1__TE = 0x8,
    UART_CR1__RE = 0x4,
    UART_CR1__RWU = 0x2,
    UART_CR1__SBK = 0x1,
};

enum
{
    UART_CR3__ONEBITE = 0x0800,
    UART_CR3__CTSIE = 0x0400,
    UART_CR3__CTSE = 0x0200,
    UART_CR3__RTSE = 0x0100,
    UART_CR3__DMAT = 0x0080,
    UART_CR3__DMAR = 0x0040,
    UART_CR3__SCEN = 0x0020,
    UART_CR3__NACK = 0x0010,
    UART_CR3__HDSEL = 0x0008,
    UART_CR3__IRLP = 0x0004,
    UART_CR3__IREN = 0x0002,
    UART_CR3__EIE = 0x0001,
};

enum
{
    UART_SR__CTS = 0x200,
    UART_SR__LBD = 0x100,
    UART_SR__TXE = 0x80,
    UART_SR__TC = 0x40,
    UART_SR__RXNE = 0x20,
    UART_SR__IDLE = 0x10,
    UART_SR__ORE = 0x8,
    UART_SR__NF = 0x4,
    UART_SR__FE = 0x2,
    UART_SR__PE = 0x1,
};

#endif /* UART_REGISTERS_H_ */
