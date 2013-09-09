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
 * dma_registers.h
 *
 *  Created on: Aug 19, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef DMA_REGISTERS_H_
#define DMA_REGISTERS_H_

#include "memmap.h"
#include "dma.h"

/*
 * Note: although the datasheet specifies the registers may be accessed
 * in 8, 16 or 32bit, it only works in practice with 32bit.
 *
 * It otherwise produces HardFaults!
 */

static inline volatile uint32_t *dma_get_ISR(const _dma_t *_dma)
{
    return mem_get_reg32(_dma->base_address + DMA_ISR_OFFSET);
}

static inline volatile uint32_t *dma_get_IFCR(const _dma_t *_dma)
{
    return mem_get_reg32(_dma->base_address + DMA_IFCR_OFFSET);
}

static inline volatile uint32_t *dma_get_CCRx(const _dma_t *_dma)
{
    return mem_get_reg32(_dma->base_address + DMA_CHANNEL1_OFFSET
                         + (_dma->channel * DMA_CHANNEL_OFFSET) + DMA_CCRx_OFFSET);
}

static inline volatile uint32_t *dma_get_CNDTRx(const _dma_t *_dma)
{
    return mem_get_reg32(_dma->base_address + DMA_CHANNEL1_OFFSET
                         + (_dma->channel * DMA_CHANNEL_OFFSET) + DMA_CNDTRx_OFFSET);
}

static inline volatile uint32_t *dma_get_CPARx(const _dma_t *_dma)
{
    return mem_get_reg32(_dma->base_address + DMA_CHANNEL1_OFFSET
                         + (_dma->channel * DMA_CHANNEL_OFFSET) + DMA_CPARx_OFFSET);
}

static inline volatile uint32_t *dma_get_CMARx(const _dma_t *_dma)
{
    return mem_get_reg32(_dma->base_address + DMA_CHANNEL1_OFFSET
                         + (_dma->channel * DMA_CHANNEL_OFFSET) + DMA_CMARx_OFFSET);
}

enum
{
    DMA_ISR__GIFx = 0x01,
    DMA_ISR__TCIFx = 0x02,
    DMA_ISR__HTIFx = 0x04,
    DMA_ISR__TEIFx = 0x08,

    DMA_ISR__CHANNEL_OFFSET = 4
};

enum
{
    DMA_IFCR__CGIFx = 0x01,
    DMA_IFCR__CTCIFx = 0x02,
    DMA_IFCR__CHTIFx = 0x04,
    DMA_IFCR__CTEIFx = 0x08,

    DMA_IFCR__CHANNEL_OFFSET = 4
};

enum
{
    DMA_CCR__EN = 0x0001,
    DMA_CCR__TCIE = 0x0002,
    DMA_CCR__HTIE = 0x0004,
    DMA_CCR__TEIE = 0x0008,
    DMA_CCR__DIR = 0x0010,
    DMA_CCR__CIRC = 0x0020,
    DMA_CCR__PINC = 0x0040,
    DMA_CCR__MINC = 0x0080,
    DMA_CCR__PSIZE_MASK = 0x0300,
    DMA_CCR__PSIZE_8BIT = 0x0000,
    DMA_CCR__PSIZE_16BIT = 0x0100,
    DMA_CCR__PSIZE_32BIT = 0x0200,
    DMA_CCR__MSIZE_MASK = 0x0C00,
    DMA_CCR__MSIZE_8BIT = 0x0000,
    DMA_CCR__MSIZE_16BIT = 0x0400,
    DMA_CCR__MSIZE_32BIT = 0x0800,
    DMA_CCR__PL_MASK = 0x3000,
    DMA_CCR__PL_LOW = 0x0000,
    DMA_CCR__PL_MEDIUM = 0x1000,
    DMA_CCR__PL_HIGH = 0x2000,
    DMA_CCR__PL_VERY_HIGH = 0x3000,
    DMA_CCR__MEM2MEM = 0x4000,
};

#endif /* DMA_REGISTERS_H_ */
