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
 * rcc_clock.h
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef RCC_CLOCK_H_
#define RCC_CLOCK_H_
#include <stdint.h>

/* MSI section */
typedef enum rcc_msi_frequency
{
    RCC_MSI_FREQ_65kHz = 0,
    RCC_MSI_FREQ_131kHz = 1,
    RCC_MSI_FREQ_262kHz = 2,
    RCC_MSI_FREQ_524kHz = 3,
    RCC_MSI_FREQ_1048kHz = 4,
    RCC_MSI_FREQ_2097kHz = 5,
    RCC_MSI_FREQ_4194kHz = 6,
} rcc_msi_frequency_t;
void rcc_msi_enable(rcc_msi_frequency_t freq, uint8_t trim);
void rcc_msi_disable();
void rcc_msi_compute_frequency();
uint32_t rcc_msi_get_frequency();

/* HSI section */
enum
{
    RCC_HSI_FREQUENCY = 16000000
};
void rcc_hsi_enable();
void rcc_hsi_disable();
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
    RCC_LSI_FREQUENCY = 37000
};
void rcc_lsi_enable();
void rcc_lsi_disable();
static inline uint32_t rcc_lsi_get_requency()
{
    return RCC_LSI_FREQUENCY;
}

/* PLL section */
typedef enum rcc_pll_source
{
    RCC_PLL_SOURCE_HSI = 0, RCC_PLL_SOURCE_HSE = 1,
} rcc_pll_source_t;
typedef enum rcc_pll_multiplier
{
    RCC_PLL_MUL_3 = 0x0,
    RCC_PLL_MUL_4 = 0x1,
    RCC_PLL_MUL_6 = 0x2,
    RCC_PLL_MUL_8 = 0x3,
    RCC_PLL_MUL_12 = 0x4,
    RCC_PLL_MUL_16 = 0x5,
    RCC_PLL_MUL_24 = 0x6,
    RCC_PLL_MUL_32 = 0x7,
    RCC_PLL_MUL_48 = 0x8,
} rcc_pll_multiplier_t;
typedef enum rcc_pll_divider
{
    RCC_PLL_DIV_2 = 0x1, RCC_PLL_DIV_3 = 0x2, RCC_PLL_DIV_4 = 0x3,
} rcc_pll_divider_t;

void rcc_pll_enable(rcc_pll_source_t source, rcc_pll_multiplier_t mul,
                    rcc_pll_divider_t div);
void rcc_pll_disable();
uint32_t rcc_pll_get_frequency();

#endif /* RCC_CLOCK_H_ */
