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
 * test_pga308.c
 *
 *  Created on: Jan 12, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include <stdint.h>
#include "platform.h"
#include "printf.h"
#include "debug.h"

#include "event.h"
#include "soft_timer.h"

#include "adc.h"
#include "pga308.h"
#include "adg759.h"

static void start(handler_arg_t arg);
static void measure(handler_arg_t arg);

static void adc_sample_done(handler_arg_t arg, uint16_t value);
static void char_rx(handler_arg_t arg, uint8_t c);
static volatile uint8_t new_char = 0;

extern adg759_t pga308_mux;

static volatile int32_t adc_value;
static soft_timer_t st;

int main()
{
    // Initialize the platform
    platform_init();

    event_post(EVENT_QUEUE_APPLI, start, NULL);

    platform_run();

    return 0;
}
static void start(handler_arg_t arg)
{
    uart_set_rx_handler(uart_print, char_rx, NULL);

    log_printf("### Testing PGA308 ###\n");

    // Configure the PGA for input gain 400, offset Vref/4, remaining gain 3
    pga308_configure_gain(PGA308_INPUT_GAIN__100, 0x4000, 0x4000,
                          PGA308_OUTPUT_GAIN__6);

    // Enable the pga and all
    pga308_enable();

    adg759_enable(pga308_mux);

    log_printf("Enabled\n");

    // Set ADC handler
    adc_set_handler(pga308_get_adc(), adc_sample_done, NULL);

    // Request an ADC sample
    adc_prepare_single(pga308_get_adc(), pga308_get_adc_channel());

    // Create a soft timer
    soft_timer_set_handler(&st, measure, NULL);
    soft_timer_start(&st, soft_timer_ms_to_ticks(100), 1);

    pga308_print_regs();
}

static void measure(handler_arg_t arg)
{
    static int count = 0;
    uint32_t mux_input = ADG759_INPUT_1;
    //    for (mux_input = 1; mux_input <= 4; mux_input++)
    {
        // Set the selected mux
        adg759_select(pga308_mux, mux_input);

        // Start new sample
        adc_value = -1;
        adc_prepare_single(pga308_get_adc(), pga308_get_adc_channel());
        adc_sample_single(pga308_get_adc());

        // Wait until value updated
        while (adc_value == -1)
        {
            asm volatile("nop");
        }

#if RELEASE == 0
        const char signs[] =
        { '*', '#', '$', '@' };
        log_printf("\x1b[0G\x1b[%uC%c", adc_value / 15, signs[mux_input - ADG759_INPUT_1]);
#endif
    }

    log_printf("\t%u", adc_value);
    log_printf("\n");

    count ++;

    if (count % 16 == 0)
    {
        pga308_print_regs();
    }

}
static void char_rx(handler_arg_t arg, uint8_t c)
{
    // Store the received char
    new_char = c;
}

static void adc_sample_done(handler_arg_t arg, uint16_t value)
{
    // Store the value
    adc_value = value;
}
