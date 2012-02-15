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
 * Copyright (C) 2011 HiKoB.
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

#include "printf.h"

void timer_enable(timer_t timer)
{
    _timer_t *_timer = timer;

    // Enable the clock for TIMx in the RCC registers
    rcc_apb_enable(_timer->apb_bus, _timer->apb_bit);

    // Clear the registers
    *timer_get_CR1(_timer) = 0;
    *timer_get_CR2(_timer) = 0;
    *timer_get_SMCR(_timer) = 0;
    *timer_get_DIER(_timer) = 0;
    *timer_get_SR(_timer) = 0;
    *timer_get_EGR(_timer) = 0;
    *timer_get_CCMR1(_timer) = 0;
    *timer_get_CCMR2(_timer) = 0;
    *timer_get_CCER(_timer) = 0;

    // Enable the interrupt in the NVIC
    nvic_enable_interrupt_line(_timer->irq_line);
}
void timer_disable(timer_t timer)
{
    _timer_t *_timer = timer;

    // Clear the TIMx_CR1 register (to stop it)
    *timer_get_CR1(_timer) = 0;

    // Disable the interrupt in the NVIC
    nvic_disable_interrupt_line(_timer->irq_line);

    // Stop the clock in the RCC registers
    rcc_apb_disable(_timer->apb_bus, _timer->apb_bit);
}

void timer_select_internal_clock(timer_t timer, uint16_t prescaler)
{
    _timer_t *_timer = timer;

    // Disable slave mode
    *timer_get_SMCR(_timer) &= ~TIMER_SMCR__SMS_MASK;

    // Set the prescaler
    *timer_get_PSC(_timer) = prescaler;

    // Compute and save the timer frequency
    uint32_t freq;

    // Get internal frequency
    if(_timer->apb_bus == 1)
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
    _timer->frequency = freq;
}

void timer_select_external_clock(timer_t timer, uint16_t prescaler)
{
    _timer_t *_timer = timer;

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

    // Save the timer frequency, assuming external clock is 32k
    uint32_t freq;
    freq = 32768;

    // Divide by prescaler
    freq /= (prescaler + 1);

    // Store
    _timer->frequency = freq;
}

void timer_start(timer_t timer, uint16_t update_value,
                 timer_handler_t update_handler, handler_arg_t update_arg)
{
    _timer_t *_timer = timer;

    // Store the handler
    _timer->update_handler = update_handler;
    _timer->update_handler_arg = update_arg;

    // Set the auto reload value
    *timer_get_ARR(_timer) = update_value;

    // Check if the interrupt should be enabled or disabled
    if(update_handler)
    {
        // Enable interrupt generation for update event
        *timer_get_DIER(_timer) |= TIMER_DIER__UIE;
    }
    else
    {
        // Disable interrupt generation
        *timer_get_DIER(_timer) &= ~TIMER_DIER__UIE;
    }

    // Enable the counter
    *timer_get_CR1(_timer) = TIMER_CR1__CEN;
}

void timer_restart(timer_t timer)
{
    _timer_t *_timer = timer;
    // Set the UG bit to generate an update event (reset)
    *timer_get_EGR(_timer) = TIMER_EGR__UG;
}
void timer_stop(timer_t timer)
{
    _timer_t *_timer = timer;

    // Disable the counter
    *timer_get_CR1(_timer) &= ~TIMER_CR1__CEN;
}

uint16_t timer_time(timer_t timer)
{
    return *timer_get_CNT(timer);
}
uint32_t timer_get_frequency(timer_t timer)
{
    _timer_t *_timer = timer;

    if(_timer->frequency != 0)
    {
        // Return the estimated frequency
        return _timer->frequency;
    }
    else
    {
        // Assume internal clock
        uint32_t freq;

        // Get internal frequency
        if(_timer->apb_bus == 1)
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

uint16_t timer_get_number_of_channels(timer_t timer)
{
    _timer_t *_timer = timer;
    return _timer->number_of_channels;
}

void timer_set_channel_compare(timer_t timer, timer_channel_t channel,
                               uint16_t compare_value, timer_handler_t handler, handler_arg_t arg)
{
    _timer_t *_timer = timer;

    // Get CCMR register
    volatile uint16_t *ccmr;
    ccmr = (channel < TIMER_CHANNEL_3) ? timer_get_CCMR1(_timer)
           : timer_get_CCMR2(_timer);

    // Get bit offset
    uint8_t ccmr_offset;
    ccmr_offset = (channel & 1) ? 8 : 0;

    // Clear CCER channel settings
    *timer_get_CCER(timer) &= ~((TIMER_CCER__CC1NP | TIMER_CCER__CC1P
                                 | TIMER_CCER__CC1E) << (4 * channel));

    // Clear CCMR channel settings
    *ccmr &= ~((TIMER_CCMRx__OCxM__MASK | TIMER_CCMRx__CCxS__MASK)
               << ccmr_offset);

    // Set output state
    *timer_get_CCER(timer) |= TIMER_CCER__CC1E << (4 * channel);

    // Store compare match value
    *timer_get_CCRx(_timer, channel) = compare_value;

    // Store the handler
    _timer->channel_handlers[channel] = handler;
    _timer->channel_handler_args[channel] = arg;

    // clear the interrupt flag
    *timer_get_SR(_timer) &= ~(TIMER_SR__CC1IF << channel);

    // Enable interrupt if required
    if(handler)
    {
        *timer_get_DIER(_timer) |= TIMER_DIER__CC1IE << channel;
    }
    else
    {
        *timer_get_DIER(_timer) &= ~(TIMER_DIER__CC1IE << channel);
    }
}

void timer_activate_channel_output(timer_t timer, timer_channel_t channel,
                                   timer_output_mode_t mode)
{
    _timer_t *_timer = timer;

    // Get CCMR register
    volatile uint16_t *ccmr;
    ccmr = (channel < TIMER_CHANNEL_3) ? timer_get_CCMR1(_timer)
           : timer_get_CCMR2(_timer);

    // Get bit offset
    uint8_t ccmr_offset;
    ccmr_offset = (channel & 1) ? 8 : 0;

    // Set output compare mode
    *ccmr |= mode << (4 + ccmr_offset);

    switch(mode)
    {
        case TIMER_OUTPUT_MODE_PWM1:
        case TIMER_OUTPUT_MODE_PWM2:
            // Enable preload for PWM only!
            *ccmr |= (TIMER_CCMRx__OCxPE << ccmr_offset);
            break;
        default:
            break;
    }
}

void timer_update_channel_compare(timer_t timer, timer_channel_t channel,
                                  uint16_t value)
{
    _timer_t *_timer = timer;

    // Check channel
    if(channel >= _timer->number_of_channels)
    {
        return;
    }

    // Store the new compare match value
    *timer_get_CCRx(_timer, channel) = value;
}

void timer_set_channel_capture(timer_t timer, timer_channel_t channel,
                               timer_capture_edge_t signal_edge, timer_handler_t handler,
                               handler_arg_t arg)
{
    _timer_t *_timer = timer;

    // Check channel
    if(channel >= _timer->number_of_channels)
    {
        return;
    }

    // Select Capture, Set 01 to CCxS in CCMRx register (no filter, no prescaler)

    // Get CCMR register
    volatile uint16_t *ccmr;

    if(channel < 2)
    {
        ccmr = timer_get_CCMR1(_timer);
    }
    else
    {
        ccmr = timer_get_CCMR2(_timer);
    }

    // Get bit offset
    uint8_t ccmr_offset;

    if(channel & 1)
    {
        ccmr_offset = 8;
    }
    else
    {
        ccmr_offset = 0;
    }

    // Clear the CCER register
    *timer_get_CCER(_timer) &= ~(TIMER_CCER__CC1_MASK);

    // Clear the CCMR register
    *ccmr &= ~(0xF << ccmr_offset);

    // clear the interrupt flag
    *timer_get_SR(_timer) &= ~(TIMER_SR__CC1IF << channel);

    // Disable the interruption
    *timer_get_DIER(_timer) &= ~(TIMER_DIER__CC1IE << channel);

    // If handler is NULL, stop here
    if(handler == NULL)
    {
        return;
    }

    // Set CCMR register value
    *ccmr |= TIMER_CCMRx__CCxS__INPUT1 << (ccmr_offset);

    // Select the trigger edge by setting the CCxP and CCxNP bits of the CCER register
    uint8_t value = 0;

    switch(signal_edge)
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

    // Enable the interrupt
    *timer_get_DIER(_timer) |= TIMER_DIER__CC1IE << channel;
}

void timer_handle_interrupt(_timer_t *_timer)
{
    // Check source of interrupt
    // Update event
    if(*timer_get_SR(_timer) & TIMER_SR__UIF)
    {
        // Clear flag
        *timer_get_SR(_timer) &= ~TIMER_SR__UIF;

        // Call handler if any
        if(_timer->update_handler)
        {
            _timer->update_handler(_timer->update_handler_arg, *timer_get_ARR(
                                       _timer));
        }
    }

    // Channel event
    int i;

    for(i = 0; i < _timer->number_of_channels; i++)
    {
        uint16_t ccif = (TIMER_SR__CC1IF << i);

        // Channel i capture/compare event
        if((*timer_get_SR(_timer) & ccif) && (*timer_get_DIER(_timer) & ccif))
        {
            // Clear flag
            *timer_get_SR(_timer) &= ~ccif;

            // Call handler if any
            if(_timer->channel_handlers[i])
            {
                _timer->channel_handlers[i](_timer->channel_handler_args[i],
                                            *timer_get_CCRx(_timer, i));
            }
        }
    }
}
