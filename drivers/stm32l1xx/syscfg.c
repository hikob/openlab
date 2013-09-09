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
 * syscfg.c
 *
 *  Created on: Jul 12, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "syscfg.h"
#include "syscfg_registers.h"

void syscfg_select_exti_pin(exti_line_t line, syscfg_port_t port)
{
    volatile uint32_t *p32;
    uint32_t reg_offset, bit_offset;

    // Check line
    if (line > EXTI_LINE_Px15)
    {
        // Invalid, return
        return;
    }

    // Compute register offset and bit offset
    reg_offset = 4 * (line / 4);
    bit_offset = 4 * (line % 4);

    // Get a pointer to the register
    p32 = mem_get_reg32(SYSCFG_BASE_ADDRESS + SYSCFG_EXTICR1_OFFSET
                        + reg_offset);

    // Clear value
    *p32 &= ~(0xF << bit_offset);
    // Set value
    *p32 |= port << bit_offset;
}

void syscfg_pmc_config(syscfg_dev_t dev, int mode)
{
    int val;

    val = *syscfg_get_PMC();

    if (mode)
    {
        val |= (1 << dev);
    }
    else
    {
        val &= ~(1L << dev);
    }

    // Set value
    *syscfg_get_PMC() = val;
}
