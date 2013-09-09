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

/*
 * adc.c
 *
 *  Created on: Oct 12, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include "platform.h"

#include "adc_.h"
#include "adc_registers.h"
#include "rcc.h"
#include "nvic_.h"

#include "debug.h"

void adc_enable(adc_t adc)
{
    const _adc_t *_adc = (const _adc_t *) adc;

    // Enable the ADC in the APB
    rcc_apb_enable(_adc->apb_bus, _adc->apb_bit);

    // Clear the handlers
    _adc->data->handler = NULL;
    _adc->data->handler_arg = NULL;

    // Disable the ADC (ADON bit)
    *adc_get_CR2(_adc) &= ~ADC_CR2__ADON;
    // Clear all registers
    *adc_get_CR1(_adc) = 0;
    *adc_get_CR2(_adc) = 0;
    *adc_get_SQR1(_adc) = 0;
    *adc_get_JSQR(_adc) = 0;

    // Enable the interrupt line in the NVIC
    nvic_enable_interrupt_line(_adc->irq_line);
}
void adc_disable(adc_t adc)
{
    const _adc_t *_adc = (const _adc_t *) adc;

    // Disable the ADC (ADON bit)
    *adc_get_CR2(_adc) &= ~ADC_CR2__ADON;

    // Clear all registers
    *adc_get_CR1(_adc) = 0;
    *adc_get_CR2(_adc) = 0;
    *adc_get_SQR1(_adc) = 0;
    *adc_get_JSQR(_adc) = 0;

    // Disable the interrupt line in the NVIC
    nvic_disable_interrupt_line(_adc->irq_line);

    // Disable the ADC in the APB
    rcc_apb_enable(_adc->apb_bus, _adc->apb_bit);
}

int32_t adc_take(adc_t adc)
{
    const _adc_t *_adc = (const _adc_t *) adc;
    int ret = 0;

    // Disable interrupts
    platform_enter_critical();

    if (_adc->data->taken)
    {
        ret = 0;
    }
    else
    {
        _adc->data->taken = 1;
        ret = 1;
    }

    // Enable interupts
    platform_exit_critical();

    return ret;
}

void adc_release(adc_t adc)
{
    const _adc_t *_adc = (const _adc_t *) adc;

    // Clear taken flag
    _adc->data->taken = 0;
}

void adc_set_handler(adc_t adc, adc_handler_t handler, handler_arg_t arg)
{
    const _adc_t *_adc = (const _adc_t *) adc;

    // Store handlers
    _adc->data->handler = handler;
    _adc->data->handler_arg = arg;
}

void adc_config_channel(adc_t adc, uint8_t channel, uint8_t sampling_time)
{
    const _adc_t *_adc = (const _adc_t *) adc;

    // Compute the smprx register number and offset
    uint8_t smprx = 3 - channel / 10;
    uint8_t smprx_offset = 3 * (channel % 10);

    // Update the conversion time
    *adc_get_SMPRx(_adc, smprx) &= ~0x7 << smprx_offset;
    *adc_get_SMPRx(_adc, smprx) |= (sampling_time & 0x7) << smprx_offset;
}

void adc_prepare_single(adc_t adc, uint8_t channel)
{
    const _adc_t *_adc = (const _adc_t *) adc;

    // Stop the ADC
    *adc_get_CR2(_adc) = 0;

    // Set only EOCIE in CR1 (interrupt enabled at end of conversion)
    *adc_get_CR1(_adc) = ADC_CR1__EOCIE;

    // Set interrupt at end of each conversion in CR2 (do not set ADON)
    *adc_get_CR2(_adc) = ADC_CR2__EOCS;

    // Set 1 channel length sequence
    *adc_get_SQR1(_adc) = 0;

    // Set the first channel of the sequence
    *adc_get_SQRx(_adc, 5) = channel;
}
void adc_sample_single(adc_t adc)
{
    const _adc_t *_adc = (const _adc_t *) adc;

    // Prevent HSI disabling
    platform_prevent_low_power();

    // Start the ADC
    *adc_get_CR2(_adc) |= ADC_CR2__ADON;

    // Wait until started
    while ((*adc_get_SR(_adc) & ADC_SR__ADONS) == 0)
    {
    }

    // Start conversion
    *adc_get_CR2(_adc) |= ADC_CR2__SWSTART;
}

void adc_enable_vrefint()
{
    // Set the TSVREFE bit in CCR
    *adc_get_CCR() |= ADC_CCR__TSVREFE;
}

uint16_t adc_get_factory_3v_vrefint()
{
    return *adc_get_VREFINT_FACTORY();
}
void adc_disable_vrefint()
{
    // Clear the TSVREFE bit in CCR
    *adc_get_CCR() &= ~ADC_CCR__TSVREFE;
}

void adc_handle_interrupt(const _adc_t *_adc)
{
    // Read the SR register
    uint16_t sr = *adc_get_SR(_adc);

    // Check EOC
    if (sr & ADC_SR__EOC)
    {
        // Clear the flag
        *adc_get_SR(_adc) &= ~(ADC_SR__EOC | ADC_SR__STRT);

        // remember the value
        uint16_t value = *adc_get_DR(_adc);

        // Stop the ADC
        *adc_get_CR2(_adc) &= ~ADC_CR2__ADON;

        // Release HSI disabling  prevention
        platform_release_low_power();

        // Call handler if any
        if (_adc->data->handler)
        {
            _adc->data->handler(_adc->data->handler_arg, value);
        }
    }
}

