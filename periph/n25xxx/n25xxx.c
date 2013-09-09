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
 * Copyright (C) 2012 HiKoB.
 */

/**
 * \file n25xxx.c
 * \date May 18, 2012
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "gpio.h"
#include "spi.h"
#include "n25xxx.h"
#include "n25xxx_.h"
#include "n25xxx_regs.h"

/** This type defines a flash chip
 */
static struct
{
    // To define a flash chip, we need an SPI link
    spi_t spi;

    // CSn pin
    gpio_t csn_gpio;
    gpio_pin_t csn_pin;

    // Wn pin
    gpio_t wn_gpio;
    gpio_pin_t wn_pin;

    // HOLDn pin
    gpio_t holdn_gpio;
    gpio_pin_t holdn_pin;
} flash;

/* Handy functions */
inline static void csn_set()
{
    gpio_pin_set(flash.csn_gpio, flash.csn_pin);
}

inline static void csn_clear()
{
    gpio_pin_clear(flash.csn_gpio, flash.csn_pin);
}

inline static void wn_set()
{
    gpio_pin_set(flash.wn_gpio, flash.wn_pin);
}

inline static void wn_clear()
{
    gpio_pin_clear(flash.wn_gpio, flash.wn_pin);
}

inline static void holdn_set()
{
    gpio_pin_set(flash.holdn_gpio, flash.holdn_pin);
}

inline static void holdn_clear()
{
    gpio_pin_clear(flash.holdn_gpio, flash.holdn_pin);
}

/* Configuration */
void n25xxx_config(spi_t spi,
                   gpio_t csn_gpio, gpio_pin_t csn_pin,
                   gpio_t wn_gpio, gpio_pin_t wn_pin,
                   gpio_t holdn_gpio, gpio_pin_t holdn_pin)
{
    flash.spi = spi;
    flash.csn_gpio = csn_gpio;
    flash.csn_pin = csn_pin;
    flash.wn_gpio = wn_gpio;
    flash.wn_pin = wn_pin;
    flash.holdn_gpio = holdn_gpio;
    flash.holdn_pin = holdn_pin;

    wn_set();
    holdn_set();
}

void n25xxx_init()
{
    // 1: enable the ports
    gpio_enable(flash.csn_gpio);
    gpio_enable(flash.wn_gpio);
    gpio_enable(flash.holdn_gpio);

    // 2: set output/input
    gpio_set_output(flash.csn_gpio, flash.csn_pin);
    gpio_set_output(flash.wn_gpio, flash.wn_pin);
    gpio_set_output(flash.holdn_gpio, flash.holdn_pin);

    // Set CSn, Wn and HOLDn high
    csn_set();
    wn_set();
    holdn_set();
}

void n25xxx_read_id(uint8_t *id, uint16_t len)
{
    // Start the SPI transfer
    csn_clear();

    // Send instructions
    spi_transfer_single(flash.spi, N25XXX_INS__RDID);

    // Read the ID value (max 20 bytes)
    spi_transfer(flash.spi, NULL, id, len <= 20 ? len : 20);

    // End the SPI transfer
    csn_set();
}

void n25xxx_read(uint32_t address, uint8_t *buf, uint16_t len)
{
    uint8_t ad[3] = {address >> 16, address >> 8, address};

    // Start the SPI transfer
    csn_clear();

    // Send instruction
    spi_transfer_single(flash.spi, N25XXX_INS__READ);

    // Send the address
    spi_transfer(flash.spi, ad, NULL, 3);

    // Read the data
    spi_transfer(flash.spi, NULL, buf, len);

    // End the SPI transfer
    csn_set();
}

static uint8_t n25xxx_read_status()
{
    uint8_t t;

    // Start the SPI transfer
    csn_clear();

    // Send the instruction
    spi_transfer_single(flash.spi, N25XXX_INS__RDSR);

    // Read the 16 bits of status register
    spi_transfer(flash.spi, NULL, &t, 1);

    // End the SPI transfer
    csn_set();

    return t;
}

void n25xxx_write_enable()
{
    // Start the SPI transfer
    csn_clear();

    // Send instruction
    spi_transfer_single(flash.spi, N25XXX_INS__WREN);

    // End the SPI transfer
    csn_set();

    // Wait for the WEL bit to be set
    while (!(n25xxx_read_status() & 0x2))
    {
        ;
    }
}

void n25xxx_write_disable()
{
    // Start the SPI transfer
    csn_clear();

    // Send instruction
    spi_transfer_single(flash.spi, N25XXX_INS__WRDI);

    // End the SPI transfer
    csn_set();

    // Wait for the WEL bit to be set
    while (n25xxx_read_status() & 0x2)
    {
        ;
    }
}

void n25xxx_write_page(uint32_t address, uint8_t *buf)
{
    uint8_t ad[3] = {address >> 16, address >> 8, 0};

    // Start the SPI transfer
    csn_clear();

    // Send instruction
    spi_transfer_single(flash.spi, N25XXX_INS__PP);

    // Send the address
    spi_transfer(flash.spi, ad, NULL, 3);

    // Write the data
    spi_transfer(flash.spi, buf, NULL, 256);

    // End the SPI transfer
    csn_set();

    // Wait for the WIP bit to be cleared
    while (n25xxx_read_status() & 0x1)
    {
        ;
    }
}

void n25xxx_erase_subsector(uint32_t address)
{
    uint8_t ad[3] = {address >> 16, address >> 8, 0};

    // Start the SPI transfer
    csn_clear();

    // Send instruction
    spi_transfer_single(flash.spi, N25XXX_INS__SSE);

    // Send the address
    spi_transfer(flash.spi, ad, NULL, 3);

    // End the SPI transfer
    csn_set();

    // Wait for the WIP bit to be cleared
    while (n25xxx_read_status() & 0x1)
    {
        ;
    }
}

void n25xxx_erase_sector(uint32_t address)
{
    uint8_t ad[3] = {address >> 16, 0, 0};

    // Start the SPI transfer
    csn_clear();

    // Send instruction
    spi_transfer_single(flash.spi, N25XXX_INS__SE);

    // Send the address
    spi_transfer(flash.spi, ad, NULL, 3);

    // End the SPI transfer
    csn_set();

    // Wait for the WIP bit to be cleared
    while (n25xxx_read_status() & 0x1)
    {
        ;
    }
}

void n25xxx_bulk_erase()
{
    // Start the SPI transfer
    csn_clear();

    // Send instruction
    spi_transfer_single(flash.spi, N25XXX_INS__BE);

    // End the SPI transfer
    csn_set();

    // Wait for the WIP bit to be cleared
    while (n25xxx_read_status() & 0x1)
    {
        ;
    }
}
