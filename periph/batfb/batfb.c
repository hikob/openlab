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
 * batfb.c
 *
 *  Created on: Oct 26, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "batfb.h"
#include "batfb_.h"
#include "gpio.h"
#include "adc.h"

#include "platform.h"
#include "debug.h"

/* Handy inline functions to control all the pins */
static inline void batfb_enable();
static inline void batfb_disable();

/** Handler for ADC conversion complete */
static void adc_sample_done(handler_arg_t arg, uint16_t value);

static struct
{
    /** A pointer to the configuration */
    const batfb_config_t *config;
    /** A pointer to the solar configuration */
    const batfb_solar_config_t *solar_config;

    /** The handler to be called on each conversion */
    handler_t handler;
    /** The handler argument */
    handler_arg_t handler_arg;

    /** Flag indicating conversion is terminated */
    volatile uint32_t conversion_done;
} batfb = {.config = NULL, .solar_config = NULL};

/** Result values */
float batfb_vcc;
float batfb_vbat;

void batfb_config(const batfb_config_t *cfg)
{
    // Store the configuration
    batfb.config = cfg;
}

void batfb_solar_config(const batfb_solar_config_t *cfg)
{
    // Store the configuration
    batfb.solar_config = cfg;
}

void batfb_init()
{
    // Configure enable IO pin
    gpio_enable(batfb.config->enable_gpio);
    if (batfb.config->mode == BATFB_MODE_TRANSISTORS)
    {
        // Set output
        gpio_set_output(batfb.config->enable_gpio, batfb.config->enable_pin);
    }
    batfb_disable();

    // Prepare the ADC channel
    adc_config_channel(batfb.config->adc, batfb.config->adc_channel, 7);

    // Reset stored values
    batfb.handler = NULL;
    batfb.conversion_done = 0;

    // Configure the solar config if any
    if (batfb.solar_config != NULL)
    {
        gpio_set_input(batfb.solar_config->pgood_gpio, batfb.solar_config->pgood_pin);
        gpio_set_input(batfb.solar_config->vldo_gpio, batfb.solar_config->vldo_pin);
        gpio_config_pull_up_down(batfb.solar_config->vldo_gpio, batfb.solar_config->vldo_pin, GPIO_PULL_DOWN);
    }
}

batfb_type_t batfb_get_type()
{
    if (batfb.config == NULL )
    {
        return BATFB_TYPE_NONE;
    }
    else
    {
        return batfb.config->type;
    }
}
void batfb_sample(handler_t handler, handler_arg_t arg)
{
    // Clear the conversion flag
    batfb.conversion_done = 0;

    // Store handler and arg
    batfb.handler = handler;
    batfb.handler_arg = arg;

    // Try to get a hold of the ADC
    if (!adc_take(batfb.config->adc))
    {
        // Do nothing and call handler
        batfb.conversion_done = 1;
        handler(arg);
        return;
    }

    // First, measure Vcc by sampling Vrefint
    adc_enable_vrefint();

    // Wait until booted
    soft_timer_delay_ms(3);

    adc_set_handler(batfb.config->adc, adc_sample_done, (handler_arg_t) 0);
    adc_prepare_single(batfb.config->adc, ADC_CHANNEL_VREFINT);
    adc_sample_single(batfb.config->adc);
}

void batfb_sample_sync()
{
    // Request a sampling
    batfb_sample(NULL, NULL);

    // Wait for end of conversion
    while (!batfb.conversion_done)
    {
    }
}

int32_t batfb_has_solar()
{
    return (batfb.solar_config != NULL);
}

batfb_solar_status_t batfb_get_solar_state()
{
    return (gpio_pin_read(batfb.solar_config->pgood_gpio,
            batfb.solar_config->pgood_pin) ? BATFB_SOLAR_POWERING : 0)
            | (gpio_pin_read(batfb.solar_config->vldo_gpio,
                    batfb.solar_config->vldo_pin) ? BATFB_SOLAR_LITTLE : 0);
}

/* ****************** */

static void adc_sample_done(handler_arg_t arg, uint16_t value)
{
    if (arg == 0)
    {
        // Vcc is sampled
        adc_disable_vrefint();
        batfb_vcc = (3.f * adc_get_factory_3v_vrefint()) / value;

        // Enable the feedback
        batfb_enable();

        // Sampling
        adc_set_handler(batfb.config->adc, adc_sample_done, (handler_arg_t) 1);
        adc_prepare_single(batfb.config->adc, batfb.config->adc_channel);
        adc_sample_single(batfb.config->adc);
    }
    else
    {
        // Vbat is sampled, release the ADC
        adc_release(batfb.config->adc);

        // Disable the feedback
        batfb_disable();

        // Store the value, converted
        batfb_vbat = batfb_vcc * value / 2048.f;

        // Set the conversion flag
        batfb.conversion_done = 1;

        // Call handler
        if (batfb.handler)
        {
            batfb.handler(batfb.handler_arg);
        }
    }
}

/* INLINE simple IO accesses implementations */

static inline void batfb_enable()
{
    if (batfb.config->mode == BATFB_MODE_HIGH_Z)
    {
        // Set the pin output low
        gpio_set_output(batfb.config->enable_gpio, batfb.config->enable_pin);
        gpio_pin_clear(batfb.config->enable_gpio, batfb.config->enable_pin);
    }
    else
    {
        // Set the pin output high
        gpio_pin_set(batfb.config->enable_gpio, batfb.config->enable_pin);
    }
}
static inline void batfb_disable()
{
    if (batfb.config->mode == BATFB_MODE_HIGH_Z)
    {
        // Set the pin analog
        gpio_set_analog(batfb.config->enable_gpio, batfb.config->enable_pin);
    }
    else
    {
        // Set the pin output low
        gpio_pin_clear(batfb.config->enable_gpio, batfb.config->enable_pin);
    }
}
