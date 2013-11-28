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
 * stm32f1xx.h
 *
 *  Created on: Nov 6, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef STM32F1XX_H_
#define STM32F1XX_H_

#include "gpio_.h"
#include "timer_.h"
#include "uart_.h"
#include "i2c_.h"
#include "spi_.h"
#include "adc_.h"
#include "dma_.h"

extern const _gpio_t _gpioA, _gpioB, _gpioC, _gpioD, _gpioE, _gpioF, _gpioG;
#define GPIO_A (&_gpioA)
#define GPIO_B (&_gpioB)
#define GPIO_C (&_gpioC)
#define GPIO_D (&_gpioD)
#define GPIO_E (&_gpioE)
#define GPIO_F (&_gpioF)
#define GPIO_G (&_gpioG)

extern const _dma_t _dma1_ch2, _dma1_ch3, _dma1_ch4, _dma1_ch5, _dma2_ch4;
#define DMA_1_CH2 (&_dma1_ch2)
#define DMA_1_CH3 (&_dma1_ch3)
#define DMA_1_CH4 (&_dma1_ch4)
#define DMA_1_CH5 (&_dma1_ch5)
#define DMA_2_CH4 (&_dma2_ch4)

extern const _i2c_t _i2c1, _i2c2;
#define I2C_1 (&_i2c1)
#define I2C_2 (&_i2c2)

extern const _spi_t _spi1, _spi2;
#define SPI_1 (&_spi1)
#define SPI_2 (&_spi2)

extern const _uart_t _uart1, _uart2, _uart3;
#define UART_1 (&_uart1)
#define UART_2 (&_uart2)
#define UART_3 (&_uart3)

extern const _openlab_timer_t _tim2, _tim3, _tim4, _tim5, _tim6, _tim7;
#define TIM_2 (&_tim2)
#define TIM_3 (&_tim3)
#define TIM_4 (&_tim4)
#define TIM_5 (&_tim5)
#define TIM_6 (&_tim6)
#define TIM_7 (&_tim7)

extern const _adc_t _adc1;
#define ADC_1 (&_adc1)

#endif /* STM32F1XX_H_ */
