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
 * stm32l1xx.c
 *
 *  Created on: Nov 6, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "stm32l1xx.h"
#include "nvic_.h"

/* All GPIOs */
GPIO_INIT(_gpioA, GPIO_BASE_ADDRESS + GPIOA_OFFSET, RCC_AHB_BIT_GPIOA);
GPIO_INIT(_gpioB, GPIO_BASE_ADDRESS + GPIOB_OFFSET, RCC_AHB_BIT_GPIOB);
GPIO_INIT(_gpioC, GPIO_BASE_ADDRESS + GPIOC_OFFSET, RCC_AHB_BIT_GPIOC);
GPIO_INIT(_gpioD, GPIO_BASE_ADDRESS + GPIOD_OFFSET, RCC_AHB_BIT_GPIOD);
GPIO_INIT(_gpioH, GPIO_BASE_ADDRESS + GPIOH_OFFSET, RCC_AHB_BIT_GPIOH);

/* Real DMAs */
DMA_INIT(_dma1_ch4, DMA_BASE_ADDRESS, RCC_AHB_BIT_DMA1, DMA_CHANNEL_4,
         NVIC_IRQ_LINE_DMA1_Channel4);
DMA_INIT(_dma1_ch5, DMA_BASE_ADDRESS, RCC_AHB_BIT_DMA1, DMA_CHANNEL_5,
         NVIC_IRQ_LINE_DMA1_Channel5);
DMA_INIT(_dma1_ch7, DMA_BASE_ADDRESS, RCC_AHB_BIT_DMA1, DMA_CHANNEL_7,
         NVIC_IRQ_LINE_DMA1_Channel7);

/* Real I2C */
I2C_INIT(_i2c1, I2C1_BASE_ADDRESS, RCC_APB_BUS_I2C1, RCC_APB_BIT_I2C1,
        NVIC_IRQ_LINE_I2C1_EV, NVIC_IRQ_LINE_I2C1_ER);
I2C_INIT(_i2c2, I2C2_BASE_ADDRESS, RCC_APB_BUS_I2C2, RCC_APB_BIT_I2C2,
        NVIC_IRQ_LINE_I2C2_EV, NVIC_IRQ_LINE_I2C2_ER);

/* Real SPIs */
SPI_INIT(_spi1, SPI1_BASE_ADDRESS, RCC_APB_BUS_SPI1, RCC_APB_BIT_SPI1, NVIC_IRQ_LINE_SPI1);
SPI_INIT(_spi2, SPI2_BASE_ADDRESS, RCC_APB_BUS_SPI2, RCC_APB_BIT_SPI2, NVIC_IRQ_LINE_SPI2);

/* Real Timers */
TIMER_INIT(_tim2, TIM2_BASE_ADDRESS, RCC_APB_BUS_TIM2, RCC_APB_BIT_TIM2, NVIC_IRQ_LINE_TIM2, 4);
TIMER_INIT(_tim3, TIM3_BASE_ADDRESS, RCC_APB_BUS_TIM3, RCC_APB_BIT_TIM3, NVIC_IRQ_LINE_TIM3, 4);
TIMER_INIT(_tim9, TIM9_BASE_ADDRESS, RCC_APB_BUS_TIM9, RCC_APB_BIT_TIM9, NVIC_IRQ_LINE_TIM9, 2);
TIMER_INIT(_tim10, TIM10_BASE_ADDRESS, RCC_APB_BUS_TIM10, RCC_APB_BIT_TIM10, NVIC_IRQ_LINE_TIM10, 1);
TIMER_INIT(_tim11, TIM11_BASE_ADDRESS, RCC_APB_BUS_TIM11, RCC_APB_BIT_TIM11, NVIC_IRQ_LINE_TIM11, 1);

/* Real UARTs */
UART_INIT(_uart1, USART1_BASE_ADDRESS, RCC_APB_BUS_USART1,
        RCC_APB_BIT_USART1, NVIC_IRQ_LINE_USART1);
UART_INIT(_uart2, USART2_BASE_ADDRESS, RCC_APB_BUS_USART2,
        RCC_APB_BIT_USART2, NVIC_IRQ_LINE_USART2);
UART_INIT(_uart3, USART3_BASE_ADDRESS, RCC_APB_BUS_USART3,
        RCC_APB_BIT_USART3, NVIC_IRQ_LINE_USART3);

/* Real ADCs */
ADC_INIT(_adc1, ADC_BASE_ADDRESS, RCC_APB_BUS_ADC1, RCC_APB_BIT_ADC1,
        NVIC_IRQ_LINE_ADC1);


/* Definitions for user access */
const adc_t adc1 = ADC_1;

const gpio_t gpioA = GPIO_A;
const gpio_t gpioB = GPIO_B;
const gpio_t gpioC = GPIO_C;
const gpio_t gpioD = GPIO_D;
const gpio_t gpioH = GPIO_H;

const i2c_t i2c1 = I2C_1;
const i2c_t i2c2 = I2C_2;

const spi_t spi1 = SPI_1;
const spi_t spi2 = SPI_2;

const uart_t uart1 = UART_1;
const uart_t uart2 = UART_2;
const uart_t uart3 = UART_3;

const openlab_timer_t tim2 = TIM_2;
const openlab_timer_t tim3 = TIM_3;
const openlab_timer_t tim9 = TIM_9;
const openlab_timer_t tim10 = TIM_10;
const openlab_timer_t tim11 = TIM_11;
