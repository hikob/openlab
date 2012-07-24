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
 * rcc.c
 *
 *  Created on: Jul 15, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef RCC_C_
#define RCC_C_
#include "rcc.h"

void rcc_ahb_enable(rcc_ahb_bit_t bit)
{
    *rcc_get_AHBENR() |= BV(bit);
}

void rcc_ahb_disable(rcc_ahb_bit_t bit)
{
    *rcc_get_AHBENR() &= ~BV(bit);
}

void rcc_apb_enable(rcc_apb_bus_t bus, rcc_apb_bit_t bit)
{
    *rcc_get_APBxENR(bus) |= BV(bit);
}

void rcc_apb_disable(rcc_apb_bus_t bus, rcc_apb_bit_t bit)
{
    *rcc_get_APBxENR(bus) &= ~BV(bit);
}


platform_reset_cause_t rcc_get_reset_cause()
{
    uint32_t csr = *rcc_get_CSR();

    if (csr & RCC_CSR__LPWRRSTF_MASK)
    {
        return PLATFORM_RESET_LP;
    }
    else if (csr & RCC_CSR__WWDGRSTF_MASK)
    {
        return PLATFORM_RESET_WWD;
    }
    else if (csr & RCC_CSR__IWDGRSTF_MASK)
    {
        return PLATFORM_RESET_IWD;
    }
    else if (csr & RCC_CSR__SFTRSTF_MASK)
    {
        return PLATFORM_RESET_SOFT;
    }
    else if (csr & RCC_CSR__PORRSTF_MASK)
    {
        return PLATFORM_RESET_POR;
    }
    else if (csr & RCC_CSR__PINRSTF_MASK)
    {
        return PLATFORM_RESET_PIN;
    }

    return PLATFORM_RESET_UNKNOWN;
}

void rcc_clear_reset_cause()
{
    *rcc_get_CSR() |= RCC_CSR__RMVF_MASK;
    *rcc_get_CSR() &= ~RCC_CSR__RMVF_MASK;
}

#endif /* RCC_C_ */
