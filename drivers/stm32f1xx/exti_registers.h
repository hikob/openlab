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
 * exti_registers.h
 *
 *  Created on: Jul 18, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef EXTI_REGISTERS_H_
#define EXTI_REGISTERS_H_

#include "memmap.h"

static inline volatile uint32_t *exti_get_IMR()
{
    return mem_get_reg32(EXTI_BASE_ADDRESS + EXTI_IMR_OFFSET);
}

static inline volatile uint32_t *exti_get_RTSR()
{
    return mem_get_reg32(EXTI_BASE_ADDRESS + EXTI_RTSR_OFFSET);
}

static inline volatile uint32_t *exti_get_FTSR()
{
    return mem_get_reg32(EXTI_BASE_ADDRESS + EXTI_FTSR_OFFSET);
}

static inline volatile uint32_t *exti_get_PR()
{
    return mem_get_reg32(EXTI_BASE_ADDRESS + EXTI_PR_OFFSET);
}

#endif /* EXTI_REGISTERS_H_ */
