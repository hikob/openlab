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
 * stm32l1xx.h
 *
 *  Created on: Nov 6, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef STM32L1XX_H_
#define STM32L1XX_H_

#include "gpio_.h"
#include "timer_.h"
#include "uart_.h"
#include "i2c_.h"
#include "spi_.h"
#include "adc_.h"
#include "dma_.h"

extern const _gpio_t _gpioA, _gpioB, _gpioC, _gpioD, _gpioH;
#define GPIO_A (&_gpioA)
#define GPIO_B (&_gpioB)
#define GPIO_C (&_gpioC)
#define GPIO_D (&_gpioD)
#define GPIO_H (&_gpioH)

extern const _dma_t _dma1_ch4, _dma1_ch5, _dma1_ch7;
#define DMA_1_CH4 (&_dma1_ch4)
#define DMA_1_CH5 (&_dma1_ch5)
#define DMA_1_CH7 (&_dma1_ch7)

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

extern const _openlab_timer_t _tim2, _tim3, _tim9, _tim10, _tim11;
#define TIM_2 (&_tim2)
#define TIM_3 (&_tim3)
#define TIM_9 (&_tim9)
#define TIM_10 (&_tim10)
#define TIM_11 (&_tim11)

extern const _adc_t _adc1;
#define ADC_1 (&_adc1)

#endif /* STM32L1XX_H_ */
