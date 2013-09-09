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
 * adc_.h
 *
 *  Created on: Nov 5, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef ADC__H_
#define ADC__H_

#include "adc.h"
#include "rcc.h"
#include "nvic.h"
#include "timer.h"

typedef struct
{
    /** Flag indicating if the ADC is taken */
    volatile int32_t taken;

    /** The conversion handler */
    adc_handler_t handler;
    /** The conversion handler argument. */
    handler_arg_t handler_arg;
} _adc_data_t;

typedef struct
{
    /** The ADC block base address */
    uint32_t base_address;
    /** The APB bus */
    rcc_apb_bus_t apb_bus;
    /** The APB bit */
    rcc_apb_bit_t apb_bit;
    /** The NVIC irq line */
    nvic_irq_line_t irq_line;
    // Data
    _adc_data_t *data;
} _adc_t;

#define ADC_INIT(name, addr, bus, bit, line) \
    static _adc_data_t name##_data; \
    const _adc_t name = { \
    .base_address = addr, \
    .apb_bus = bus, \
    .apb_bit = bit, \
    .irq_line = line, \
    .data = &name##_data \
}

/**
 * Handle an interrupt.
 */
void adc_handle_interrupt(const _adc_t *_adc);

#endif /* ADC__H_ */
