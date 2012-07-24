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
 * pga308_.h
 *
 *  Created on: Nov 21, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef PGA308__H_
#define PGA308__H_

#include "pga308.h"
#include "adc.h"
#include "uart.h"
#include "gpio.h"

/**
 * Initialize the PGA308, specifying its UART to use, and the GPIO to enable it
 *
 * \param uart the uart to use for communication;
 * \param gpio the gpio of the enable bit;
 * \param enable_pin the pin to enable the PGA308;
 * \param adc the adc driver;
 * \param adc_channel the ADC channel of the PGA308 output pin;
 */
void pga308_init(uart_t uart, gpio_t gpio, gpio_pin_t enable_pin,
                 adc_t adc, uint8_t adc_channel);

#endif /* PGA308__H_ */
