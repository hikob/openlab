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
 * \file n25xxx_.h
 * \date May 18, 2012
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */
#ifndef N25XXX__H_
#define N25XXX__H_

#include "n25xxx.h"

/** Configure all the pins for the Flash */
void n25xxx_config(spi_t spi,
                   gpio_t csn_gpio, gpio_pin_t csn_pin,
                   gpio_t wn_gpio, gpio_pin_t wn_pin,
                   gpio_t holdn_gpio, gpio_pin_t holdn_pin);

/** Initialize the Flash */
void n25xxx_init();

#endif /* N25XXX__H_ */
