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
 * rcc_clock.h
 *
 *  Created on: Jul 15, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef RCC_CLOCK_H_
#define RCC_CLOCK_H_

#include <stdbool.h>
#include <stdint.h>

/* HSI section */
enum
{
    RCC_HSI_FREQUENCY = 8000000
};

void rcc_hsi_enable();
void rcc_hsi_disable();
bool rcc_is_hsi_enabled();

static inline uint32_t rcc_hsi_get_frequency()
{
    return RCC_HSI_FREQUENCY;
}

/* HSE section */
void rcc_hse_enable();
void rcc_hse_disable();
void rcc_hse_indicate_frequency(uint32_t f);
uint32_t rcc_hse_get_frequency();

/* LSE section */
enum
{
    RCC_LSE_FREQUENCY = 32768
};
void rcc_lse_enable();
void rcc_lse_disable();

static inline uint32_t rcc_lse_get_frequency()
{
    return RCC_LSE_FREQUENCY;
}

/* LSI section */
enum
{
    RCC_LSI_FREQUENCY = 40000
};
void rcc_lsi_enable();
void rcc_lsi_disable();

static inline uint32_t rcc_lsi_get_requency()
{
    return RCC_LSI_FREQUENCY;
}

/* PLL section */
typedef enum
{
    RCC_PLL_SOURCE_HSI = 0,
    RCC_PLL_SOURCE_HSE = 1
} rcc_pll_source_t;

typedef enum
{
    RCC_PLL_MUL_2  = 0x0,
    RCC_PLL_MUL_3  = 0x1,
    RCC_PLL_MUL_4  = 0x2,
    RCC_PLL_MUL_5  = 0x3,
    RCC_PLL_MUL_6  = 0x4,
    RCC_PLL_MUL_7  = 0x5,
    RCC_PLL_MUL_8  = 0x6,
    RCC_PLL_MUL_9  = 0x7,
    RCC_PLL_MUL_10 = 0x8,
    RCC_PLL_MUL_11 = 0x9,
    RCC_PLL_MUL_12 = 0xA,
    RCC_PLL_MUL_13 = 0xB,
    RCC_PLL_MUL_14 = 0xC,
    RCC_PLL_MUL_15 = 0xD,
    RCC_PLL_MUL_16 = 0xE
} rcc_pll_multiplier_t;

typedef enum
{
    RCC_PLL_NO_PREDIV = 0,
    RCC_PLL_PREDIV    = 1
} rcc_pll_prediv_t;

void rcc_pll_enable(rcc_pll_source_t source, rcc_pll_multiplier_t mul, rcc_pll_prediv_t prediv);
void rcc_pll_disable();
uint32_t rcc_pll_get_frequency();

#endif /* RCC_CLOCK_H_ */
