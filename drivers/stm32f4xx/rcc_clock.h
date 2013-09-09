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
 * rcc_clock.h
 *
 *  Created on: Jul 14, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef RCC_CLOCK_H_
#define RCC_CLOCK_H_
#include <stdint.h>

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
    RCC_LSI_FREQUENCY = 32000
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
    RCC_PLL_MAIN_DIV_2 = 0,
    RCC_PLL_MAIN_DIV_4 = 1,
    RCC_PLL_MAIN_DIV_6 = 2,
    RCC_PLL_MAIN_DIV_8 = 3,
} rcc_pll_main_clock_div_t;

/**
 * Enable the PLL
 *
 * \param source the source of the pll,
 * \param input_div the divider on the input, to make a 2MHz clock, values between 2 and 63
 * \param input_mul the multiplier after the divider, values between 63 and 432 included,
 * \param main_div the divider to feed the system clock, value 2/4/6/8
 * \param usb_div the divider to feet the usb clock to make it 48MHz, values between 2 and 15 included
 */
void
rcc_pll_enable(rcc_pll_source_t source, uint32_t input_div, uint32_t input_mul,
        rcc_pll_main_clock_div_t main_div, uint32_t usb_div);
void rcc_pll_disable();
uint32_t rcc_pll_get_frequency();

#endif /* RCC_CLOCK_H_ */
