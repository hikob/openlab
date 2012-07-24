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
 * sdio_registers.h
 *
 *  Created on: Jul 26, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef SDIO_REGISTERS_H_
#define SDIO_REGISTERS_H_

static inline volatile uint32_t *sdio_get_POWER()
{
    return mem_get_reg32(SDIO_BASE_ADDRESS + SDIO_POWER_OFFSET);
}

static inline volatile uint32_t *sdio_get_CLKCR()
{
    return mem_get_reg32(SDIO_BASE_ADDRESS + SDIO_CLKCR_OFFSET);
}

static inline volatile uint32_t *sdio_get_ARG()
{
    return mem_get_reg32(SDIO_BASE_ADDRESS + SDIO_ARG_OFFSET);
}

static inline volatile uint32_t *sdio_get_CMD()
{
    return mem_get_reg32(SDIO_BASE_ADDRESS + SDIO_CMD_OFFSET);
}

static inline volatile uint32_t *sdio_get_RESP1()
{
    return mem_get_reg32(SDIO_BASE_ADDRESS + SDIO_RESP1_OFFSET);
}

static inline volatile uint32_t *sdio_get_RESP2()
{
    return mem_get_reg32(SDIO_BASE_ADDRESS + SDIO_RESP2_OFFSET);
}

static inline volatile uint32_t *sdio_get_RESP3()
{
    return mem_get_reg32(SDIO_BASE_ADDRESS + SDIO_RESP3_OFFSET);
}

static inline volatile uint32_t *sdio_get_RESP4()
{
    return mem_get_reg32(SDIO_BASE_ADDRESS + SDIO_RESP4_OFFSET);
}

static inline volatile uint32_t *sdio_get_DTIMER()
{
    return mem_get_reg32(SDIO_BASE_ADDRESS + SDIO_DTIMER_OFFSET);
}

static inline volatile uint32_t *sdio_get_DLEN()
{
    return mem_get_reg32(SDIO_BASE_ADDRESS + SDIO_DLEN_OFFSET);
}

static inline volatile uint32_t *sdio_get_DCTRL()
{
    return mem_get_reg32(SDIO_BASE_ADDRESS + SDIO_DCTRL_OFFSET);
}

static inline volatile uint32_t *sdio_get_STA()
{
    return mem_get_reg32(SDIO_BASE_ADDRESS + SDIO_STA_OFFSET);
}

static inline volatile uint32_t *sdio_get_ICR()
{
    return mem_get_reg32(SDIO_BASE_ADDRESS + SDIO_ICR_OFFSET);
}

static inline volatile uint32_t *sdio_get_MASK()
{
    return mem_get_reg32(SDIO_BASE_ADDRESS + SDIO_MASK_OFFSET);
}

static inline volatile uint32_t *sdio_get_FIFO()
{
    return mem_get_reg32(SDIO_BASE_ADDRESS + SDIO_FIFO_OFFSET);
}

enum
{
    SDIO_POWER__PWRCTRL_MASK = 0x3
};

enum
{
    SDIO_CLKCR__CLKDIV_MASK = 0x00FF,
    SDIO_CLKCR__CLKEN       = 0x0100,
    SDIO_CLKCR__PWRSAV      = 0x0200,
    SDIO_CLKCR__BYPASS      = 0x0400,
    SDIO_CLKCR__WIDBUS_MASK = 0x1800,
    SDIO_CLKCR__NEGEDGE     = 0x2000,
    SDIO_CLKCR__HWFC_EN     = 0x4000,

    SDIO_CLKCR__WIDBUS_4    = 0x0800
};

enum
{
    SDIO_CMD__CMDINDEX_MASK = 0x003F,
    SDIO_CMD__WAITRESP_MASK = 0x00C0,
    SDIO_CMD__WAITINT       = 0x0100,
    SDIO_CMD__WAITPEND      = 0x0200,
    SDIO_CMD__CPSMEN        = 0x0400,
    SDIO_CMD__SDIOSUSPEND   = 0x0800,
    SDIO_CMD__ENCMDCOMPL    = 0x1000,
    SDIO_CMD__NIEN          = 0x2000,
    SDIO_CMD__CE_ATACMD     = 0x4000
};

enum
{
    SDIO_DCTRL__DTEN             = 0x0001,
    SDIO_DCTRL__DTDIR            = 0x0002,
    SDIO_DCTRL__DTMODE           = 0x0004,
    SDIO_DCTRL__DMAEN            = 0x0008,
    SDIO_DCTRL__DBLOCKSIZE_MASK  = 0x00F0,
    SDIO_DCTRL__RWSTART          = 0x0100,
    SDIO_DCTRL__RWSTOP           = 0x0200,
    SDIO_DCTRL__RWMOD            = 0x0400,
    SDIO_DCTRL__SDIOEN           = 0x0800,

    SDIO_DCTRL__DBLOCKSIZE_1     = 0x0000,
    SDIO_DCTRL__DBLOCKSIZE_2     = 0x0010,
    SDIO_DCTRL__DBLOCKSIZE_4     = 0x0020,
    SDIO_DCTRL__DBLOCKSIZE_8     = 0x0030,
    SDIO_DCTRL__DBLOCKSIZE_16    = 0x0040,
    SDIO_DCTRL__DBLOCKSIZE_32    = 0x0050,
    SDIO_DCTRL__DBLOCKSIZE_64    = 0x0060,
    SDIO_DCTRL__DBLOCKSIZE_128   = 0x0070,
    SDIO_DCTRL__DBLOCKSIZE_256   = 0x0080,
    SDIO_DCTRL__DBLOCKSIZE_512   = 0x0090,
    SDIO_DCTRL__DBLOCKSIZE_1024  = 0x00A0,
    SDIO_DCTRL__DBLOCKSIZE_2048  = 0x00B0,
    SDIO_DCTRL__DBLOCKSIZE_4096  = 0x00C0,
    SDIO_DCTRL__DBLOCKSIZE_8192  = 0x00D0,
    SDIO_DCTRL__DBLOCKSIZE_16384 = 0x00E0
};

enum
{
    SDIO_STA__CCRCFAIL = 0x00000001,
    SDIO_STA__DCRCFAIL = 0x00000002,
    SDIO_STA__CTIMEOUT = 0x00000004,
    SDIO_STA__DTIMEOUT = 0x00000008,
    SDIO_STA__TXUNDERR = 0x00000010,
    SDIO_STA__RXOVERR  = 0x00000020,
    SDIO_STA__CMDREND  = 0x00000040,
    SDIO_STA__CMDSENT  = 0x00000080,
    SDIO_STA__DATAEND  = 0x00000100,
    SDIO_STA__STBITERR = 0x00000200,
    SDIO_STA__DBCKEND  = 0x00000400,
    SDIO_STA__CMDACT   = 0x00000800,
    SDIO_STA__TXACT    = 0x00001000,
    SDIO_STA__RXACT    = 0x00002000,
    SDIO_STA__TXFIFOHE = 0x00004000,
    SDIO_STA__RXFIFOHF = 0x00008000,
    SDIO_STA__TXFIFOF  = 0x00010000,
    SDIO_STA__RXFIFOF  = 0x00020000,
    SDIO_STA__TXFIFOE  = 0x00040000,
    SDIO_STA__RXFIFOE  = 0x00080000,
    SDIO_STA__TXDAVL   = 0x00100000,
    SDIO_STA__RXDAVL   = 0x00200000,
    SDIO_STA__SDIOIT   = 0x00400000,
    SDIO_STA__CEATAEND = 0x00800000
};

enum
{
    SDIO_ICR__CCRCFAILC = 0x00000001,
    SDIO_ICR__DCRCFAILC = 0x00000002,
    SDIO_ICR__CTIMEOUTC = 0x00000004,
    SDIO_ICR__DTIMEOUTC = 0x00000008,
    SDIO_ICR__TXUNDERRC = 0x00000010,
    SDIO_ICR__RXOVERRC  = 0x00000020,
    SDIO_ICR__CMDRENDC  = 0x00000040,
    SDIO_ICR__CMDSENTC  = 0x00000080,
    SDIO_ICR__DATAENDC  = 0x00000100,
    SDIO_ICR__STBITERRC = 0x00000200,
    SDIO_ICR__DBCKENDC  = 0x00000400,
    SDIO_ICR__SDIOITC   = 0x00400000,
    SDIO_ICR__CEATAENDC = 0x00800000,

    SDIO_ICR__ALL       = 0x000005FF
};

enum
{
    SDIO_MASK__CCRCFAILIE = 0x00000001,
    SDIO_MASK__DCRCFAILIE = 0x00000002,
    SDIO_MASK__CTIMEOUTIE = 0x00000004,
    SDIO_MASK__DTIMEOUTIE = 0x00000008,
    SDIO_MASK__TXUNDERRIE = 0x00000010,
    SDIO_MASK__RXOVERRIE  = 0x00000020,
    SDIO_MASK__CMDRENDIE  = 0x00000040,
    SDIO_MASK__CMDSENTIE  = 0x00000080,
    SDIO_MASK__DATAENDIE  = 0x00000100,
    SDIO_MASK__STBITERRIE = 0x00000200,
    SDIO_MASK__DBCKENDIE  = 0x00000400,
    SDIO_MASK__CMDACTIE   = 0x00000800,
    SDIO_MASK__TXACTIE    = 0x00001000,
    SDIO_MASK__RXACTIE    = 0x00002000,
    SDIO_MASK__TXFIFOHEIE = 0x00004000,
    SDIO_MASK__RXFIFOHFIE = 0x00008000,
    SDIO_MASK__TXFIFOFIE  = 0x00010000,
    SDIO_MASK__RXFIFOFIE  = 0x00020000,
    SDIO_MASK__TXFIFOEIE  = 0x00040000,
    SDIO_MASK__RXFIFOEIE  = 0x00080000,
    SDIO_MASK__TXDAVLIE   = 0x00100000,
    SDIO_MASK__RXDAVLIE   = 0x00200000,
    SDIO_MASK__SDIOITIE   = 0x00400000,
    SDIO_MASK__CEATAENDIE = 0x00800000
};

#endif
