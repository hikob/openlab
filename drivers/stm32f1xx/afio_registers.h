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
 * afio_registers.h
 *
 *  Created on: Jul 19, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef AFIO_REGISTERS_H_
#define AFIO_REGISTERS_H_

#include "memmap.h"

static inline volatile uint32_t *afio_get_EVCR()
{
    return mem_get_reg32(AFIO_BASE_ADDRESS + AFIO_EVCR_OFFSET);
}

static inline volatile uint32_t *afio_get_MAPR()
{
    return mem_get_reg32(AFIO_BASE_ADDRESS + AFIO_MAPR_OFFSET);
}

static inline volatile uint32_t *afio_get_EXTICR1()
{
    return mem_get_reg32(AFIO_BASE_ADDRESS + AFIO_EXTICR1_OFFSET);
}

static inline volatile uint32_t *afio_get_EXTICR2()
{
    return mem_get_reg32(AFIO_BASE_ADDRESS + AFIO_EXTICR2_OFFSET);
}

static inline volatile uint32_t *afio_get_EXTICR3()
{
    return mem_get_reg32(AFIO_BASE_ADDRESS + AFIO_EXTICR3_OFFSET);
}

static inline volatile uint32_t *afio_get_EXTICR4()
{
    return mem_get_reg32(AFIO_BASE_ADDRESS + AFIO_EXTICR4_OFFSET);
}

static inline volatile uint32_t *afio_get_MAPR2()
{
    return mem_get_reg32(AFIO_BASE_ADDRESS + AFIO_MAPR2_OFFSET);
}

#endif /* AFIO_REGISTERS_H_ */
