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
 * nvic_.h
 *
 *  Created on: Jul 14, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef NVIC__H_
#define NVIC__H_

#include "nvic.h"

enum
{
    NVIC_IRQ_LINE_WWDG          = 0,
    NVIC_IRQ_LINE_PVD           = 1,
    NVIC_IRQ_LINE_TAMPER_STAMP  = 2,
    NVIC_IRQ_LINE_RTC_WKUP      = 3,
    NVIC_IRQ_LINE_FLASH         = 4,
    NVIC_IRQ_LINE_RCC           = 5,
    NVIC_IRQ_LINE_EXTI0         = 6,
    NVIC_IRQ_LINE_EXTI1         = 7,
    NVIC_IRQ_LINE_EXTI2         = 8,
    NVIC_IRQ_LINE_EXTI3         = 9,
    NVIC_IRQ_LINE_EXTI4         = 10,
    NVIC_IRQ_LINE_DMA1_Channel1 = 11,
    NVIC_IRQ_LINE_DMA1_Channel2 = 12,
    NVIC_IRQ_LINE_DMA1_Channel3 = 13,
    NVIC_IRQ_LINE_DMA1_Channel4 = 14,
    NVIC_IRQ_LINE_DMA1_Channel5 = 15,
    NVIC_IRQ_LINE_DMA1_Channel6 = 16,
    NVIC_IRQ_LINE_DMA1_Channel7 = 17,
    NVIC_IRQ_LINE_ADC1          = 18,
    NVIC_IRQ_LINE_USB_HP        = 19,
    NVIC_IRQ_LINE_USB_LP        = 20,
    NVIC_IRQ_LINE_DAC           = 21,
    NVIC_IRQ_LINE_COMP          = 22,
    NVIC_IRQ_LINE_EXTI9_5       = 23,
    NVIC_IRQ_LINE_LCD           = 24,
    NVIC_IRQ_LINE_TIM9          = 25,
    NVIC_IRQ_LINE_TIM10         = 26,
    NVIC_IRQ_LINE_TIM11         = 27,
    NVIC_IRQ_LINE_TIM2          = 28,
    NVIC_IRQ_LINE_TIM3          = 29,
    NVIC_IRQ_LINE_TIM4          = 30,
    NVIC_IRQ_LINE_I2C1_EV       = 31,
    NVIC_IRQ_LINE_I2C1_ER       = 32,
    NVIC_IRQ_LINE_I2C2_EV       = 33,
    NVIC_IRQ_LINE_I2C2_ER       = 34,
    NVIC_IRQ_LINE_SPI1          = 35,
    NVIC_IRQ_LINE_SPI2          = 36,
    NVIC_IRQ_LINE_USART1        = 37,
    NVIC_IRQ_LINE_USART2        = 38,
    NVIC_IRQ_LINE_USART3        = 39,
    NVIC_IRQ_LINE_EXTI15_10     = 40,
    NVIC_IRQ_LINE_RTC_Alarm     = 41,
    NVIC_IRQ_LINE_USB_FS_WKUP   = 42,
    NVIC_IRQ_LINE_TIM6          = 43,
    NVIC_IRQ_LINE_TIM7          = 44
};

#endif /* NVIC__H_ */
