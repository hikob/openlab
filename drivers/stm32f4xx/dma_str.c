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
 * dma.c
 *
 *  Created on: Aug 19, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>

#include "platform.h"

#include "dma.h"
#include "dma_str.h"
#include "dma_str_registers.h"
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

void dma_config(dma_t dma, uint32_t peripheral_address,
        uint32_t memory_address, uint16_t transfer_number,
        dma_size_t transfer_size, dma_direction_t direction,
        dma_increment_t memory_increment)
{
    const _dma_t *_dma = dma;

    // Disable the DMA channel
    *dma_get_SxCR(_dma) = 0;

    // Compute the IFCR register and bit offset
    volatile uint32_t* ifcr = _dma->stream > 3 ? dma_get_HIFCR(_dma)
            : dma_get_LIFCR(_dma);
    uint32_t ifcr_offset = 6 * ((_dma->stream & 0x1) != 0) + 16
            * ((_dma->stream & 0x2) != 0);

    // Clear all flags
    *ifcr = (DMA_LIFCR__CFEIF0 | DMA_LIFCR__CDMEIF0 | DMA_LIFCR__CTEIF0
            | DMA_LIFCR__CHTIF0 | DMA_LIFCR__CTCIF0) << ifcr_offset;

    // Configure the channel, with memory increment if required,
    // peripheral no increment
    // Do not enable yet the transfer complete interrupt
    *dma_get_SxCR(_dma) = (_dma->data->trigger_channel << 25) | DMA_SxCR__PL_MEDIUM
            | (transfer_size << 13) | (transfer_size << 11) | (memory_increment
            << 10) | (direction << 6);

    // Store the transfer number
    *dma_get_SxNDTR(_dma) = transfer_number;

    // Store the memory and peripheral address
    *dma_get_SxM0AR(_dma) = memory_address;
    *dma_get_SxPAR(_dma) = peripheral_address;
}

void dma_start(dma_t dma, handler_t handler, handler_arg_t handler_arg)
{
    const _dma_t *_dma = dma;

    // Store the handlers
    _dma->data->handler = handler;
    _dma->data->handler_arg = handler_arg;

    // Enable the transfer complete interrupt
    *dma_get_SxCR(_dma) |= DMA_SxCR__TCIE;

    // Set the EN bit to start channel
    *dma_get_SxCR(_dma) |= DMA_SxCR__EN;
}

int32_t dma_cancel(dma_t dma)
{
    const _dma_t *_dma = dma;

    int32_t canceled = 0;

    // Disable interrupts
    platform_enter_critical();

    // Check if the transfer is finished (still enabled)
    if (*dma_get_SxCR(_dma) &= ~DMA_SxCR__EN)
    {
        canceled = 1;
    }

    // Compute the ISR register and bit offset
    uint32_t isr_offset = 6 * ((_dma->stream & 0x1) != 0) + 16 * ((_dma->stream
            & 0x2) != 0);

    // Get the IFCR register
    volatile uint32_t* ifcr = _dma->stream > 3 ? dma_get_HIFCR(_dma)
            : dma_get_LIFCR(_dma);
    // Clear the interrupt flag
    *ifcr = (DMA_LIFCR__CFEIF0 | DMA_LIFCR__CDMEIF0 | DMA_LIFCR__CTEIF0
            | DMA_LIFCR__CHTIF0 | DMA_LIFCR__CTCIF0) << isr_offset;

    // Disable the DMA channel
    *dma_get_SxCR(_dma) &= ~DMA_SxCR__EN;

    // Enable back
    platform_exit_critical();

    return canceled;
}
void dma_handle_interrupt(dma_t dma)
{
    const _dma_t *_dma = dma;

    // Compute the ISR register and bit offset
    volatile uint32_t* isr = _dma->stream > 3 ? dma_get_HISR(_dma)
            : dma_get_LISR(_dma);
    uint32_t isr_offset = 6 * ((_dma->stream & 0x1) != 0) + 16 * ((_dma->stream
            & 0x2) != 0);

    // Check if the transfer complete interrupt flag is set for this channel
    if (*isr & (DMA_LISR__TCIF0 << isr_offset))
    {
        // Get the IFCR register
        volatile uint32_t* ifcr = _dma->stream > 3 ? dma_get_HIFCR(_dma)
                : dma_get_LIFCR(_dma);
        // Clear the interrupt flag
        *ifcr = (DMA_LIFCR__CFEIF0 | DMA_LIFCR__CDMEIF0 | DMA_LIFCR__CTEIF0
                | DMA_LIFCR__CHTIF0 | DMA_LIFCR__CTCIF0) << isr_offset;

        // Disable the DMA channel
        *dma_get_SxCR(_dma) &= ~DMA_SxCR__EN;

        // Call the handler if any
        if (_dma->data->handler)
        {
            _dma->data->handler(_dma->data->handler_arg);
        }
    }
}
