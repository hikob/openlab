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
 * crc_registers.h
 *
 *  Created on: Jan 17, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef CRC_REGISTERS_H_
#define CRC_REGISTERS_H_

#include <stdint.h>
#include "memmap.h"

static inline volatile uint32_t *crc_get_DR()
{
    return mem_get_reg32(CRC_BASE_ADDRESS + CRC_DR_OFFSET);
}
static inline volatile uint32_t *crc_get_IDR()
{
    return mem_get_reg32(CRC_BASE_ADDRESS + CRC_IDR_OFFSET);
}
static inline volatile uint32_t *crc_get_CR()
{
    return mem_get_reg32(CRC_BASE_ADDRESS + CRC_CR_OFFSET);
}

enum
{
    CRC_CR__RESET = 0x1
};

#endif /* CRC_REGISTERS_H_ */
