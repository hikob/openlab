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
 * rcc_sysclk.h
 *
 *  Created on: Jul 15, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef RCC_SYSCLK_H_
#define RCC_SYSCLK_H_

/* SYSCLK section */
typedef enum
{
    RCC_SYSCLK_SOURCE_HSI = 0,
    RCC_SYSCLK_SOURCE_HSE = 1,
    RCC_SYSCLK_SOURCE_PLL = 2
} rcc_sysclk_source_t;

typedef enum
{
    RCC_SYSCLK_CLOCK_SYSCLK,
    RCC_SYSCLK_CLOCK_HCLK,
    RCC_SYSCLK_CLOCK_SYSTICK_CLK,
    RCC_SYSCLK_CLOCK_PCLK1,
    RCC_SYSCLK_CLOCK_PCLK1_TIM,
    RCC_SYSCLK_CLOCK_PCLK2,
    RCC_SYSCLK_CLOCK_PCLK2_TIM
} rcc_sysclk_clock_t;

typedef enum
{
    RCC_SYSCLK_AHB_PRE_1   = 0x0,
    RCC_SYSCLK_AHB_PRE_2   = 0x8,
    RCC_SYSCLK_AHB_PRE_4   = 0x9,
    RCC_SYSCLK_AHB_PRE_8   = 0xA,
    RCC_SYSCLK_AHB_PRE_16  = 0xB,
    RCC_SYSCLK_AHB_PRE_64  = 0xC,
    RCC_SYSCLK_AHB_PRE_128 = 0xD,
    RCC_SYSCLK_AHB_PRE_256 = 0xE,
    RCC_SYSCLK_AHB_PRE_512 = 0xF
} rcc_sysclk_ahb_prescaler_t;

typedef enum
{
    RCC_SYSCLK_APB_PRE_1  = 0x0,
    RCC_SYSCLK_APB_PRE_2  = 0x4,
    RCC_SYSCLK_APB_PRE_4  = 0x5,
    RCC_SYSCLK_APB_PRE_8  = 0x6,
    RCC_SYSCLK_APB_PRE_16 = 0x7
} rcc_sysclk_apb_prescaler_t;

typedef enum
{
    RCC_SYSCLK_MCO_CLOCK_NONE   = 0x0,
    RCC_SYSCLK_MCO_CLOCK_SYSCLK = 0x4,
    RCC_SYSCLK_MCO_CLOCK_HSI    = 0x5,
    RCC_SYSCLK_MCO_CLOCK_HSE    = 0x6,
    RCC_SYSCLK_MCO_CLOCK_PLL    = 0x7
} rcc_sysclk_mco_clock_t;

/* Clock Selection and Prescaler configuration */
void rcc_sysclk_select_source(rcc_sysclk_source_t source);
void rcc_sysclk_set_prescalers(rcc_sysclk_ahb_prescaler_t ahb_pres, rcc_sysclk_apb_prescaler_t apb1_pres, rcc_sysclk_apb_prescaler_t apb2_pres);

/* Clock information */
uint32_t rcc_sysclk_get_clock_frequency(rcc_sysclk_clock_t clock);

/* Clock Output */
void rcc_sysclk_set_mco(rcc_sysclk_mco_clock_t clock);

#endif /* RCC_SYSCLK_H_ */
