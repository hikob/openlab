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
 * \file timer.h
 *
 * \date Jul 6, 2011
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef OPENLAB_TIMER_H_
#define OPENLAB_TIMER_H_

/**
 * \addtogroup drivers
 * @{
 */

/**
 * \defgroup Timer Timer driver
 *
 * This driver provides all API functions required to control a hardware timer.
 * The clock source can be selected from either internal or external clock, then
 * each of its channels may be configured in capture or compare mode.
 *
 *@{
 */

#include <stdint.h>
#include "handler.h"

/** Abstract type representing a timer */
typedef const void *openlab_timer_t;

/**
 * Timer Handler function prototype, called on update/compare/capture interrupt.
 *
 * \param arg the argument passed when registering the handler
 * \param timer_value the timer value when the interrupt triggered
 */
typedef void (*timer_handler_t)(handler_arg_t arg, uint16_t timer_value);

/**
 *  Maximum available timer channels
 *
 * \note Not all timer have all the timer channels available.
 */
typedef enum
{
    TIMER_CHANNEL_1 = 0,
    TIMER_CHANNEL_2 = 1,
    TIMER_CHANNEL_3 = 2,
    TIMER_CHANNEL_4 = 3
} timer_channel_t;

/**
 * Enable a Timer.
 *
 * This enables the timer in the clock system.
 * \param timer the timer to enable.
 */
void timer_enable(openlab_timer_t timer);

/**
 * Disable a Timer.
 *
 * This disables the timer in the clock system.
 * \param timer the timer to disable.
 */
void timer_disable(openlab_timer_t timer);

/**
 * Select the internal clock (TIMx_CLK) to feed the timer.
 *
 * This should be called before starting the timer, but after enabling it.
 *
 * \param prescaler the prescaler to use between the internal clock and
 * the timer clock (the prescaler is N+1).
 * \param timer the timer to select.
 */
void timer_select_internal_clock(openlab_timer_t timer, uint16_t prescaler);

/**
 * Select the external clock (either from a PIN or from the LSE) to feed the
 * timer.
 *
 * This should be called before starting the timer, but after enabling it.
 * Depending on the timer, it may provide a 32kHz clock, or a clock signal from
 * a pin.
 *
 * \param prescaler the prescaler to use between the external clock and
 * the timer clock (the prescaler is N+1).
 * \param timer the timer to select.
 */
void timer_select_external_clock(openlab_timer_t timer, uint16_t prescaler);

/**
 * Start the timer.
 *
 * The timer counts up from 0 to the provided update value. Once the update value
 * is reached, an interruption is generated and the provided handler is called
 * (if any). Then the timer counts again starting from 0.
 *
 * \param timer the timer to start.
 * \param update_value the value until which the timer counts (value used is N+1)
 * e.g. to have a 65536 ticks loop, use a 65535 update value.
 * \param update_handler a handler function to be called on each timer loop.
 * \param update_arg the argument to be provided to the handler function.
 */
void timer_start(openlab_timer_t timer, uint16_t update_value,
                 timer_handler_t update_handler, handler_arg_t update_arg);
/**
 * Stop the timer.
 *
 * \param timer the timer to stop.
 */
void timer_stop(openlab_timer_t timer);

/**
 * Get the timer counter actual value.
 *
 * \param timer the timer to get the value from.
 * \return the timer counter value.
 */
uint16_t timer_time(openlab_timer_t timer);

/**
 * Adjust the current tick count value;
 *
 * \param timer the timer to get the value from;
 * \param dt the number of ticks to add (may be negative);
 */
void timer_tick_update(openlab_timer_t timer, int16_t dt);

/**
 * Get the timer frequency, in Hz.
 * \param timer the timer to get the frequency from.
 * \return the frequency in Hz.
 */
uint32_t timer_get_frequency(openlab_timer_t timer);

/**
 * Get the number of channels of this timer.
 * When calling the channel related functions, the channel should be a number
 * ranging from 0 to (number_of_channels - 1).
 *
 * \return the number of channels of this timer.
 */

uint16_t timer_get_number_of_channels();
/**
 * Enable a Timer channel in compare mode.
 *
 * In compare mode, an interruption is generated when the timer counter equals
 * the channel compare value. The provided handler function is then called with
 * the provided argument.
 *
 * If the handler is null, the channel is disabled.
 *
 * \param timer the timer to use.
 * \param channel the channel number to use, note that if the channel number
 * is invalid, nothing will happen.
 * \param compare_value the compare value to configure the channel with.
 * \param handler the handler function to call.
 * \param arg the argument to provide to the handler when called.
 */
void timer_set_channel_compare(openlab_timer_t timer, timer_channel_t channel,
                               uint16_t compare_value, timer_handler_t handler, handler_arg_t arg);

/**
 * Update a Timer channel compare value.
 *
 * This allows to update the channel timer value, usually to set the next
 * interruption
 * \param timer the timer to use.
 * \param channel the channel number to use.
 * \param value the new value to set as the compare value of this channel.
 */
void
timer_update_channel_compare(openlab_timer_t timer, timer_channel_t channel,
                             uint16_t value);

typedef enum
{
    TIMER_OUTPUT_MODE_FROZEN = 0x0,
    TIMER_OUTPUT_MODE_SET_ACTIVE = 0x1,
    TIMER_OUTPUT_MODE_SET_INACTIVE = 0x2,
    TIMER_OUTPUT_MODE_TOGGLE = 0x3,
    TIMER_OUTPUT_MODE_FORCE_INACTIVE = 0x4,
    TIMER_OUTPUT_MODE_FORCE_ACTIVE = 0x5,

    TIMER_OUTPUT_MODE_PWM1 = 0x6,
    TIMER_OUTPUT_MODE_PWM2 = 0x7,
} timer_output_mode_t;

/**
 * Activate a channel output on the corresponding PINs.
 *
 * Use the FROZEN mode to disable
 *
 * \param timer the timer;
 * \param channel the channel to activate output;
 * \param mode the output mode
 */
void timer_activate_channel_output(openlab_timer_t timer, timer_channel_t channel,
                                   timer_output_mode_t mode);

/**
 * Available modes for input capture mode.
 */
typedef enum
{
    TIMER_CAPTURE_EDGE_RISING = 0,
    TIMER_CAPTURE_EDGE_FALLING = 1,
    TIMER_CAPTURE_EDGE_BOTH = 3,
} timer_capture_edge_t;

/**
 * Enable a Timer channel in capture mode.
 *
 * In capture mode, an interruption is generated when the input pin
 * corresponding to the channel timer generate a signal edge. The provided
 * handler function is then called, specifying the counter value.
 *
 * If the handler is null, the channel is disabled.
 *
 * \param timer the timer to use.
 * \param channel the channel number to use, note that if the channel number
 * is invalid, nothing will happen.
 * \param edge the signal edge on which the capture will happen.
 * \param handler the handler function to call.
 * \param arg the argument to provide to the handler when called.
 */
void timer_set_channel_capture(openlab_timer_t timer, timer_channel_t channel,
                               timer_capture_edge_t signal_edge, timer_handler_t handler,
                               handler_arg_t arg);

/**
 * Get the update flag of a timer.
 *
 * \param timer the timer to try
 * \return 1 if the update flag is set, 0 if it is not
 */
uint32_t timer_get_update_flag(openlab_timer_t timer);

/**
 * @}
 * @}
 */

#endif /* TIMER_H_ */
