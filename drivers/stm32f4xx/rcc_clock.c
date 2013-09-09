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
 * rcc_clock.c
 *
 *  Created on: Oct 22, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "rcc_registers.h"
#include "rcc.h"
#include "rcc_clock.h"
#include "pwr.h"

/* HSI section */
void rcc_hsi_enable()
{
    // Set the HSION bit
    *rcc_get_CR() |= RCC_CR__HSION;

    // Wait for HSIRDY to be set
    while (!(*rcc_get_CR() & RCC_CR__HSIRDY))
    {
        asm("nop");
    }
}

void rcc_hsi_disable()
{
    *rcc_get_CR() &= ~RCC_CR__HSION;
}

/* HSE section */
static uint32_t hse_freq;

void rcc_hse_enable()
{
    // Clear the HSE bypass, set HSE ON
    *rcc_get_CR() &= ~RCC_CR__HSEBYP;
    *rcc_get_CR() |= RCC_CR__HSEON;

    // Wait for HSERDY to be set
    while (!(*rcc_get_CR() & RCC_CR__HSERDY))
    {
        asm("nop");
    }
}

void rcc_hse_disable()
{
    // Clear the HSE bypass, clear HSE ON
    *rcc_get_CR() &= ~(RCC_CR__HSEON | RCC_CR__HSEBYP);
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
    // Check if LSE already started
    if (*rcc_get_BDCR() & RCC_BDCR__LSERDY)
    {
        // Nothing to do, return
        return;
    }

    // Unlock write access to the registers
    pwr_disable_backup_write_protection();

    // Clear LSE bypass, Set LSE ON
    *rcc_get_BDCR() &= ~RCC_BDCR__LSEBYP;
    *rcc_get_BDCR() |= RCC_BDCR__LSEON;

    // Wait for LSERDY to be set
    while (!(*rcc_get_BDCR() & RCC_BDCR__LSERDY))
    {
    }

    // Lock back write access
    pwr_enable_backup_write_protection();
}
void rcc_lse_disable()
{
    // Unlock the write access to the registers
    pwr_disable_backup_write_protection();

    // Clear LSE bypass, Clear LSE ON
    *rcc_get_BDCR() &= ~(RCC_BDCR__LSEON| RCC_BDCR__LSEBYP);

    // Lock the protection back
    pwr_enable_backup_write_protection();
}

/* LSI section */
void rcc_lsi_enable()
{
    //Set LSI ON
    *rcc_get_CSR() |= RCC_CSR__LSION;

    // Wait for LSERDY to be set
    while (!(*rcc_get_CSR() & RCC_CSR__LSIRDY))
    {
        asm("nop");
    }
}

void rcc_lsi_disable()
{
    // Clear LSI ON
    *rcc_get_CSR() &= ~RCC_CSR__LSION;
}

/* PLL section */
void rcc_pll_enable(rcc_pll_source_t source, uint32_t input_div,
        uint32_t input_mul, rcc_pll_main_clock_div_t main_div, uint32_t usb_div)
{
    // Stop the pll
    *rcc_get_CR() &= ~RCC_CR__PLLON;

    *rcc_get_PLLCFGR() = (usb_div << 24) | (source << 22) | (main_div << 16)
            | (input_mul << 6) | (input_div << 0);

    // Start the PLL
    *rcc_get_CR() |= RCC_CR__PLLON;

    // Wait for PLL RDY to be set
    while (!(*rcc_get_CR() & RCC_CR__PLLRDY))
    {
        asm("nop");
    }
}

void rcc_pll_disable()
{
    // Stop the PLL
    *rcc_get_CR() &= ~RCC_CR__PLLON;
}

uint32_t rcc_pll_get_frequency()
{
    uint32_t clk = 0;

    // Get base frequency
    if (*rcc_get_PLLCFGR() & RCC_PLLCFGR__PLLSRC)
    {
        clk = rcc_hse_get_frequency();
    }
    else
    {
        clk = rcc_hsi_get_frequency();
    }

    // Apply input divider
    clk /= (*rcc_get_PLLCFGR() & RCC_PLLCFGR__PLLM_MASK);

    // Apply input multiplier
    clk *= (*rcc_get_PLLCFGR() & RCC_PLLCFGR__PLLN_MASK) >> 6;

    // Apply the main clock divider
    clk /= 2 * (((*rcc_get_PLLCFGR() >> 16) & 0x3) + 1);

    return clk;
}

