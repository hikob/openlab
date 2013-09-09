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
 * rcc.h
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef RCC_H_
#define RCC_H_

#include "platform.h"

#include "rcc_registers.h"
#include "rcc_sysclk.h"
#include "rcc_clock.h"

typedef enum rcc_ahb_bit
{
    RCC_AHB_BIT_DMA1 = 24,
    RCC_AHB_BIT_FLITF = 15,
    RCC_AHB_BIT_CRC = 12,
    RCC_AHB_BIT_GPIOH = 5,
    RCC_AHB_BIT_GPIOE = 4,
    RCC_AHB_BIT_GPIOD = 3,
    RCC_AHB_BIT_GPIOC = 2,
    RCC_AHB_BIT_GPIOB = 1,
    RCC_AHB_BIT_GPIOA = 0,
} rcc_ahb_bit_t;

static inline void rcc_ahb_enable(rcc_ahb_bit_t bit)
{
    *rcc_get_AHBENR() |= BV(bit);
}
static inline void rcc_ahb_disable(rcc_ahb_bit_t bit)
{
    *rcc_get_AHBENR() &= ~BV(bit);
}
static inline void rcc_ahb_lp_enable(rcc_ahb_bit_t bit)
{
    *rcc_get_AHBLPENR() |= BV(bit);
}
static inline void rcc_ahb_lp_disable(rcc_ahb_bit_t bit)
{
    *rcc_get_AHBLPENR() &= ~BV(bit);
}

typedef enum rcc_apb_bus
{
    // APB2 peripherals
    RCC_APB_BUS_USART1 = 2,
    RCC_APB_BUS_SPI1 = 2,
    RCC_APB_BUS_ADC1 = 2,
    RCC_APB_BUS_TIM11 = 2,
    RCC_APB_BUS_TIM10 = 2,
    RCC_APB_BUS_TIM9 = 2,
    RCC_APB_BUS_SYSCFG = 2,
    // APB1 peripherals
    RCC_APB_BUS_COMP = 1,
    RCC_APB_BUS_DAC = 1,
    RCC_APB_BUS_PWR = 1,
    RCC_APB_BUS_USB = 1,
    RCC_APB_BUS_I2C2 = 1,
    RCC_APB_BUS_I2C1 = 1,
    RCC_APB_BUS_USART3 = 1,
    RCC_APB_BUS_USART2 = 1,
    RCC_APB_BUS_SPI2 = 1,
    RCC_APB_BUS_WWDG = 1,
    RCC_APB_BUS_LCD = 1,
    RCC_APB_BUS_TIM7 = 1,
    RCC_APB_BUS_TIM6 = 1,
    RCC_APB_BUS_TIM4 = 1,
    RCC_APB_BUS_TIM3 = 1,
    RCC_APB_BUS_TIM2 = 1,
} rcc_apb_bus_t;

typedef enum rcc_apb_bit
{
    // APB2 peripherals
    RCC_APB_BIT_USART1 = 14,
    RCC_APB_BIT_SPI1 = 12,
    RCC_APB_BIT_ADC1 = 9,
    RCC_APB_BIT_TIM11 = 4,
    RCC_APB_BIT_TIM10 = 3,
    RCC_APB_BIT_TIM9 = 2,
    RCC_APB_BIT_SYSCFG = 0,
    // APB1 peripherals
    RCC_APB_BIT_COMP = 31,
    RCC_APB_BIT_DAC = 29,
    RCC_APB_BIT_PWR = 28,
    RCC_APB_BIT_USB = 23,
    RCC_APB_BIT_I2C2 = 22,
    RCC_APB_BIT_I2C1 = 21,
    RCC_APB_BIT_USART3 = 18,
    RCC_APB_BIT_USART2 = 17,
    RCC_APB_BIT_SPI2 = 14,
    RCC_APB_BIT_WWDG = 11,
    RCC_APB_BIT_LCD = 9,
    RCC_APB_BIT_TIM7 = 5,
    RCC_APB_BIT_TIM6 = 4,
    RCC_APB_BIT_TIM4 = 2,
    RCC_APB_BIT_TIM3 = 1,
    RCC_APB_BIT_TIM2 = 0,
} rcc_apb_bit_t;

static inline void rcc_apb_enable(rcc_apb_bus_t bus, rcc_apb_bit_t bit)
{
    *rcc_get_APBxENR(bus) |= BV(bit);
}
static inline void rcc_apb_disable(rcc_apb_bus_t bus, rcc_apb_bit_t bit)
{
    *rcc_get_APBxENR(bus) &= ~BV(bit);
}
static inline void rcc_apb_lp_enable(rcc_apb_bus_t bus, rcc_apb_bit_t bit)
{
    *rcc_get_APBxLPENR(bus) |= BV(bit);
}
static inline void rcc_apb_lp_disable(rcc_apb_bus_t bus, rcc_apb_bit_t bit)
{
    *rcc_get_APBxLPENR(bus) &= ~BV(bit);
}
static inline void rcc_enable_dbg_features()
{
    // Disable Clock stopping in low power modes
    *rcc_get_DBGMCU_CR() = 0x3;

    // Enable debug features
    *rcc_get_DBGMCU_APB1_FZ() = 0x00601837;
    *rcc_get_DBGMCU_APB2_FZ() = 0x0000001C;
}

/** Get the reset cause */
static inline platform_reset_cause_t rcc_get_reset_cause()
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
    else if (csr & RCC_CSR__OBLRSTF_MASK)
    {
        return PLATFORM_RESET_OBL;
    }

    return PLATFORM_RESET_UNKNOWN;
}

/** Clear the reset cause */
static inline void rcc_clear_reset_cause()
{
    *rcc_get_CSR() |= RCC_CSR__RMVF_MASK;
    *rcc_get_CSR() &= ~RCC_CSR__RMVF_MASK;
}

#endif /* RCC_H_ */
