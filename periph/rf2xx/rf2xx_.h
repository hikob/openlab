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

#include "rf2xx.h"
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
    uint32_t slp_tr_af;

    // We need the IRQ pin EXTI line
    exti_line_t irq_exti_line;

    // We need to know the DIG2 pin Timer and Timer channel
    openlab_timer_t dig2_timer;
    uint8_t dig2_channel;

    // The external PA enable pin
    gpio_t pa_enable_gpio;
    gpio_pin_t pa_enable_pin;

    // The type of radio
    rf2xx_type_t type;

} _rf2xx_config_t;

typedef struct
{
    /** A pointer to the configuration structure */
    const _rf2xx_config_t *config;

    // We need a handler for a transfer done event
    handler_t transfer_handler;
    handler_arg_t transfer_arg;

    // We need a timer handler for the DIG2 pin
    timer_handler_t dig2_handler;
    handler_arg_t dig2_arg;

} _rf2xx_t;

void rf2xx_init(rf2xx_t radio);

#endif /* RF2XX__H_ */
