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
 * dma.c
 *
 *  Created on: Aug 19, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "dma.h"
#include "dma_.h"
#include "dma_registers.h"
#include "nvic.h"
#include "nvic_.h"
#include "rcc.h"

void dma_enable(dma_t dma)
{
    const _dma_t *_dma = dma;

    // Enable the DMA in the RCC
    rcc_ahb_enable(_dma->dma_bit);

    // Enable the interrupt for the DMA channel
    nvic_enable_interrupt_line(_dma->irq_line);
}

void dma_disable(dma_t dma)
{
    const _dma_t *_dma = dma;

    // Disable the DMA in the RCC
    rcc_ahb_disable(_dma->dma_bit);

    // Disable the interrupt for the DMA channel
    nvic_disable_interrupt_line(_dma->irq_line);
}

void dma_config(dma_t dma, uint32_t peripheral_address,
                uint32_t memory_address, uint16_t transfer_number,
                dma_size_t transfer_size, dma_direction_t direction,
                dma_increment_t memory_increment)
{
    const _dma_t *_dma = dma;

    // Disable the DMA channel and clear all flags
    *dma_get_CCRx(_dma) = 0;
    *dma_get_IFCR(_dma) = (DMA_IFCR__CGIFx | DMA_IFCR__CTCIFx
                           | DMA_IFCR__CHTIFx | DMA_IFCR__CTEIFx) << (DMA_IFCR__CHANNEL_OFFSET
                                   * _dma->channel);

    // Configure the channel, with memory increment if required,
    // peripheral no increment
    // Do not enable yet the transfer complete interrupt
    *dma_get_CCRx(_dma)
    = (DMA_CCR__PL_MEDIUM | (transfer_size << 10)
       | (transfer_size << 8) | (memory_increment << 7)
       | (direction << 4));

    // Store the transfer number
    *dma_get_CNDTRx(_dma) = transfer_number;

    // Store the memory and peripheral address
    *dma_get_CMARx(_dma) = memory_address;
    *dma_get_CPARx(_dma) = peripheral_address;
}

void dma_start(dma_t dma, handler_t handler, handler_arg_t handler_arg)
{
    const _dma_t *_dma = dma;

    // Store the handlers
    _dma->data->handler = handler;
    _dma->data->handler_arg = handler_arg;

    // Enable the transfer complete interrupt
    *dma_get_CCRx(_dma) |= DMA_CCR__TCIE;

    // Set the EN bit to start the channel
    *dma_get_CCRx(_dma) |= DMA_CCR__EN;
}

int32_t dma_cancel(dma_t dma)
{
    const _dma_t *_dma = dma;

    int32_t canceled = 0;

    // Disable interrupts
    platform_enter_critical();

    // Check if the transfer is finished (still enabled)
    if (*dma_get_CCRx(_dma) & DMA_CCR__EN)
    {
        canceled = 1;
    }

    // Clear the CCR to stop the channel
    *dma_get_CCRx(_dma) = 0;
    // Clear the interrupt flags
    *dma_get_IFCR(_dma) = (DMA_IFCR__CGIFx | DMA_IFCR__CHTIFx
                           | DMA_IFCR__CTCIFx | DMA_IFCR__CTEIFx) << (_dma->channel
                                   * DMA_IFCR__CHANNEL_OFFSET);

    platform_exit_critical();

    return canceled;
}

void dma_handle_interrupt(const _dma_t *_dma)
{
    uint32_t isr;

    isr = *dma_get_ISR(_dma);

    // Check if the transfer complete interrupt flag is set for this channel
    if (isr & (DMA_ISR__TCIFx << (_dma->channel * DMA_ISR__CHANNEL_OFFSET)))
    {
        // Clear the interrupt flags
        *dma_get_IFCR(_dma) = (DMA_IFCR__CGIFx | DMA_IFCR__CHTIFx
                               | DMA_IFCR__CTCIFx | DMA_IFCR__CTEIFx) << (_dma->channel
                                       * DMA_IFCR__CHANNEL_OFFSET);

        // Disable the DMA channel
        *dma_get_CCRx(_dma) &= ~DMA_CCR__EN;

        // Call the handler if any
        if (_dma->data->handler)
        {
            _dma->data->handler(_dma->data->handler_arg);
        }
    }
}
