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

/**
 * \file adc.h
 *
 * \date Oct 12, 2011
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef ADC_H_
#define ADC_H_

/**
 * \addtogroup drivers
 * @{
 */

/**
 * \defgroup ADC ADC driver
 *
 * This driver provides all functions required to perform ADC conversions on
 * a selected channel.
 *
 *@{
 */

#include <stdint.h>
#include "handler.h"

/** Abstract pointer representing an ADC */
typedef const void *adc_t;

/**
 * ADC handler function used to be notified of end of conversion, and to get
 * the conversion result.
 *
 * \param arg the argument provided when registering the handler
 * \param value the 12bit ADC conversion result
 */
typedef void (*adc_handler_t)(handler_arg_t arg, uint16_t value);

enum
{
    ADC_CHANNEL_TEMPERATURE = 16,
    ADC_CHANNEL_VREFINT = 17
};

/**
 * Enable an ADC.
 *
 * This powers up the ADC.
 *
 * \param adc the ADC to enable
 */
void adc_enable(adc_t adc);
/**
 * Disable an ADC
 *
 * This powers down the ADC, saving power.
 *
 * \param adc the ADC to disable
 */
void adc_disable(adc_t adc);

/**
 *  Get exclusivity over the ADC
 *
 *  \param adc the adc to request
 *  \return 1 if ADC taken, 0 if busy
 */
int32_t adc_take(adc_t adc);
/**
 * Release the ADC
 * \param adc the ADC to release
 */
void adc_release(adc_t adc);

/**
 * Set the end of conversion handler.
 *
 * The provided handler will be called on end of the next conversion.
 *
 * \param adc the ADC to configure
 * \param handler the handler function to register
 * \param arg the argument to provide to the handler
 */
void adc_set_handler(adc_t adc, adc_handler_t handler, handler_arg_t arg);

/**
 * Configure a channel sampling time.
 *
 * The sampling time should be a value from 0 to 7, refer to the STM32 datasheet
 * for the corresponding real time.
 *
 * \param adc the ADC to configure
 * \param channel the single channel to sample
 * \param sampling_time the ADC sampling time to set
 */
void adc_config_channel(adc_t adc, uint8_t channel, uint8_t sampling_time);

/**
 * Prepare the ADC for single conversion specifying the channel to use
 *
 * \note The ADC channel MUST be prepared with this method before being sampled
 * using \ref adc_sample_single.
 *
 * \param adc the ADC to configure
 * \param channel the single channel to sample
 */
void adc_prepare_single(adc_t adc, uint8_t channel);

/**
 * Start a single conversion.
 *
 * This starts the conversion of the configured ADC channel (selected with
 * \ref adc_prepare_single). Once the conversion is terminated, the handler
 * registered with \ref adc_set_handler will be called with the conversion result
 * in parameter.
 *
 * \param adc the ADC to sample
 */
void adc_sample_single(adc_t adc);

/**
 * Enable VrefINT for sampling it and computing Vcc
 */
void adc_enable_vrefint();

/**
 * Get the factory measure of Vrefint when Vcc is 3V
 * \return the vrefint value
 */
uint16_t adc_get_factory_3v_vrefint();

/**
 * Disable VrefINT
 */
void adc_disable_vrefint();

/**
 * @}
 * @}
 */

#endif /* ADC_H_ */
