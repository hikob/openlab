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
 * Copyright (C) 2011 HiKoB.
 */

/*
 * rcc.c
 *
 *  Created on: Jul 14, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef RCC_C_
#define RCC_C_

#include "rcc.h"

void rcc_ahb_enable(rcc_ahb_bit_t bit)
{
	*rcc_get_AHBxENR(1) |= BV(bit);
}

void rcc_ahb_disable(rcc_ahb_bit_t bit)
{
	*rcc_get_AHBxENR(1) &= ~BV(bit);
}

void rcc_apb_enable(rcc_apb_bus_t bus, rcc_apb_bit_t bit)
{
	*rcc_get_APBxENR(bus) |= BV(bit);
}

void rcc_apb_disable(rcc_apb_bus_t bus, rcc_apb_bit_t bit)
{
    *rcc_get_APBxENR(bus) &= ~BV(bit);
}
void rcc_apb_reset(rcc_apb_bus_t bus, rcc_apb_bit_t bit)
{
    *rcc_get_APBxRSTR(bus) |= BV(bit);
    asm volatile ("nop");
    *rcc_get_APBxRSTR(bus) &= ~BV(bit);
}

#endif /* RCC_C_ */
