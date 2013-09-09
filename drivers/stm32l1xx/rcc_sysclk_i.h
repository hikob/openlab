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
 * rcc_sysclk_i.h
 *
 *  Created on: Nov 9, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef RCC_SYSCLK_I_H_
#define RCC_SYSCLK_I_H_

#include "rcc_sysclk.h"
#include "rcc_registers.h"

#define rcc_sysclk_set_i(source, ahb_pres, apb1_pres, apb2_pres) \
    do { \
        *rcc_get_CFGR() = (*rcc_get_CFGR() & 0xFFFF0000) | source | \
                          (apb2_pres << 11) | (apb1_pres << 8) | (ahb_pres << 4); \
    } while (0)

#endif /* RCC_SYSCLK_I_H_ */
