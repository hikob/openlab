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
 * adc.c
 *
 *  Created on: Nov 5, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "adc_.h"
#include "adc_registers.h"

void adc_enable(adc_t adc)
{
    const _adc_t *_adc = adc;

    // Enable the ADC in the APB
    rcc_apb_enable(_adc->apb_bus, _adc->apb_bit);

    // Clear the handlers
    _adc->data->handler = NULL;
    _adc->data->handler_arg = NULL;

    // Set the ADON bit to wake it up
    *adc_get_CR2(_adc) |= ADCx_CR2__ADON;

    // Perform calibration
    *adc_get_CR2(_adc) |= ADCx_CR2__CAL;

    // Wait until done
    while (*adc_get_CR2(_adc) & ADCx_CR2__CAL)
    {
    }

    // Disable the ADC (ADON bit)
    *adc_get_CR2(_adc) &= ~ADCx_CR2__ADON;

    // Clear the control registers
    *adc_get_CR1(_adc) = 0;
    *adc_get_CR2(_adc) = 0;
    *adc_get_SQR1(_adc) = 0;
    *adc_get_JSQR(_adc) = 0;

    // Enable the interrupt line in the NVIC
    nvic_enable_interrupt_line(_adc->irq_line);
}

void adc_set_handler(adc_t adc, adc_handler_t handler, handler_arg_t arg)
{
    const _adc_t *_adc = adc;

    // Store handlers
    _adc->data->handler = handler;
    _adc->data->handler_arg = arg;
}

void adc_prepare_single(adc_t adc, uint8_t channel)
{
    const _adc_t *_adc = adc;

    // Clear the sequences length and channel selections
    *adc_get_JSQR(_adc) = 0;
    *adc_get_SQR1(_adc) = 0;
    *adc_get_SQRx(_adc, 2) = 0;
    *adc_get_SQRx(_adc, 3) = 0;
    *adc_get_SMPRx(_adc, 1) = 0;
    *adc_get_SMPRx(_adc, 2) = 0;

    // Store the conversion length to 1
    *adc_get_SQR1(_adc) = 0;

    // Enable interrupt at end of conversion
    *adc_get_CR1(_adc) = ADCx_CR1__EOCIE;
    *adc_get_CR2(_adc) = ADCx_CR2__ADON;

    // Compute the SMPRx of this channel
    uint8_t smprx = 2 - (channel / 10);

    // Set the sampling time for this channel to minimum
    *adc_get_SMPRx(_adc, smprx) = 0;

    // Set the first channel of the sequence
    *adc_get_SQRx(_adc, 3) = channel;
}

void adc_sample_single(adc_t adc)
{
    const _adc_t *_adc = adc;

    // Start ADC sampling
    *adc_get_CR2(_adc) |= ADCx_CR2__ADON;
}

void adc_handle_interrupt(const _adc_t *_adc)
{
    // Read the SR register
    uint16_t sr = *adc_get_SR(_adc);

    // Check EOC
    if (sr & ADCx_SR__EOC)
    {
        // Clear the flag
        *adc_get_SR(_adc) &= ~ADCx_SR__EOC;

        // remember the value
        uint16_t value = *adc_get_DR(_adc);

        // Call handler if any
        if (_adc->data->handler)
        {
            _adc->data->handler(_adc->data->handler_arg, value);
        }
    }
}

