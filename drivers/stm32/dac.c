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
 * dac.c
 *
 *  Created on: Oct 17, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "dac.h"
#include "dac_registers.h"

#include "rcc.h"

void dac_enable(dac_channel_t channel)
{
    // Enable the DAC in the RCC
    rcc_apb_enable(RCC_APB_BUS_DAC, RCC_APB_BIT_DAC);

    switch (channel)
    {
        case DAC_CHANNEL_1:
            *dac_get_CR() &= ~0x0000FFFF;
            *dac_get_CR() |= DAC_CR__EN1;
            break;
        case DAC_CHANNEL_2:
            *dac_get_CR() &= ~0xFFFF0000;
            *dac_get_CR() |= DAC_CR__EN2;
            break;
    }
}

void dac_disable(dac_channel_t channel)
{
    switch (channel)
    {
        case DAC_CHANNEL_1:
            *dac_get_CR() &= ~0x0000FFFF;
            break;
        case DAC_CHANNEL_2:
            *dac_get_CR() &= ~0xFFFF0000;
            break;
    }

    // Disable the DAC in the RCC
    rcc_apb_disable(RCC_APB_BUS_DAC, RCC_APB_BIT_DAC);
}

void dac_set_channel_value(dac_channel_t channel, uint16_t value)
{
    // Assuming 12bit left justified data
    *dac_get_DHR12Lx(channel) = value;
}
