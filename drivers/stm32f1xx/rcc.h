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
 * rcc.h
 *
 *  Created on: Jul 14, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef RCC_H_
#define RCC_H_

#include "platform.h"

#include "rcc_registers.h"
#include "rcc_sysclk.h"
#include "rcc_clock.h"

typedef enum
{
    RCC_AHB_BIT_DMA1 = 0,
    RCC_AHB_BIT_DMA2 = 1,
    RCC_AHB_BIT_SRAM = 2,
    RCC_AHB_BIT_FLITF = 4,
    RCC_AHB_BIT_CRC = 6,
    RCC_AHB_BIT_FSMC = 8,
    RCC_AHB_BIT_SDIO = 10
} rcc_ahb_bit_t;

void rcc_ahb_enable(rcc_ahb_bit_t bit);
void rcc_ahb_disable(rcc_ahb_bit_t bit);

typedef enum
{
    RCC_APB1 = 1,
    RCC_APB2 = 2
} rcc_apb_bus_t;

enum
{
    // APB1
    RCC_APB_BUS_TIM2 = RCC_APB1,
    RCC_APB_BUS_TIM3 = RCC_APB1,
    RCC_APB_BUS_TIM4 = RCC_APB1,
    RCC_APB_BUS_TIM5 = RCC_APB1,
    RCC_APB_BUS_TIM6 = RCC_APB1,
    RCC_APB_BUS_TIM7 = RCC_APB1,
    RCC_APB_BUS_TIM12 = RCC_APB1,
    RCC_APB_BUS_TIM13 = RCC_APB1,
    RCC_APB_BUS_TIM14 = RCC_APB1,
    RCC_APB_BUS_WWDG = RCC_APB1,
    RCC_APB_BUS_SPI2 = RCC_APB1,
    RCC_APB_BUS_SPI3 = RCC_APB1,
    RCC_APB_BUS_USART2 = RCC_APB1,
    RCC_APB_BUS_USART3 = RCC_APB1,
    RCC_APB_BUS_UART4 = RCC_APB1,
    RCC_APB_BUS_UART5 = RCC_APB1,
    RCC_APB_BUS_I2C1 = RCC_APB1,
    RCC_APB_BUS_I2C2 = RCC_APB1,
    RCC_APB_BUS_USB = RCC_APB1,
    RCC_APB_BUS_CAN = RCC_APB1,
    RCC_APB_BUS_BKP = RCC_APB1,
    RCC_APB_BUS_PWR = RCC_APB1,
    RCC_APB_BUS_DAC = RCC_APB1,


    // APB2
    RCC_APB_BUS_AFIO = RCC_APB2,
    RCC_APB_BUS_GPIOA = RCC_APB2,
    RCC_APB_BUS_GPIOB = RCC_APB2,
    RCC_APB_BUS_GPIOC = RCC_APB2,
    RCC_APB_BUS_GPIOD = RCC_APB2,
    RCC_APB_BUS_GPIOE = RCC_APB2,
    RCC_APB_BUS_GPIOF = RCC_APB2,
    RCC_APB_BUS_GPIOG = RCC_APB2,
    RCC_APB_BUS_ADC1 = RCC_APB2,
    RCC_APB_BUS_ADC2 = RCC_APB2,
    RCC_APB_BUS_TIM1 = RCC_APB2,
    RCC_APB_BUS_SPI1 = RCC_APB2,
    RCC_APB_BUS_TIM8 = RCC_APB2,
    RCC_APB_BUS_USART1 = RCC_APB2,
    RCC_APB_BUS_ADC3 = RCC_APB2,
    RCC_APB_BUS_TIM9 = RCC_APB2,
    RCC_APB_BUS_TIM10 = RCC_APB2,
    RCC_APB_BUS_TIM11 = RCC_APB2,
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
    RCC_APB_BIT_USB = 23,
    RCC_APB_BIT_CAN = 25,
    RCC_APB_BIT_BKP = 27,
    RCC_APB_BIT_PWR = 28,
    RCC_APB_BIT_DAC = 29,

    // APB2
    RCC_APB_BIT_AFIO = 0,
    RCC_APB_BIT_GPIOA = 2,
    RCC_APB_BIT_GPIOB = 3,
    RCC_APB_BIT_GPIOC = 4,
    RCC_APB_BIT_GPIOD = 5,
    RCC_APB_BIT_GPIOE = 6,
    RCC_APB_BIT_GPIOF = 7,
    RCC_APB_BIT_GPIOG = 8,
    RCC_APB_BIT_ADC1 = 9,
    RCC_APB_BIT_ADC2 = 10,
    RCC_APB_BIT_TIM1 = 11,
    RCC_APB_BIT_SPI1 = 12,
    RCC_APB_BIT_TIM8 = 13,
    RCC_APB_BIT_USART1 = 14,
    RCC_APB_BIT_ADC3 = 15,
    RCC_APB_BIT_TIM9 = 19,
    RCC_APB_BIT_TIM10 = 20,
    RCC_APB_BIT_TIM11 = 21
} rcc_apb_bit_t;

void rcc_apb_enable(rcc_apb_bus_t bus, rcc_apb_bit_t bit);
void rcc_apb_disable(rcc_apb_bus_t bus, rcc_apb_bit_t bit);

platform_reset_cause_t rcc_get_reset_cause();
void rcc_clear_reset_cause();

#endif /* RCC_H_ */
