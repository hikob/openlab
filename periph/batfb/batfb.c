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
#include "printf.h"

/* Handy inline functions to control all the pins */
static inline void batfb_enable();
static inline void batfb_disable();

/** Handler for ADC conversion complete */
static void adc_sample_done(handler_arg_t arg, uint16_t value);

static struct
{
    /** A pointer to the in use configuration */
    const batfb_config_t *config;

    /** The handler to be called on each conversion */
    handler_t handler;
    /** The handler argument */
    handler_arg_t handler_arg;

    /** Flag indicating conversion is terminated */
    volatile uint32_t conversion_done;
} batfb;

/** Result value */
uint16_t batfb_vbat;

void batfb_config(const batfb_config_t *cfg)
{
    // Store the configuration
    batfb.config = cfg;
}

void batfb_init()
{
    // Configure enable IO pin
    gpio_enable(batfb.config->enable_gpio);
    batfb_disable();

    // Prepare the ADC channel
    adc_config_channel(batfb.config->adc, batfb.config->adc_channel, 7);

    // Reset stored values
    batfb.handler = NULL;
    batfb.conversion_done = 0;
}

void batfb_sample(handler_t handler, handler_arg_t arg)
{
    // Store handler and arg
    batfb.handler = handler;
    batfb.handler_arg = arg;

    // Enable the feedback
    batfb_enable();

    // Clear the conversion flag
    batfb.conversion_done = 0;

    // Start sampling
    adc_set_handler(batfb.config->adc, adc_sample_done, NULL);
    adc_prepare_single(batfb.config->adc, batfb.config->adc_channel);
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
static void adc_sample_done(handler_arg_t arg, uint16_t value)
{
    // Disable the feedback
    batfb_disable();

    // Store the value, converted
    batfb_vbat = (uint32_t) value * 2800 * 2 / 4096;

    // Set the conversion flag
    batfb.conversion_done = 1;

    // Call handler
    if (batfb.handler)
    {
        batfb.handler(batfb.handler_arg);
    }
}

/* INLINE simple IO accesses implementations */

static inline void batfb_enable()
{
    // Set the pin output low
    gpio_set_output(batfb.config->enable_gpio, batfb.config->enable_pin);
    gpio_pin_clear(batfb.config->enable_gpio, batfb.config->enable_pin);
}
static inline void batfb_disable()
{
    // Set the pin analog
    gpio_set_analog(batfb.config->enable_gpio, batfb.config->enable_pin);
}
