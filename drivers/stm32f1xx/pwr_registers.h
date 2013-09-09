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
 * pwr_registers.h
 *
 *  Created on: May 28, 2013
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef PWR_REGISTERS_H_
#define PWR_REGISTERS_H_


#include "memmap.h"

inline static volatile uint16_t *pwr_get_CR()
{
    return mem_get_reg16(PWR_BASE_ADDRESS + PWR_CR_OFFSET);
}

inline static volatile uint16_t *pwr_get_CSR()
{
    return mem_get_reg16(PWR_BASE_ADDRESS + PWR_CSR_OFFSET);
}

enum
{
    PWR_CR__DBP = 0x0100,
    PWR_CR__PDDS = 0x0002,
    PWR_CR__LPDS = 0x0001,
};

#endif /* PWR_REGISTERS_H_ */
