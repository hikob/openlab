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
 * afio.c
 *
 *  Created on: Jul 19, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include "afio.h"
#include "afio_registers.h"

void afio_select_exti_pin(exti_line_t line, afio_port_t port)
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
    p32 = mem_get_reg32(AFIO_BASE_ADDRESS + AFIO_EXTICR1_OFFSET
                        + reg_offset);

    // Clear value
    *p32 &= ~(0xF << bit_offset);
    // Set value
    *p32 |= port << bit_offset;
}
