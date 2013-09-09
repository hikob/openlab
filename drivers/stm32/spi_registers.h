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
 * spi_registers.h
 *
 *  Created on: Jul 7, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef SPI_REGISTERS_H_
#define SPI_REGISTERS_H_

#include "memmap.h"

static inline volatile uint16_t *spi_get_CR1(const _spi_t *_spi)
{
    return mem_get_reg16(_spi->base_address + SPIx_CR1_OFFSET);
}
static inline volatile uint16_t *spi_get_CR2(const _spi_t *_spi)
{
    return mem_get_reg16(_spi->base_address + SPIx_CR2_OFFSET);
}
static inline volatile uint16_t *spi_get_SR(const _spi_t *_spi)
{
    return mem_get_reg16(_spi->base_address + SPIx_SR_OFFSET);
}
static inline volatile uint16_t *spi_get_DR(const _spi_t *_spi)
{
    return mem_get_reg16(_spi->base_address + SPIx_DR_OFFSET);
}
static inline volatile uint16_t *spi_get_CRCPR(const _spi_t *_spi)
{
    return mem_get_reg16(_spi->base_address + SPIx_CRCPR_OFFSET);
}
enum
{
    SPI_CR1__BIDIMODE = 0x8000,
    SPI_CR1__BIDIOE = 0x4000,
    SPI_CR1__CRCEN = 0x2000,
    SPI_CR1__CRCNEXT = 0x1000,
    SPI_CR1__DFF = 0x0800,
    SPI_CR1__RXONLY = 0x0400,
    SPI_CR1__SSM = 0x0200,
    SPI_CR1__SSI = 0x0100,
    SPI_CR1__LSBFIRST = 0x0080,
    SPI_CR1__SPE = 0x0040,
    SPI_CR1__BR_MASK = 0x0038,
    SPI_CR1__MSTR = 0x0004,
    SPI_CR1__CPOL = 0x0002,
    SPI_CR1__CPHA = 0x0001,
};

enum
{
    SPI_CR2__TXEIE = 0x80,
    SPI_CR2__RXNEIE = 0x40,
    SPI_CR2__ERRIE = 0x20,
    SPI_CR2__SSOE = 0x4,
    SPI_CR2__TXDMAEN = 0x2,
    SPI_CR2__RXDMAEN = 0x1,
};
enum
{
    SPI_SR__BSY = 0x80,
    SPI_SR__OVR = 0x40,
    SPI_SR__MODF = 0x20,
    SPI_SR__CRCERR = 0x10,
    SPI_SR__TXE = 0x02,
    SPI_SR__RXNE = 0x01,
};

#endif /* SPI_REGISTERS_H_ */
