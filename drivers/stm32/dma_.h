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
 * dma_.h
 *
 *  Created on: Aug 19, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef DMA__H_
#define DMA__H_

#include <stdint.h>
#include "handler.h"
#include "rcc.h"
#include "nvic.h"
#include "dma.h"

/*
 * The available peripheral per DMA channels for the STM32L1xx chips are:
 * Ch.1: ADC1        / TIM2_CH3   / TIM4_CH1     /
 * Ch.2: SPI1_RX     / USART3_TX  / TIM2_UP      / TIM3_CH3    / TIM6_UP
 * Ch.3: SPI1_TX     / USART3_RX  / TIM3_CH4-UP  / TIM7_UP
 * Ch.4: SPI2_RX     / USART1_TX  / I2C2_TX      / TIM4_CH2
 * Ch.5: SPI2_TX     / USART1_RX  / I2C2_RX      / TIM2_CH1    / TIM4_CH3
 * Ch.6: USART2_RX   / I2C1_TX    / TIM3_CH1
 * Ch.7: USART2_TX   / I2C1_RX    / TIM2_CH2-CH4 / TIM4_UP
 */

/*
 * The available peripheral per DMA1 channels for the STM32F1xx chips are:
 * Ch.1: ADC1      / TIM2_CH3  / TIM4_CH1
 * Ch.2: SPI1_RX   / USART3_TX / TIM2_UP   / TIM3_CH3 / TIM1_CH1
 * Ch.3: SPI1_TX   / USART3_RX / TIM3_CH4  / TIM3_UP  / TIM1_CH2
 * Ch.4: SPI2_RX   / I2S2_RX   / USART1_TX / I2C2_TX  / TIM4_CH2  / TIM1_CH4 / TIM1_TRIG / TIM1_COM
 * Ch.5: SPI2_TX   / I2S2_TX   / USART1_RX / I2C2_RX  / TIM2_CH1  / TIM4_CH3 / TIM1_UP
 * Ch.6: USART2_RX / I2C1_TX   / TIM3_CH1  / TIM1_CH3 / TIM3_TRIG
 * Ch.7: USART2_TX / I2C1_RX   / TIM2_CH2  / TIM2_CH4 / TIM4_UP
 *
 * The available peripheral per DMA2 channels are:
 * Ch.1: TIM5_CH4 / TIM5_TRIG / TIM8_CH3 / TIM8_UP  / SPI3_RX / I2S3_RX
 * Ch.2: TIM8_CH4 / TIM8_TRIG / TIM8_COM / TIM5_CH3 / TIM5_UP / SPI3_TX / I2S3_TX
 * Ch.3: TIM8_CH1 / UART4_RX  / TIM6_UP  / DAC_Channel1
 * Ch.4: TIM5_CH2 / SDIO      / TIM7_UP  / DAC_Channel2
 * Ch.5: ADC3     / TIM8_CH2  / TIM5_CH1 / UART4_TX
 */

typedef struct
{
    // The handler for transfer done
    handler_t handler;
    handler_arg_t handler_arg;
} _dma_data_t;

typedef struct
{
    // DMA base address
    uint32_t base_address;

    // DMA RCC line
    rcc_ahb_bit_t dma_bit;

    // The DMA channel
    dma_channel_t channel;

    // DMA channel IRQ line
    nvic_irq_line_t irq_line;

    // The data
    _dma_data_t *data;
} _dma_t;

/**
 * Initialize a DMA object.
 *
 * \param dma the DMA object to initialize.
 * \param channel the DMA channel of this object.
 */
#define DMA_INIT(name, addr, bit, chan, line) \
    static _dma_data_t name##_data; \
    const _dma_t name = { \
    .base_address = addr, \
    .dma_bit = bit, \
    .channel = chan, \
    .irq_line = line, \
    .data = &name##_data \
}

/**
 * Handle a DMA channel interrupt.
 */
void dma_handle_interrupt(const _dma_t *_dma);

#endif /* DMA__H_ */
