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
 * rcc.h
 *
 *  Created on: Oct 22, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef RCC_H_
#define RCC_H_

#include "rcc_registers.h"
#include "rcc_sysclk.h"
#include "rcc_clock.h"

typedef enum
{
    RCC_AHB_BIT_GPIOA = 0,
    RCC_AHB_BIT_GPIOB = 1,
    RCC_AHB_BIT_GPIOC = 2,
    RCC_AHB_BIT_GPIOD = 3,
    RCC_AHB_BIT_GPIOE = 4,
    RCC_AHB_BIT_GPIOF = 5,
    RCC_AHB_BIT_GPIOG = 6,
    RCC_AHB_BIT_GPIOH = 7,
    RCC_AHB_BIT_GPIOI = 8,

    RCC_AHB_BIT_CRC = 12,

    RCC_AHB_BIT_BKPSRAM = 18,

    RCC_AHB_BIT_CCMDATARAM = 20,
    RCC_AHB_BIT_DMA1 = 21,
    RCC_AHB_BIT_DMA2 = 22,

    RCC_AHB_BIT_ETHMAC = 25,
    RCC_AHB_BIT_ETHMACTX = 26,
    RCC_AHB_BIT_ETHMACRX = 27,
    RCC_AHB_BIT_ETHMACPTP = 28,
    RCC_AHB_BIT_OTGHS = 29,
    RCC_AHB_BIT_OTGHSULPI = 30

} rcc_ahb_bit_t;

void rcc_ahb_enable(rcc_ahb_bit_t bit);
void rcc_ahb_disable(rcc_ahb_bit_t bit);

typedef enum
{
    RCC_APB1 = 1, RCC_APB2 = 2
} rcc_apb_bus_t;

enum
{
    RCC_APB_BUS_USART1 = RCC_APB2,
    RCC_APB_BUS_USART2 = RCC_APB1,
    RCC_APB_BUS_USART3 = RCC_APB1,
    RCC_APB_BUS_UART4 = RCC_APB1,
    RCC_APB_BUS_UART5 = RCC_APB1,
    RCC_APB_BUS_USART6 = RCC_APB2,

    RCC_APB_BUS_TIM1 = RCC_APB2,
    RCC_APB_BUS_TIM2 = RCC_APB1,
    RCC_APB_BUS_TIM3 = RCC_APB1,
    RCC_APB_BUS_TIM4 = RCC_APB1,
    RCC_APB_BUS_TIM5 = RCC_APB1,
    RCC_APB_BUS_TIM6 = RCC_APB1,
    RCC_APB_BUS_TIM7 = RCC_APB1,
    RCC_APB_BUS_TIM8 = RCC_APB2,
    RCC_APB_BUS_TIM9 = RCC_APB2,
    RCC_APB_BUS_TIM10 = RCC_APB2,
    RCC_APB_BUS_TIM11 = RCC_APB2,
    RCC_APB_BUS_TIM12 = RCC_APB1,
    RCC_APB_BUS_TIM13 = RCC_APB1,
    RCC_APB_BUS_TIM14 = RCC_APB1,

    RCC_APB_BUS_SPI1 = RCC_APB2,
    RCC_APB_BUS_SPI2 = RCC_APB1,
    RCC_APB_BUS_SPI3 = RCC_APB1,

    RCC_APB_BUS_I2C1 = RCC_APB1,
    RCC_APB_BUS_I2C2 = RCC_APB1,
    RCC_APB_BUS_I2C3 = RCC_APB1,

    RCC_APB_BUS_CAN1 = RCC_APB1,
    RCC_APB_BUS_CAN2 = RCC_APB1,

    RCC_APB_BUS_WWDG = RCC_APB1,

    RCC_APB_BUS_PWR = RCC_APB1,
    RCC_APB_BUS_DAC = RCC_APB1,

    RCC_APB_BUS_SDIO = RCC_APB2,
    RCC_APB_BUS_SYSCFG = RCC_APB2,

    RCC_APB_BUS_ADC1 = RCC_APB2,
    RCC_APB_BUS_ADC2 = RCC_APB2,
    RCC_APB_BUS_ADC3 = RCC_APB2,
};

typedef enum
{
    // APB1
    RCC_APB_BIT_TIM2 = 0,
    RCC_APB_BIT_TIM3 = 1,
    RCC_APB_BIT_TIM4 = 2,
    RCC_APB_BIT_TIM5 = 3,
    RCC_APB_BIT_TIM6 = 4,
    RCC_APB_BIT_TIM7 = 5,
    RCC_APB_BIT_TIM12 = 6,
    RCC_APB_BIT_TIM13 = 7,
    RCC_APB_BIT_TIM14 = 8,

    RCC_APB_BIT_WWDG = 11,

    RCC_APB_BIT_SPI2 = 14,
    RCC_APB_BIT_SPI3 = 15,

    RCC_APB_BIT_USART2 = 17,
    RCC_APB_BIT_USART3 = 18,
    RCC_APB_BIT_UART4 = 19,
    RCC_APB_BIT_UART5 = 20,
    RCC_APB_BIT_I2C1 = 21,
    RCC_APB_BIT_I2C2 = 22,
    RCC_APB_BIT_I2C3 = 23,

    RCC_APB_BIT_CAN1 = 25,
    RCC_APB_BIT_CAN2 = 26,

    RCC_APB_BIT_PWR = 28,
    RCC_APB_BIT_DAC = 29,

    // APB2
    RCC_APB_BIT_TIM1 = 0,
    RCC_APB_BIT_TIM8 = 1,

    RCC_APB_BIT_USART1 = 4,
    RCC_APB_BIT_USART6 = 5,

    RCC_APB_BIT_ADC1 = 8,
    RCC_APB_BIT_ADC2 = 9,
    RCC_APB_BIT_ADC3 = 10,
    RCC_APB_BIT_SDIO = 11,
    RCC_APB_BIT_SPI1 = 12,

    RCC_APB_BIT_SYSCFG = 14,

    RCC_APB_BIT_TIM9 = 16,
    RCC_APB_BIT_TIM10 = 17,
    RCC_APB_BIT_TIM11 = 18,
} rcc_apb_bit_t;

void rcc_apb_enable(rcc_apb_bus_t bus, rcc_apb_bit_t bit);
void rcc_apb_disable(rcc_apb_bus_t bus, rcc_apb_bit_t bit);
void rcc_apb_reset(rcc_apb_bus_t bus, rcc_apb_bit_t bit);

#endif /* RCC_H_ */
