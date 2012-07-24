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
 * rcc_sysclk.c
 *
 *  Created on: Jul 15, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "rcc.h"
#include "rcc_sysclk.h"

void rcc_sysclk_select_source(rcc_sysclk_source_t source)
{
    // Set SW
    uint32_t reg = *rcc_get_CFGR();
    // Clear SW bits
    reg &= ~RCC_CFGR__SW_MASK;
    // Set SW bits
    reg |= source;
    // Write register
    *rcc_get_CFGR() = reg;

    // Wait for SWS to be set as SW
    while ((((*rcc_get_CFGR()) & RCC_CFGR__SWS_MASK) >> 2) != source)
    {
        asm("nop");
    }

    // Enable debug features
    //*rcc_get_DBGMCU_APB1_FZ() = 0x1837;
    //*rcc_get_DBGMCU_APB2_FZ() = 0x001c;
}

void rcc_sysclk_set_prescalers(rcc_sysclk_ahb_prescaler_t ahb_pres, rcc_sysclk_apb_prescaler_t apb1_pres, rcc_sysclk_apb_prescaler_t apb2_pres)
{
    // Clear prescaler values
    *rcc_get_CFGR() &= ~(RCC_CFGR__HPRE_MASK | RCC_CFGR__PPRE1_MASK | RCC_CFGR__PPRE2_MASK);

    // Set new values
    *rcc_get_CFGR() |= (apb2_pres << 11) | (apb1_pres << 8) | (ahb_pres << 4);
}

uint32_t rcc_sysclk_get_clock_frequency(rcc_sysclk_clock_t clock)
{
    // First compute clk
    static uint32_t clk;

    switch (*rcc_get_CFGR() & RCC_CFGR__SW_MASK)
    {
        case RCC_SYSCLK_SOURCE_HSE:
            // SYSCLK is HSE frequency
            clk = rcc_hse_get_frequency();
            break;

        case RCC_SYSCLK_SOURCE_HSI:
            // SYSCLK is default HSI frequency
            clk = rcc_hsi_get_frequency();
            break;

        case RCC_SYSCLK_SOURCE_PLL:
            // SYSCLK is the output of the PLL
            clk = rcc_pll_get_frequency();
            break;
    }

    // Hoora, here is clk, check if wanted
    if (clock == RCC_SYSCLK_CLOCK_SYSCLK)
    {
        return clk;
    }

    // If not wanted, divide by AHB prescaler to get HCLK
    switch ((*rcc_get_CFGR() >> 4) & 0xF)
    {
        case RCC_SYSCLK_AHB_PRE_2:
            clk /= 2;
            break;

        case RCC_SYSCLK_AHB_PRE_4:
            clk /= 4;
            break;

        case RCC_SYSCLK_AHB_PRE_8:
            clk /= 8;
            break;

        case RCC_SYSCLK_AHB_PRE_16:
            clk /= 16;
            break;

        case RCC_SYSCLK_AHB_PRE_64:
            clk /= 64;
            break;

        case RCC_SYSCLK_AHB_PRE_128:
            clk /= 128;
            break;

        case RCC_SYSCLK_AHB_PRE_256:
            clk /= 256;
            break;

        case RCC_SYSCLK_AHB_PRE_512:
            clk /= 512;
            break;

        default:
            // Other values: not divided
            break;
    }

    // Check if HCLK is wanted
    if (clock == RCC_SYSCLK_CLOCK_HCLK)
    {
        return clk;
    }

    // Check if SYSTICK CLOCK is wanted
    if (clock == RCC_SYSCLK_CLOCK_SYSTICK_CLK)
    {
        // It should be HCLK divided by 8, but in practice it works with 1
        return clk;
    }

    // Check for clock after APB1 prescaler
    if ((clock == RCC_SYSCLK_CLOCK_PCLK1) || (clock == RCC_SYSCLK_CLOCK_PCLK1_TIM))
    {
        uint8_t div;

        // Compute APB1 prescaler
        switch ((*rcc_get_CFGR() >> 8) & 0x7)
        {
            case RCC_SYSCLK_APB_PRE_2:
                div = 2;
                break;

            case RCC_SYSCLK_APB_PRE_4:
                div = 4;
                break;

            case RCC_SYSCLK_APB_PRE_8:
                div = 8;
                break;

            case RCC_SYSCLK_APB_PRE_16:
                div = 16;
                break;

            default:
                // No divider
                div = 1;
                break;
        }

        clk /= div;

        // Check if PCLK1 is wanted
        if (clock == RCC_SYSCLK_CLOCK_PCLK1)
        {
            return clk;
        }
        else
        {
            // PCLK1_TIM is wanted
            if (div == 1)
            {
                return clk;
            }
            else
            {
                return clk * 2;
            }
        }
    }

    // Check for clock after APB2 prescaler
    if ((clock == RCC_SYSCLK_CLOCK_PCLK2) || (clock == RCC_SYSCLK_CLOCK_PCLK2_TIM))
    {
        uint8_t div;

        // Compute APB1 prescaler
        switch ((*rcc_get_CFGR() >> 11) & 0x7)
        {
            case RCC_SYSCLK_APB_PRE_2:
                div = 2;
                break;

            case RCC_SYSCLK_APB_PRE_4:
                div = 4;
                break;

            case RCC_SYSCLK_APB_PRE_8:
                div = 8;
                break;

            case RCC_SYSCLK_APB_PRE_16:
                div = 16;
                break;

            default:
                // No divider
                div = 1;
                break;
        }

        clk /= div;

        // Check if PCLK1 is wanted
        if (clock == RCC_SYSCLK_CLOCK_PCLK2)
        {
            return clk;
        }
        else
        {
            // PCLK1_TIM is wanted
            if (div == 1)
            {
                return clk;
            }
            else
            {
                return clk * 2;
            }
        }
    }

    return clk;
}

void rcc_sysclk_set_mco(rcc_sysclk_mco_clock_t clock)
{
    uint32_t cfgr = *rcc_get_CFGR();

    cfgr &= ~RCC_CFGR__MCOSEL_MASK;
    cfgr |= (clock << 24);

    *rcc_get_CFGR() = cfgr;
}
