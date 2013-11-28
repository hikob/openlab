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
 * _timer.c
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "timer.h"
#include "timer_.h"
#include "timer_registers.h"
#include "rcc.h"
#include "nvic_.h"

#include "printf.h"

void timer_enable(openlab_timer_t timer)
{
    const _openlab_timer_t *_timer = timer;

    // Enable the clock for TIMx in the RCC registers
    rcc_apb_enable(_timer->apb_bus, _timer->apb_bit);

    // Clear the registers
    *timer_get_CR1(_timer) = 0;
    *timer_get_CR2(_timer) = 0;
    *timer_get_SMCR(_timer) = 0;
    *timer_get_DIER(_timer) = 0;
    *timer_get_SR(_timer) = 0;
    *timer_get_EGR(_timer) = 0;
    *timer_get_CCMRx(_timer, 1) = 0;
    *timer_get_CCMRx(_timer, 2) = 0;
    *timer_get_CCER(_timer) = 0;

    // Enable the interrupt in the NVIC
    nvic_enable_interrupt_line(_timer->irq_line);
}
void timer_disable(openlab_timer_t timer)
{
    const _openlab_timer_t *_timer = timer;

    // Clear the TIMx_CR1 register (to stop it)
    *timer_get_CR1(_timer) = 0;

    // Disable the interrupt in the NVIC
    nvic_disable_interrupt_line(_timer->irq_line);

    // Stop the clock in the RCC registers
    rcc_apb_disable(_timer->apb_bus, _timer->apb_bit);
}

void timer_select_internal_clock(openlab_timer_t timer, uint16_t prescaler)
{
    const _openlab_timer_t *_timer = timer;

    // Disable slave mode
    *timer_get_SMCR(_timer) &= ~TIMER_SMCR__SMS_MASK;

    // Set the prescaler
    *timer_get_PSC(_timer) = prescaler;

    // Compute and save the timer frequency
    uint32_t freq;

    // Get internal frequency
    if (_timer->apb_bus == 1)
    {
        freq = rcc_sysclk_get_clock_frequency(RCC_SYSCLK_CLOCK_PCLK1_TIM);
    }
    else
    {
        freq = rcc_sysclk_get_clock_frequency(RCC_SYSCLK_CLOCK_PCLK2_TIM);
    }

    // Divide by prescaler
    freq /= (prescaler + 1);

    // Store
    _timer->data->frequency = freq;
}

void timer_select_external_clock(openlab_timer_t timer, uint16_t prescaler)
{
    const _openlab_timer_t *_timer = timer;

    // Write ETF = 0 in SMCR
    *timer_get_SMCR(_timer) &= ~TIMER_SMCR__ETF_MASK;

    // Set prescaler = 1
    *timer_get_SMCR(_timer) &= ~TIMER_SMCR__ETPS_MASK;

    // Set rising edge (ETP=0)
    *timer_get_SMCR(_timer) &= ~TIMER_SMCR__ETP;

    // Enable external clock mode 2
    *timer_get_SMCR(_timer) |= TIMER_SMCR__ECE;

    // Set the prescaler
    *timer_get_PSC(_timer) = prescaler;

    // Compute and save the timer frequency, assuming external clock is 32k
    uint32_t freq;
    freq = 32768;

    // Divide by prescaler
    freq /= (prescaler + 1);

    // Store
    _timer->data->frequency = freq;
}

void timer_start(openlab_timer_t timer, uint16_t update_value,
                 timer_handler_t update_handler, handler_arg_t update_arg)
{
    const _openlab_timer_t *_timer = timer;

    // Disable interrupt generation
    *timer_get_DIER_bitband(_timer, TIMER_DIER__UIE_bit) = 0;

    // Clear DIR, CMS, CKD
    *timer_get_CR1(timer) = 0;

    // Store the handler
    _timer->data->update_handler = update_handler;
    _timer->data->update_handler_arg = update_arg;

    // Set the auto reload value
    *timer_get_ARR(_timer) = update_value;

    // Force ARR reload
    *timer_get_EGR(timer) |= TIMER_EGR__UG;

    // Set SMS bits
    *timer_get_SMCR(_timer) &= ~TIMER_SMCR__SMS_MASK;

    // Check if the interrupt should be enabled or disabled
    if (update_handler)
    {
        // Enable interrupt generation for update event
        *timer_get_DIER_bitband(_timer, TIMER_DIER__UIE_bit) = 1;
    }

    // Enable CR1 ARPE
    *timer_get_CR1(timer) |= TIMER_CR1__ARPE;

    // Enable the counter
    *timer_get_CR1(_timer) |= TIMER_CR1__CEN;
}

void timer_stop(openlab_timer_t timer)
{
    const _openlab_timer_t *_timer = timer;

    // Disable the counter
    *timer_get_CR1(_timer) &= ~TIMER_CR1__CEN;
}

uint16_t timer_time(openlab_timer_t timer)
{
    return *timer_get_CNT(timer);
}
void timer_tick_update(openlab_timer_t timer, int16_t dt)
{
    (*timer_get_CNT(timer)) += dt;
}

uint32_t timer_get_frequency(openlab_timer_t timer)
{
    const _openlab_timer_t *_timer = timer;

    if (_timer->data->frequency != 0)
    {
        // Return the estimated frequency
        return _timer->data->frequency;
    }
    else
    {
        // Assume internal clock
        uint32_t freq;

        // Get internal frequency
        if (_timer->apb_bus == 1)
        {
            freq = rcc_sysclk_get_clock_frequency(RCC_SYSCLK_CLOCK_PCLK1_TIM);
        }
        else
        {
            freq = rcc_sysclk_get_clock_frequency(RCC_SYSCLK_CLOCK_PCLK2_TIM);
        }

        return freq;
    }
}

uint16_t timer_get_number_of_channels(openlab_timer_t timer)
{
    const _openlab_timer_t *_timer = timer;
    return _timer->number_of_channels;
}

uint32_t timer_get_update_flag(openlab_timer_t timer)
{
    const _openlab_timer_t *_timer = timer;
    return *timer_get_SR_bitband(_timer, TIMER_SR__UIF_bit);
}
void timer_set_channel_compare(openlab_timer_t timer, timer_channel_t channel,
                               uint16_t compare_value, timer_handler_t handler, handler_arg_t arg)
{
    const _openlab_timer_t *_timer = timer;

    // Disable the interrupt
    *timer_get_DIER_bitband(_timer, TIMER_DIER__CC1IE_bit + channel) = 0;

    // Get CCMR register
    volatile uint16_t *ccmr;
    ccmr = timer_get_CCMRx(_timer, (channel < TIMER_CHANNEL_3) ? 1 : 2);

    // Get bit offset
    uint8_t ccmr_offset;
    ccmr_offset = (channel & 1) ? 8 : 0;

    // Clear CCER channel settings
    *timer_get_CCER(timer) &= ~((TIMER_CCER__CC1NP | TIMER_CCER__CC1P
                                 | TIMER_CCER__CC1E) << (4 * channel));

    // Clear CCMR channel settings
    *ccmr &= ~((TIMER_CCMRx__OCxM__MASK | TIMER_CCMRx__CCxS__MASK)
               << ccmr_offset);

    // Store compare match value
    *timer_get_CCRx(_timer, channel) = compare_value;

    // Store the handler
    _timer->channel_handlers[channel] = handler;
    _timer->channel_handler_args[channel] = arg;

    // clear the interrupt flag
    *timer_get_SR(_timer) = ~(TIMER_SR__CC1IF << channel);

    // Enable interrupt if required
    if (handler)
    {
        *timer_get_DIER_bitband(_timer, TIMER_DIER__CC1IE_bit + channel) = 1;
    }
}

void timer_activate_channel_output(openlab_timer_t timer, timer_channel_t channel,
                                   timer_output_mode_t mode)
{
    const _openlab_timer_t *_timer = timer;

    // Get CCMR register
    volatile uint16_t *ccmr;
    ccmr = timer_get_CCMRx(_timer, (channel < TIMER_CHANNEL_3) ? 1 : 2);

    // Get bit offset
    uint8_t ccmr_offset;
    ccmr_offset = (channel & 1) ? 8 : 0;

    // Set output compare mode
    *ccmr &= ~(0x7 << (4 + ccmr_offset));

    if (mode)
    {
        *ccmr |= mode << (4 + ccmr_offset);

        switch (mode)
        {
            case TIMER_OUTPUT_MODE_PWM1:
            case TIMER_OUTPUT_MODE_PWM2:
                // Enable preload for PWM only!
                *ccmr |= (TIMER_CCMRx__OCxPE << ccmr_offset);
                break;
            default:
                // Otherwise disable preload
                *ccmr &= ~(TIMER_CCMRx__OCxPE << ccmr_offset);
                break;
        }

        // Enable output state
        *timer_get_CCER(timer) |= TIMER_CCER__CC1E << (4 * channel);
    }
    else
    {
        // Disable output state
        *timer_get_CCER(timer) &= ~(TIMER_CCER__CC1E << (4 * channel));
        *ccmr &= ~(TIMER_CCMRx__OCxPE << ccmr_offset);
    }
}

void timer_update_channel_compare(openlab_timer_t timer, timer_channel_t channel,
                                  uint16_t value)
{
    const _openlab_timer_t *_timer = timer;

    // Store the new compare match value
    *timer_get_CCRx(_timer, channel) = value;
}

void timer_set_channel_capture(openlab_timer_t timer, timer_channel_t channel,
                               timer_capture_edge_t signal_edge, timer_handler_t handler,
                               handler_arg_t arg)
{
    const _openlab_timer_t *_timer = timer;

    // Disable the interruption
    *timer_get_DIER_bitband(_timer, TIMER_DIER__CC1IE_bit + channel) = 0;

    // Select Capture, Set 01 to CCxS in CCMRx register (no filter, no prescaler)

    // Get CCMR register
    volatile uint16_t *ccmr;
    ccmr = timer_get_CCMRx(_timer, channel < 2);

    // Get bit offset
    uint8_t ccmr_offset;
    ccmr_offset = 8 * (channel & 1);

    // Clear the CCER register
    *timer_get_CCER(_timer) &= ~(TIMER_CCER__CC1_MASK);

    // Clear the CCMR register
    *ccmr &= ~(0xF << ccmr_offset);

    // If handler is NULL, stop here
    if (handler == NULL)
    {
        return;
    }

    // Set CCMR register value
    *ccmr |= TIMER_CCMRx__CCxS__INPUT1 << (ccmr_offset);

    // Select the trigger edge by setting the CCxP and CCxNP bits of the CCER register
    uint8_t value = 0;

    switch (signal_edge)
    {
        case TIMER_CAPTURE_EDGE_RISING:
            break;
        case TIMER_CAPTURE_EDGE_FALLING:
            value = TIMER_CCER__CC1P;
            break;
        case TIMER_CAPTURE_EDGE_BOTH:
            value = TIMER_CCER__CC1P | TIMER_CCER__CC1NP;
            break;
    }
    *timer_get_CCER(_timer) |= value << (4 * channel);

    // Enable the the capture, setting CCxIE in the CCER register
    *timer_get_CCER(_timer) |= TIMER_CCER__CC1E << (4 * channel);

    // Store the handler
    _timer->channel_handlers[channel] = handler;
    _timer->channel_handler_args[channel] = arg;

    // Clear the interrupt flag
    *timer_get_SR(_timer) = ~(TIMER_SR__CC1IF << channel);

    // Enable the interrupt
    *timer_get_DIER_bitband(_timer, TIMER_DIER__CC1IE_bit + channel) = 1;
}

void timer_handle_interrupt(const _openlab_timer_t *_timer)
{
    // Check source of interrupt
    uint16_t sr_dier = *timer_get_SR(_timer) & *timer_get_DIER(_timer);

    // Update event
    if (sr_dier & TIMER_SR__UIF)
    {
        // Clear flag
        *timer_get_SR(_timer) = ~TIMER_SR__UIF;

        // Call handler
        _timer->data->update_handler(_timer->data->update_handler_arg, *timer_get_ARR(
                                       _timer));
    }

    // Channel event
    int i;
    for (i = 0; i < _timer->number_of_channels; i++)
    {
        // Channel i capture/compare event
        if (sr_dier & (TIMER_SR__CC1IF << i))
        {
            // Clear flag
            *timer_get_SR(_timer) = ~(TIMER_SR__CC1IF << i);

            // Call handler
            _timer->channel_handlers[i](_timer->channel_handler_args[i],
                                        *timer_get_CCRx(_timer, i));
        }
    }
}
