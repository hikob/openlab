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
 * gpio_registers.h
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef GPIO_REGISTERS_H_
#define GPIO_REGISTERS_H_

#include "gpio_.h"

static inline volatile uint32_t* gpio_get_MODER(const _gpio_t *_gpio)
{
    return mem_get_reg32(_gpio->base_address + GPIOx_MODER_OFFSET);
}
static inline volatile uint32_t* gpio_get_OTYPER(const _gpio_t *_gpio)
{
    return mem_get_reg32(_gpio->base_address + GPIOx_OTYPER_OFFSET);
}
static inline volatile uint32_t* gpio_get_OSPEEDR(const _gpio_t *_gpio)
{
    return mem_get_reg32(_gpio->base_address + GPIOx_OSPEEDR_OFFSET);
}
static inline volatile uint32_t* gpio_get_PUPDR(const _gpio_t *_gpio)
{
    return mem_get_reg32(_gpio->base_address + GPIOx_PUPDR_OFFSET);
}
static inline volatile uint32_t* gpio_get_IDR(const _gpio_t *_gpio)
{
    return mem_get_reg32(_gpio->base_address + GPIOx_IDR_OFFSET);
}
static inline volatile uint32_t* gpio_get_ODR(const _gpio_t *_gpio)
{
    return mem_get_reg32(_gpio->base_address + GPIOx_ODR_OFFSET);
}
static inline volatile uint32_t* gpio_get_BSRR(const _gpio_t *_gpio)
{
    return mem_get_reg32(_gpio->base_address + GPIOx_BSRR_OFFSET);
}
static inline volatile uint32_t* gpio_get_LCKR(const _gpio_t *_gpio)
{
    return mem_get_reg32(_gpio->base_address + GPIOx_LCKR_OFFSET);
}
static inline volatile uint32_t* gpio_get_AFRL(const _gpio_t *_gpio)
{
    return mem_get_reg32(_gpio->base_address + GPIOx_AFRL_OFFSET);
}
static inline volatile uint32_t* gpio_get_AFRH(const _gpio_t *_gpio)
{
    return mem_get_reg32(_gpio->base_address + GPIOx_AFRH_OFFSET);
}

#endif /* GPIO_REGISTERS_H_ */
