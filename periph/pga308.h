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
 * pga308.h
 *
 *  Created on: Nov 21, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef PGA308_H_
#define PGA308_H_

#include "adc.h"

typedef enum
{
    PGA308_INPUT_GAIN__4 = 0x0,
    PGA308_INPUT_GAIN__6 = 0x1,
    PGA308_INPUT_GAIN__8 = 0x2,
    PGA308_INPUT_GAIN__12 = 0x3,
    PGA308_INPUT_GAIN__16 = 0x4,
    PGA308_INPUT_GAIN__32 = 0x5,
    PGA308_INPUT_GAIN__64 = 0x6,
    PGA308_INPUT_GAIN__100 = 0x7,
    PGA308_INPUT_GAIN__200 = 0x8,
    PGA308_INPUT_GAIN__400 = 0x9,
    PGA308_INPUT_GAIN__480 = 0xA,
    PGA308_INPUT_GAIN__600 = 0xB,
    PGA308_INPUT_GAIN__800 = 0xC,
    PGA308_INPUT_GAIN__960 = 0xD,
    PGA308_INPUT_GAIN__1200 = 0xE,
    PGA308_INPUT_GAIN__1600 = 0xF,
} pga308_input_gain_t;

typedef enum
{
    PGA308_OUTPUT_GAIN__2 = 0x0,
    PGA308_OUTPUT_GAIN__2_4 = 0x1,
    PGA308_OUTPUT_GAIN__3 = 0x2,
    PGA308_OUTPUT_GAIN__3_6 = 0x3,
    PGA308_OUTPUT_GAIN__4 = 0x4,
    PGA308_OUTPUT_GAIN__4_5 = 0x5,
    PGA308_OUTPUT_GAIN__6 = 0x6,
} pga308_output_gain_t;

/**
 * Enable the PGA308 by powering it ON, and setting the configuration
 * registerd
 */
void pga308_enable();

/**
 * Disable the PGA308.
 */
void pga308_disable();

/**
 * Configure the gain registers.
 *
 * The zero-DAC value is computed as V = Vref/2 - zero_dat/65536 * Vref
 * hence to have a zero output value at Vcc/2, zero_dac should be set to 0
 *
 * The gain-DAC formula is G = 2/3 * gain_dac/65536, allowing values between
 * 1/3 and ~1
 *
 * \param input_gain the input gain applied directly to the input;
 * \param zero_dac an offset voltage applied after the input gain to configure
 *      the 0 output;
 * \param gain_dac a fine gain to precisely adjust the global gain;
 * \param output_gain the output gain;
 */
void pga308_configure_gain(pga308_input_gain_t input_gain, uint16_t zero_dac,
                           uint16_t gain_dac, pga308_output_gain_t output_gain);

/**
 * Get the ADC driver of the PGA308 output
 */
adc_t pga308_get_adc();

/**
 * Get the ADC channel of the PGA308 output
 */
uint8_t pga308_get_adc_channel();

/**
 * DEBUG: print the current registers of the PGA308.
 */
void pga308_print_regs();

#endif /* PGA308_H_ */
