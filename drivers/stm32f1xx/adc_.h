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
#include "dma.h"

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

    /** The conversion handler */
    adc_handler_t handler;
    /** The conversion handler argument. */
    handler_arg_t handler_arg;
} _adc_t;

static inline void adc_init(_adc_t *_adc, uint32_t base_address,
                            rcc_apb_bus_t apb_bus, rcc_apb_bit_t apb_bit, nvic_irq_line_t irq_line)
{
    // Store the information
    _adc->base_address = base_address;
    _adc->apb_bus = apb_bus;
    _adc->apb_bit = apb_bit;
    _adc->irq_line = irq_line;
}

/**
 * Handle an interrupt.
 */
void adc_handle_interrupt(_adc_t *_adc);

#endif /* ADC__H_ */
