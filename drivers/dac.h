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
 * \file dac.h
 * \date Oct 17, 2011
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef DAC_H_
#define DAC_H_

/**
 * \addtogroup drivers
 * @{
 */

/**
 * \defgroup DAC DAC driver
 *
 * This driver provides all functions required to output an analog voltage on
 * a configured PIN of the micro-controller.
 *
 *@{
 */

#include <stdint.h>

typedef enum
{
    DAC_CHANNEL_1 = 0,
    DAC_CHANNEL_2 = 1
} dac_channel_t;

/**
 * Enable the DAC peripheral, specifying the channel to use.
 *
 * \param channel the channel to enable
 */
void dac_enable(dac_channel_t channel);

/**
 * Disable a DAC channel
 *
 * \param channel the channel to disable
 */
void dac_disable(dac_channel_t channel);

/**
 * Update the output value of a DAC channel.
 *
 * Values range from 0 to 0xFFF0 and will output a proportional analog value
 * ranging from 0 to Vcc volts.
 *
 * The DAC resolution is 12bit, aligned on the left. Hence the smallest non-zero
 * value is 0x10, and to output Vcc/2 0x8000 should be used as the value.
 *
 * \param channel the channel to update;
 * \param value the value to set.
 */
void dac_set_channel_value(dac_channel_t channel, uint16_t value);

/**
 * @}
 * @}
 */

#endif /* DAC_H_ */
