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
 * nvic_.h
 *
 *  Created on: Jul 14, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef NVIC__H_
#define NVIC__H_

#include "nvic.h"

enum
{
    NVIC_IRQ_LINE_WWDG = 0,
    NVIC_IRQ_LINE_PVD = 1,
    NVIC_IRQ_LINE_TAMPER = 2,
    NVIC_IRQ_LINE_RTC = 3,
    NVIC_IRQ_LINE_FLASH = 4,
    NVIC_IRQ_LINE_RCC = 5,
    NVIC_IRQ_LINE_EXTI0 = 6,
    NVIC_IRQ_LINE_EXTI1 = 7,
    NVIC_IRQ_LINE_EXTI2 = 8,
    NVIC_IRQ_LINE_EXTI3 = 9,
    NVIC_IRQ_LINE_EXTI4 = 10,
    NVIC_IRQ_LINE_DMA1_STR0 = 11,
    NVIC_IRQ_LINE_DMA1_STR1 = 12,
    NVIC_IRQ_LINE_DMA1_STR2 = 13,
    NVIC_IRQ_LINE_DMA1_STR3 = 14,
    NVIC_IRQ_LINE_DMA1_STR4 = 15,
    NVIC_IRQ_LINE_DMA1_STR5 = 16,
    NVIC_IRQ_LINE_DMA1_STR6 = 17,
    NVIC_IRQ_LINE_ADC = 18,
    NVIC_IRQ_LINE_CAN1_TX = 19,
    NVIC_IRQ_LINE_CAN1_RX0 = 20,
    NVIC_IRQ_LINE_CAN1_RX1 = 21,
    NVIC_IRQ_LINE_CAN1_SCE = 22,
    NVIC_IRQ_LINE_EXTI9_5 = 23,
    NVIC_IRQ_LINE_TIM1_BRK_TIM9 = 24,
    NVIC_IRQ_LINE_TIM1_UP_TIM10 = 25,
    NVIC_IRQ_LINE_TIM1_TRG_COM_TIM11 = 26,
    NVIC_IRQ_LINE_TIM1_CC = 27,
    NVIC_IRQ_LINE_TIM2 = 28,
    NVIC_IRQ_LINE_TIM3 = 29,
    NVIC_IRQ_LINE_TIM4 = 30,
    NVIC_IRQ_LINE_I2C1_EV = 31,
    NVIC_IRQ_LINE_I2C1_ER = 32,
    NVIC_IRQ_LINE_I2C2_EV = 33,
    NVIC_IRQ_LINE_I2C2_ER = 34,
    NVIC_IRQ_LINE_SPI1 = 35,
    NVIC_IRQ_LINE_SPI2 = 36,
    NVIC_IRQ_LINE_USART1 = 37,
    NVIC_IRQ_LINE_USART2 = 38,
    NVIC_IRQ_LINE_USART3 = 39,
    NVIC_IRQ_LINE_EXTI15_10 = 40,
    NVIC_IRQ_LINE_RTC_ALARM = 41,
    NVIC_IRQ_LINE_OTG_FS_WAKEUP = 42,
    NVIC_IRQ_LINE_TIM8_BRK_TIM12 = 43,
    NVIC_IRQ_LINE_TIM8_UP_TIM13 = 44,
    NVIC_IRQ_LINE_TIM8_TRG_COM_TIM14 = 45,
    NVIC_IRQ_LINE_TIM8_CC = 46,
    NVIC_IRQ_LINE_DMA1_STR7 = 47,
    NVIC_IRQ_LINE_FSMC = 48,
    NVIC_IRQ_LINE_SDIO = 49,
    NVIC_IRQ_LINE_TIM5 = 50,
    NVIC_IRQ_LINE_SPI3 = 51,
    NVIC_IRQ_LINE_UART4 = 52,
    NVIC_IRQ_LINE_UART5 = 53,
    NVIC_IRQ_LINE_TIM6_DAC = 54,
    NVIC_IRQ_LINE_TIM7 = 55,
    NVIC_IRQ_LINE_DMA2_STR0 = 56,
    NVIC_IRQ_LINE_DMA2_STR1 = 57,
    NVIC_IRQ_LINE_DMA2_STR2 = 58,
    NVIC_IRQ_LINE_DMA2_STR3 = 59,
    NVIC_IRQ_LINE_DMA2_STR4 = 60,
    NVIC_IRQ_LINE_ETH = 61,
    NVIC_IRQ_LINE_ETH_WKUP = 62,
    NVIC_IRQ_LINE_CAN2_TX = 63,
    NVIC_IRQ_LINE_CAN2_RX0 = 64,
    NVIC_IRQ_LINE_CAN2_RX1 = 65,
    NVIC_IRQ_LINE_CAN2_SCE = 66,
    NVIC_IRQ_LINE_OTG_FS = 67,
    NVIC_IRQ_LINE_DMA2_STR5 = 68,
    NVIC_IRQ_LINE_DMA2_STR6 = 69,
    NVIC_IRQ_LINE_DMA2_STR7 = 70,
    NVIC_IRQ_LINE_USART6 = 71,
    NVIC_IRQ_LINE_I2C3_EV = 72,
    NVIC_IRQ_LINE_I2C3_ER = 73,
    NVIC_IRQ_LINE_OTG_HS_EP1_OUT = 74,
    NVIC_IRQ_LINE_OTG_HS_EP1_IN = 75,
    NVIC_IRQ_LINE_OTG_HS_WKUP = 76,
    NVIC_IRQ_LINE_OTG_HS = 77,
    NVIC_IRQ_LINE_DCMI = 78,
    NVIC_IRQ_LINE_CRYP = 79,
    NVIC_IRQ_LINE_HASH_RNG = 80,
    NVIC_IRQ_LINE_FPU = 81

};

#define NVIC_IRQ_LINE_USB NVIC_IRQ_LINE_USB_LP_CAN1_RX0

#endif /* NVIC__H_ */
