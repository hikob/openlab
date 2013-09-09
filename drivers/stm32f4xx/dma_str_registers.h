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
 * dma_registers.h
 *
 *  Created on: Nov 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef DMA_REGISTERS_H_
#define DMA_REGISTERS_H_

#include "memmap.h"
#include "dma_str.h"

/*
 * Registers must be accessed in 32bit
 */

static inline volatile uint32_t *dma_get_LISR(const _dma_t *_dma)
{
    return mem_get_reg32(_dma->base_address + DMAx_LISR_OFFSET);
}
static inline volatile uint32_t *dma_get_HISR(const _dma_t *_dma)
{
    return mem_get_reg32(_dma->base_address + DMAx_HISR_OFFSET);
}

static inline volatile uint32_t *dma_get_LIFCR(const _dma_t *_dma)
{
    return mem_get_reg32(_dma->base_address + DMAx_LIFCR_OFFSET);
}
static inline volatile uint32_t *dma_get_HIFCR(const _dma_t *_dma)
{
    return mem_get_reg32(_dma->base_address + DMAx_HIFCR_OFFSET);
}

static inline volatile uint32_t *dma_get_SxCR(const _dma_t *_dma)
{
    return mem_get_reg32(_dma->base_address + DMAx_S0CR_OFFSET + _dma->stream
            * DMAx_STREAM_OFFSET);
}
static inline volatile uint32_t *dma_get_SxNDTR(const _dma_t *_dma)
{
    return mem_get_reg32(_dma->base_address + DMAx_S0NDTR_OFFSET + _dma->stream
            * DMAx_STREAM_OFFSET);
}
static inline volatile uint32_t *dma_get_SxPAR(const _dma_t *_dma)
{
    return mem_get_reg32(_dma->base_address + DMAx_S0PAR_OFFSET + _dma->stream
            * DMAx_STREAM_OFFSET);
}
static inline volatile uint32_t *dma_get_SxM0AR(const _dma_t *_dma)
{
    return mem_get_reg32(_dma->base_address + DMAx_S0M0AR_OFFSET + _dma->stream
            * DMAx_STREAM_OFFSET);
}
static inline volatile uint32_t *dma_get_SxM1AR(const _dma_t *_dma)
{
    return mem_get_reg32(_dma->base_address + DMAx_S0M1AR_OFFSET + _dma->stream
            * DMAx_STREAM_OFFSET);
}
static inline volatile uint32_t *dma_get_SxFCR(const _dma_t *_dma)
{
    return mem_get_reg32(_dma->base_address + DMAx_S0FCR_OFFSET + _dma->stream
            * DMAx_STREAM_OFFSET);
}

enum
{
    DMA_LISR__FEIF0 = 0x0001,
    DMA_LISR__DMEIF0 = 0x0004,
    DMA_LISR__TEIF0 = 0x0008,
    DMA_LISR__HTIF0 = 0x0010,
    DMA_LISR__TCIF0 = 0x0020,
};

enum
{
    DMA_LIFCR__CFEIF0 = 0x0001,
    DMA_LIFCR__CDMEIF0 = 0x0004,
    DMA_LIFCR__CTEIF0 = 0x0008,
    DMA_LIFCR__CHTIF0 = 0x0010,
    DMA_LIFCR__CTCIF0 = 0x0020,
};

enum
{
    DMA_SxCR__EN = 0x00000001,
    DMA_SxCR__DMEIE = 0x00000002,
    DMA_SxCR__TEIE = 0x00000004,
    DMA_SxCR__HTIE = 0x00000008,
    DMA_SxCR__TCIE = 0x00000010,
    DMA_SxCR__PFCTRL = 0x00000020,
    DMA_SxCR__DIR_MASK = 0x000000C0,
    DMA_SxCR__CIRC = 0x00000100,
    DMA_SxCR__PINC = 0x00000200,
    DMA_SxCR__MINC = 0x00000400,
    DMA_SxCR__PSIZE_MASK = 0x00001800,
    DMA_SxCR__MSIZE_MASK = 0x00006000,
    DMA_SxCR__PINCOS = 0x00008000,

    DMA_SxCR__PL_MASK = 0x00030000,
    DMA_SxCR__PL_LOW = 0x00000000,
    DMA_SxCR__PL_MEDIUM = 0x00010000,
    DMA_SxCR__PL_HIGH = 0x00020000,
    DMA_SxCR__PL_VERY_HIGH = 0x00030000,

    DMA_SxCR__CT = 0x00080000,
    DMA_SxCR__PBURST_MASK = 0x00600000,
    DMA_SxCR__MBURST_MASK = 0x01800000,
    DMA_SxCR__CHSEL_MASK = 0x0E000000,
};

enum
{
    DMA_SxFCR__FTH_MASK = 0x03,
    DMA_SxFCR__DMDIS = 0x04,
    DMA_SxFCR__FS_MASK = 0x38,
    DMA_SxFCR__FEIE = 0x80,
};

#endif /* DMA_REGISTERS_H_ */
