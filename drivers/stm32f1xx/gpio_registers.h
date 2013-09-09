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
 * gpio_registers.h
 *
 *  Created on: Jul 12, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef GPIO_REGISTERS_H_
#define GPIO_REGISTERS_H_

#include "gpio_.h"

static inline volatile uint32_t *gpio_get_CRL(const _gpio_t *_gpio)
{
    return mem_get_reg32(_gpio->base_address + GPIOx_CRL_OFFSET);
}

static inline volatile uint32_t *gpio_get_CRH(const _gpio_t *_gpio)
{
    return mem_get_reg32(_gpio->base_address + GPIOx_CRH_OFFSET);
}

static inline volatile uint32_t *gpio_get_IDR(const _gpio_t *_gpio)
{
    return mem_get_reg32(_gpio->base_address + GPIOx_IDR_OFFSET);
}

static inline volatile uint32_t *gpio_get_ODR(const _gpio_t *_gpio)
{
    return mem_get_reg32(_gpio->base_address + GPIOx_ODR_OFFSET);
}

static inline volatile uint32_t *gpio_get_BSRR(const _gpio_t *_gpio)
{
    return mem_get_reg32(_gpio->base_address + GPIOx_BSRR_OFFSET);
}

#endif /* GPIO_REGISTERS_H_ */
