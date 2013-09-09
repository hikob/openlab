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
 * rcc_clock.c
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "rcc_registers.h"
#include "rcc_clock.h"
#include "pwr.h"

/* MSI section */
static uint32_t msi_freq = 2097000;

void rcc_msi_enable(rcc_msi_frequency_t freq, uint8_t trim)
{
    // Set the MSIRANGE and MSITRIM configuration
    *rcc_get_ICSCR() &= ~(RCC_ICSCR__MSIRANGE_MASK | RCC_ICSCR__MSITRIM_MASK);
    *rcc_get_ICSCR() |= (trim << 24) | (freq << 13);

    // Start the MSI
    *rcc_get_CR() |= RCC_CR__MSION_MASK;

    // Wait until ready
    while (!(*rcc_get_CR() & RCC_CR__MSIRDY_MASK))
    {
    }
}

void rcc_msi_compute_frequency()
{
    rcc_msi_frequency_t freq = (*rcc_get_ICSCR() & RCC_ICSCR__MSIRANGE_MASK)
                               >> 13;

    // Store the corresponding frequency
    switch (freq)
    {
        case RCC_MSI_FREQ_65kHz:
            msi_freq = 65536;
            break;
        case RCC_MSI_FREQ_131kHz:
            msi_freq = 131072;
            break;
        case RCC_MSI_FREQ_262kHz:
            msi_freq = 262144;
            break;
        case RCC_MSI_FREQ_524kHz:
            msi_freq = 524288;
            break;
        case RCC_MSI_FREQ_1048kHz:
            msi_freq = 1048000;
            break;
        case RCC_MSI_FREQ_2097kHz:
            msi_freq = 2097000;
            break;
        case RCC_MSI_FREQ_4194kHz:
            msi_freq = 4194000;
            break;
    }
}
void rcc_msi_trim(uint8_t trim)
{
    *rcc_get_ICSCR() &= ~RCC_ICSCR__MSITRIM_MASK;
    *rcc_get_ICSCR() |= trim << 24;
}
void rcc_msi_disable()
{
    // Stop the MSI
    *rcc_get_CR() &= ~RCC_CR__MSION_MASK;
}

uint32_t rcc_msi_get_frequency()
{
    return msi_freq;
}

/* HSI section */
void rcc_hsi_enable()
{
    // Set the HSION bit
    *rcc_get_CR() |= RCC_CR__HSION_MASK;

    // Wait for HSIRDY to be set
    while (!(*rcc_get_CR() & RCC_CR__HSIRDY_MASK))
    {
    }
}

void rcc_hsi_disable()
{
    *rcc_get_CR() &= ~RCC_CR__HSION_MASK;
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
    // Check if LSE already started
    if (*rcc_get_CSR() & RCC_CSR__LSERDY_MASK)
    {
        // Nothing to do, return
        return;
    }

    // Unlock write access to the registers
    pwr_disable_backup_write_protection();

    // Clear LSE bypass, Set LSE ON
    *rcc_get_CSR() &= ~RCC_CSR__LSEBYP_MASK;
    *rcc_get_CSR() |= RCC_CSR__LSEON_MASK;

    // Wait for LSERDY to be set
    while (!(*rcc_get_CSR() & RCC_CSR__LSERDY_MASK))
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
    *rcc_get_CSR() &= ~(RCC_CSR__LSEON_MASK | RCC_CSR__LSEBYP_MASK);

    // Lock the protection back
    pwr_enable_backup_write_protection();
}

/* LSI section */
void rcc_lsi_enable()
{
    //Set LSI ON
    *rcc_get_CSR() |= RCC_CSR__LSION_MASK;

    // Wait for LSERDY to be set
    while (!(*rcc_get_CSR() & RCC_CSR__LSIRDY_MASK))
    {
    }
}

void rcc_lsi_disable()
{
    // Clear LSI ON
    *rcc_get_CSR() &= ~RCC_CSR__LSION_MASK;
}

/* PLL section */
static uint32_t pll_freq;
void rcc_pll_enable(rcc_pll_source_t source, rcc_pll_multiplier_t mul,
                    rcc_pll_divider_t div)
{
    // Configure the PLL
    uint32_t reg = *rcc_get_CFGR();

    // Clear PLLDIV PLLMUL PLLSRC
    reg &= ~(RCC_CFGR__PLLDIV_MASK | RCC_CFGR__PLLMUL_MASK
             | RCC_CFGR__PLLSRC_MASK);
    reg |= (div << 22) | (mul << 18) | (source << 16);
    *rcc_get_CFGR() = reg;

    // Start the PLL
    *rcc_get_CR() |= RCC_CR__PLLON_MASK;

    // Wait for PLL RDY to be set
    while (!(*rcc_get_CR() & RCC_CR__PLLRDY_MASK))
    {
    }

    // Compute the frequency we've just set
    uint32_t freq;

    // Get the source clok
    if (source == RCC_PLL_SOURCE_HSE)
    {
        freq = rcc_hse_get_frequency();
    }
    else
    {
        freq = rcc_hsi_get_frequency();
    }

    // Multiply by the multiplier field
    switch (mul)
    {
        case RCC_PLL_MUL_3:
            freq *= 3;
            break;
        case RCC_PLL_MUL_4:
            freq *= 4;
            break;
        case RCC_PLL_MUL_6:
            freq *= 6;
            break;
        case RCC_PLL_MUL_8:
            freq *= 8;
            break;
        case RCC_PLL_MUL_12:
            freq *= 12;
            break;
        case RCC_PLL_MUL_16:
            freq *= 16;
            break;
        case RCC_PLL_MUL_24:
            freq *= 24;
            break;
        case RCC_PLL_MUL_32:
            freq *= 32;
            break;
        case RCC_PLL_MUL_48:
            freq *= 48;
            break;
    }

    // Divide by the divider
    switch (div)
    {
        case RCC_PLL_DIV_2:
            freq /= 2;
            break;
        case RCC_PLL_DIV_3:
            freq /= 3;
            break;
        case RCC_PLL_DIV_4:
            freq /= 4;
            break;
    }

    // Store the value
    pll_freq = freq;
}

void rcc_pll_disable()
{
    // Stop the PLL
    *rcc_get_CR() &= ~RCC_CR__PLLON_MASK;
}

uint32_t rcc_pll_get_frequency()
{
    return pll_freq;
}
