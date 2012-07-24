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
 * adc.h
 *
 *  Created on: Oct 12, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include "handler.h"

/** Abstract pointer representing an ADC */
typedef void *adc_t;

typedef void (*adc_handler_t)(handler_arg_t arg, uint16_t value);

/**
 * Enable an ADC
 *
 * \param adc the ADC to enable
 */
void adc_enable(adc_t adc);
/**
 * Disable an ADC
 *
 * \param adc the ADC to disable
 */
void adc_disable(adc_t adc);

/**
 * Set the end of conversion handler.
 */
void adc_set_handler(adc_t adc, adc_handler_t handler, handler_arg_t arg);

/**
 * Configure a channel sampling time
 *
 * \param adc the ADC to configure
 * \param channel the single channel to sample
 * \param sampling_time the sampling time to set
 */
void adc_config_channel(adc_t adc, uint8_t channel, uint8_t sampling_time);

/**
 * Prepare the ADC for single conversion specifying the channel to use
 *
 * \param adc the ADC to configure
 * \param channel the single channel to sample
 */
void adc_prepare_single(adc_t adc, uint8_t channel);

/**
 * Start a single conversion.
 */
void adc_sample_single(adc_t adc);

#endif /* ADC_H_ */
