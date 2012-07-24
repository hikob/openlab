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
 * rf2xx.h
 *
 *  Created on: Jul 8, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef RF2XX__H_
#define RF2XX__H_

#include "rf2xx_timing.h"
#include "rf2xx_regs.h"
#include "spi.h"
#include "timer.h"
#include "gpio.h"
#include "exti.h"
#include "handler.h"

/**
 * This type defines a radio chip
 */
typedef struct
{
    // To define a radio, we need an SPI link
    spi_t spi;

    // We need to know the CSn pin
    gpio_t csn_gpio;
    gpio_pin_t csn_pin;

    // We need to know the RSTn pin
    gpio_t rstn_gpio;
    gpio_pin_t rstn_pin;

    // We need to know the SLP_TR pin
    gpio_t slp_tr_gpio;
    gpio_pin_t slp_tr_pin;

    // We need the IRQ pin EXTI line
    exti_line_t irq_exti_line;

    // We need to know the DIG2 pin Timer and Timer channel
    timer_t dig2_timer;
    uint8_t dig2_channel;

    // We need a handler for a transfer done event
    handler_t transfer_handler;
    handler_arg_t transfer_arg;

    // We need a timer handler for the DIG2 pin
    timer_handler_t dig2_handler;
    handler_arg_t dig2_arg;

} _rf2xx_t;

/* Configuration */
static inline void rf2xx_config(_rf2xx_t *radio, spi_t spi, gpio_t csn_gpio,
                                gpio_pin_t csn_pin, gpio_t rstn_gpio, gpio_pin_t rstn_pin,
                                gpio_t slp_tr_gpio, gpio_pin_t slp_tr_pin, exti_line_t irq_exti_line,
                                timer_t dig2_timer, uint8_t dig2_channel)
{
    radio->spi = spi;
    radio->csn_gpio = csn_gpio;
    radio->csn_pin = csn_pin;
    radio->rstn_gpio = rstn_gpio;
    radio->rstn_pin = rstn_pin;
    radio->slp_tr_gpio = slp_tr_gpio;
    radio->slp_tr_pin = slp_tr_pin;
    radio->irq_exti_line = irq_exti_line;
    radio->dig2_timer = dig2_timer;
    radio->dig2_channel = dig2_channel;
}

#endif /* RF2XX__H_ */
