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
 * iwdg_registers.h
 *
 *  Created on: Nov 25, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef IWDG_REGISTERS_H_
#define IWDG_REGISTERS_H_

#include <stdint.h>
#include "memmap.h"

static inline volatile uint32_t *iwdg_get_KR()
{
    return mem_get_reg32(IWDG_BASE_ADDRESS + IWDG_KR_OFFSET);
}
static inline volatile uint32_t *iwdg_get_PR()
{
    return mem_get_reg32(IWDG_BASE_ADDRESS + IWDG_PR_OFFSET);
}
static inline volatile uint32_t *iwdg_get_RLR()
{
    return mem_get_reg32(IWDG_BASE_ADDRESS + IWDG_RLR_OFFSET);
}
static inline volatile uint32_t *iwdg_get_SR()
{
    return mem_get_reg32(IWDG_BASE_ADDRESS + IWDG_SR_OFFSET);
}

enum
{
    IWDG_KR__CLEAR = 0xAAAA,
    IWDG_KR__ACCESS = 0x5555,
    IWDG_KR__START = 0xCCCC,
};

enum
{
    IWDG_SR__RVU = 0x0002,
    IWDG_SR__PVU = 0x0001,
};

#endif /* IWDG_REGISTERS_H_ */
