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
 * rcc_clock_i.h
 *
 *  Created on: Nov 9, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef RCC_CLOCK_I_H_
#define RCC_CLOCK_I_H_

#include "rcc_registers.h"

#define rcc_clocks_set(hse, hsi, msi) \
    do { \
        *rcc_get_CR() = (RCC_CR__MSION_MASK * msi) | (RCC_CR__HSION_MASK * hsi) | \
                        (RCC_CR__HSEON_MASK * hse); \
    } while (0)

#define rcc_hsi_enable_i() \
    do { \
        /* Set the HSION bit */ \
        *rcc_get_CR() |= RCC_CR__HSION_MASK; \
    } while (0)

#define rcc_hsi_disable_i() \
    do { \
        /* Clear the HSION bit */ \
        *rcc_get_CR() &= ~RCC_CR__HSION_MASK; \
    } while (0)

#define rcc_msi_update_frequency_i(freq, trim) \
    do { \
        /* Set the MSIRANGE and MSITRIM configuration */ \
        *rcc_get_ICSCR() = (*rcc_get_ICSCR() & ~(RCC_ICSCR__MSIRANGE_MASK | \
                            RCC_ICSCR__MSITRIM_MASK)) | (trim << 24) | (freq << 13); \
    } while (0)

#define rcc_msi_enable_i(freq, trim) \
    do { \
        rcc_msi_update_frequency_i(freq, trim); \
        \
        /* Set the MSION bit */ \
        *rcc_get_CR() |= RCC_CR__MSION_MASK; \
    } while (0)


#define rcc_msi_disable_i() \
    do { \
        /* Clear the MSION bit */ \
        *rcc_get_CR() &= ~RCC_CR__HSION_MASK; \
    } while (0)

#endif /* RCC_CLOCK_I_H_ */
