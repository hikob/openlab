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
 * rcc_clock.c
 *
 *  Created on: Jul 15, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "rcc.h"
#include "rcc_registers.h"
#include "pwr.h"
#include "rcc_clock.h"

/* HSI section */
void rcc_hsi_enable()
{
    // Set the HSION bit
    *rcc_get_CR() |= RCC_CR__HSION_MASK;

    // Wait for HSIRDY to be set
    while (!(*rcc_get_CR() & RCC_CR__HSIRDY_MASK))
    {
        asm("nop");
    }
}

void rcc_hsi_disable()
{
    *rcc_get_CR() &= ~RCC_CR__HSION_MASK;
}

bool rcc_is_hsi_enabled()
{
    return (((*rcc_get_CR()) & RCC_CR__HSION_MASK) != 0);
}

/* HSE section */
static uint32_t hse_freq;

void rcc_hse_enable()
{
    // Clear the HSE bypass, set HSE ON
    *rcc_get_CR() &= ~RCC_CR__HSEBYP_MASK;
    *rcc_get_CR() |= RCC_CR__HSEON_MASK;

    // Wait for HSERDY to be set
    while (!(*rcc_get_CR() & RCC_CR__HSERDY_MASK))
    {
        asm("nop");
    }
}

void rcc_hse_disable()
{
    // Clear the HSE bypass, clear HSE ON
    *rcc_get_CR() &= ~(RCC_CR__HSEON_MASK | RCC_CR__HSEBYP_MASK);
}

void rcc_hse_indicate_frequency(uint32_t f)
{
    hse_freq = f;
}

uint32_t rcc_hse_get_frequency()
{
    return hse_freq;
}

/* LSE section */
void rcc_lse_enable()
{
    if  (*rcc_get_BDCR() & RCC_BDCR__LSERDY)
    {
        // Already running
        return;
    }

    pwr_enable_backup_write_protection();

    // Set LSE ON
    *rcc_get_BDCR() |= RCC_BDCR__LSEON;

    // Wait for LSERDY to be set
    while (!(*rcc_get_BDCR() & RCC_BDCR__LSERDY))
    {
        asm("nop");
    }

    pwr_disable_backup_write_protection();
}

void rcc_lse_disable()
{
    pwr_enable_backup_write_protection();

    // Clear LSE ON
    *rcc_get_BDCR() &= ~RCC_BDCR__LSEON;

    pwr_disable_backup_write_protection();
}

/* LSI section */
void rcc_lsi_enable()
{
    //Set LSI ON
    *rcc_get_CSR() |= RCC_CSR__LSION_MASK;

    // Wait for LSIRDY to be set
    while (!(*rcc_get_CSR() & RCC_CSR__LSIRDY_MASK))
    {
        asm("nop");
    }
}

void rcc_lsi_disable()
{
    // Clear LSI ON
    *rcc_get_CSR() &= ~RCC_CSR__LSION_MASK;
}

/* PLL section */
void rcc_pll_enable(rcc_pll_source_t source, rcc_pll_multiplier_t mul, rcc_pll_prediv_t prediv)
{
    // Configure the PLL
    uint32_t reg = *rcc_get_CFGR();

    // Clear PLLMUL PLLSRC
    reg &= ~(RCC_CFGR__PLLMUL_MASK | RCC_CFGR__PLLXTPRE_MASK | RCC_CFGR__PLLSRC_MASK);
    reg |= (mul << 18) | (prediv << 17) | (source << 16);
    *rcc_get_CFGR() = reg;

    // Start the PLL
    *rcc_get_CR() |= RCC_CR__PLLON_MASK;

    // Wait for PLL RDY to be set
    while (!(*rcc_get_CR() & RCC_CR__PLLRDY_MASK))
    {
        asm("nop");
    }
}

void rcc_pll_disable()
{
    // Stop the PLL
    *rcc_get_CR() &= ~RCC_CR__PLLON_MASK;
}

uint32_t rcc_pll_get_frequency()
{
    uint32_t clk = 0;
    uint32_t cfgr = *rcc_get_CFGR();

    switch ((cfgr & RCC_CFGR__PLLSRC_MASK) >> 16)
    {
        case RCC_PLL_SOURCE_HSI:
            clk = rcc_hsi_get_frequency() / 2;
            break;

        case RCC_PLL_SOURCE_HSE:
            clk = rcc_hse_get_frequency();

            if (((cfgr & RCC_CFGR__PLLXTPRE_MASK) >> 17) == RCC_PLL_PREDIV)
            {
                clk /= 2;
            }

            break;
    }

    switch ((cfgr & RCC_CFGR__PLLMUL_MASK) >> 18)
    {
        case RCC_PLL_MUL_2:
            clk *= 2;
            break;
        case RCC_PLL_MUL_3:
            clk *= 3;
            break;
        case RCC_PLL_MUL_4:
            clk *= 4;
            break;
        case RCC_PLL_MUL_5:
            clk *= 5;
            break;
        case RCC_PLL_MUL_6:
            clk *= 6;
            break;
        case RCC_PLL_MUL_7:
            clk *= 7;
            break;
        case RCC_PLL_MUL_8:
            clk *= 8;
            break;
        case RCC_PLL_MUL_9:
            clk *= 9;
            break;
        case RCC_PLL_MUL_10:
            clk *= 10;
            break;
        case RCC_PLL_MUL_11:
            clk *= 11;
            break;
        case RCC_PLL_MUL_12:
            clk *= 12;
            break;
        case RCC_PLL_MUL_13:
            clk *= 13;
            break;
        case RCC_PLL_MUL_14:
            clk *= 14;
            break;
        case RCC_PLL_MUL_15:
            clk *= 15;
            break;
        case RCC_PLL_MUL_16:
            clk *= 16;
            break;
    }

    return clk;
}

