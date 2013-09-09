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
 * ethmac_registers.h
 *
 *  Created on: Sep 20, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef ETHMAC_REGISTERS_H_
#define ETHMAC_REGISTERS_H_

#include "memmap.h"

static inline volatile uint32_t* ethmac_get_DMABMR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_DMABMR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_DMATPDR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_DMATPDR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_DMARPDR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_DMARPDR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_DMARDLAR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_DMARDLAR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_DMATDLAR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_DMATDLAR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_DMASR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_DMASR_OFFSET);
}

static inline volatile uint32_t* ethmac_get_DMAOMR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_DMAOMR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_DMAIER()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_DMAIER_OFFSET);
}
static inline volatile uint32_t* ethmac_get_DMAMFBOCR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_DMAMFBOCR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_DMARSWTR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_DMARSWTR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_DMACHTDR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_DMACHTDR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_DMACHRDR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_DMACHRDR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_DMACHTBAR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_DMACHTBAR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_DMACHRBAR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_DMACHRBAR_OFFSET);
}

static inline volatile uint32_t* ethmac_get_MACCR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACCR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_MACFFR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACFFR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_MACHTHR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACHTHR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_MACHTLR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACHTLR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_MACMIIAR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACMIIAR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_MACMIIDR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACMIIDR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_MACFCR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACFCR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_MACVLANTR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACVLANTR_OFFSET);
}

static inline volatile uint32_t* ethmac_get_MACA0HR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACA0HR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_MACA0LR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACA0LR_OFFSET);
}

static inline volatile uint32_t* ethmac_get_MACA1HR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACA1HR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_MACA1LR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACA1LR_OFFSET);
}

static inline volatile uint32_t* ethmac_get_MACA2HR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACA2HR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_MACA2LR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACA2LR_OFFSET);
}

static inline volatile uint32_t* ethmac_get_MACA3HR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACA3HR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_MACA3LR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACA3LR_OFFSET);
}

static inline volatile uint32_t* ethmac_get_MACDBGR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACDBGR_OFFSET);
}
static inline volatile uint32_t* ethmac_get_MACSR()
{
    return mem_get_reg32(ETHMAC_BASE_ADDRESS + ETHMAC_MACSR_OFFSET);
}

enum
{
    ETHMAC_MACMIIAR__PA_MASK = 0xF800,
    ETHMAC_MACMIIAR__PA_SHIFT = 11,

    ETHMAC_MACMIIAR__MR_MASK = 0x07C0,
    ETHMAC_MACMIIAR__MR_SHIFT = 6,

    ETHMAC_MACMIIAR__CR_MASK = 0x001C,
    ETHMAC_MACMIIAR__CR_Div16 = 0x0008,
    ETHMAC_MACMIIAR__CR_Div26 = 0x000C,
    ETHMAC_MACMIIAR__CR_Div42 = 0x0000,
    ETHMAC_MACMIIAR__CR_Div62 = 0x0004,
    ETHMAC_MACMIIAR__CR_SHIFT = 2,

    ETHMAC_MACMIIAR__MW = 0x0002,
    ETHMAC_MACMIIAR__MB = 0x0001,

};

enum
{
    ETHMAC_DMABMR__MB = 0x04000000,
    ETHMAC_DMABMR__AAB = 0x02000000,
    ETHMAC_DMABMR__FPM = 0x01000000,
    ETHMAC_DMABMR__USP = 0x00800000,

    ETHMAC_DMABMR__RDP_MASK = 0x007E0000,
    ETHMAC_DMABMR__RDP_1Beat = (1 << 17),
    ETHMAC_DMABMR__RDP_2Beat = (2 << 17),
    ETHMAC_DMABMR__RDP_4Beat = (4 << 17),
    ETHMAC_DMABMR__RDP_8Beat = (8 << 17),
    ETHMAC_DMABMR__RDP_16Beat = (16 << 17),
    ETHMAC_DMABMR__RDP_32Beat = (32 << 17),

    ETHMAC_DMABMR__FB = 0x00010000,
    ETHMAC_DMABMR__RTPR_MASK = 0x0000C000,

    ETHMAC_DMABMR__PBL_MASK = 0x00003F00,
    ETHMAC_DMABMR__PBL_1Beat = (1 << 8),
    ETHMAC_DMABMR__PBL_2Beat = (2 << 8),
    ETHMAC_DMABMR__PBL_4Beat = (4 << 8),
    ETHMAC_DMABMR__PBL_8Beat = (8 << 8),
    ETHMAC_DMABMR__PBL_16Beat = (16 << 8),
    ETHMAC_DMABMR__PBL_32Beat = (32 << 8),

    ETHMAC_DMABMR__EDFE = 0x00000080,
    ETHMAC_DMABMR__DSL_MASK = 0x0000007C,
    ETHMAC_DMABMR__DA = 0x00000002,
    ETHMAC_DMABMR__SR = 0x00000001,
};

enum
{
    ETHMAC_DMAIER__NISE = 0x00010000,
    ETHMAC_DMAIER__AISE = 0x00008000,
    ETHMAC_DMAIER__ERIE = 0x00004000,
    ETHMAC_DMAIER__FBEIE = 0x00002000,
    ETHMAC_DMAIER__ETIE = 0x00000400,
    ETHMAC_DMAIER__RWTIE = 0x00000200,
    ETHMAC_DMAIER__RPSIE = 0x00000100,
    ETHMAC_DMAIER__RBUIE = 0x00000080,
    ETHMAC_DMAIER__RIE = 0x00000040,
    ETHMAC_DMAIER__TUIE = 0x00000020,
    ETHMAC_DMAIER__ROIE = 0x00000010,
    ETHMAC_DMAIER__TJTIE = 0x00000008,
    ETHMAC_DMAIER__TBUIE = 0x00000004,
    ETHMAC_DMAIER__TPSIE = 0x00000002,
    ETHMAC_DMAIER__TIE = 0x00000001,
};

enum
{
    ETHMAC_DMAOMR__DTCEFD = 0x04000000,
    ETHMAC_DMAOMR__RSF = 0x02000000,
    ETHMAC_DMAOMR__DFRF = 0x01000000,
    ETHMAC_DMAOMR__TSF = 0x00200000,
    ETHMAC_DMAOMR__FTF = 0x00100000,
    ETHMAC_DMAOMR__TTC = 0x0008C000,
    ETHMAC_DMAOMR__ST = 0x00002000,
    ETHMAC_DMAOMR__FEF = 0x00000080,
    ETHMAC_DMAOMR__FUGF = 0x00000040,
    ETHMAC_DMAOMR__RTC = 0x00000018,
    ETHMAC_DMAOMR__OSF = 0x00000004,
    ETHMAC_DMAOMR__SR = 0x00000002,
};

enum
{
    ETHMAC_DMASR__TSTS = 0x20000000,
    ETHMAC_DMASR__PMTS = 0x10000000,
    ETHMAC_DMASR__MMCS = 0x08000000,
    ETHMAC_DMASR__EBS_MASK = 0x03800000,

    ETHMAC_DMASR__TPS_MASK = 0x00700000,
    ETHMAC_DMASR__TPS_STOPPED = 0x00000000,
    ETHMAC_DMASR__TPS_RUNNING_FETCHING = 0x00100000,
    ETHMAC_DMASR__TPS_RUNNING_WAITING = 0x00200000,
    ETHMAC_DMASR__TPS_RUNNING_READING = 0x00300000,
    ETHMAC_DMASR__TPS_SUSPENDED = 0x00600000,
    ETHMAC_DMASR__TPS_RUNNING_CLOSING = 0x00700000,

    ETHMAC_DMASR__RPS_MASK = 0x000E0000,
    ETHMAC_DMASR__RPS_STOPPED = 0x00000000,
    ETHMAC_DMASR__RPS_RUNNING_FETCHING = 0x00020000,
    ETHMAC_DMASR__RPS_RUNNING_WAITING = 0x00060000,
    ETHMAC_DMASR__RPS_SUSPENDED = 0x00080000,
    ETHMAC_DMASR__RPS_RUNNING_CLOSING = 0x000A0000,
    ETHMAC_DMASR__RPS_RUNNING_TRANSFERRING = 0x000E0000,

    ETHMAC_DMASR__NIS = 0x00010000,
    ETHMAC_DMASR__AIS = 0x00008000,
    ETHMAC_DMASR__ERS = 0x00004000,
    ETHMAC_DMASR__FBES = 0x00002000,
    ETHMAC_DMASR__ETS = 0x00000400,
    ETHMAC_DMASR__RWTS = 0x00000200,
    ETHMAC_DMASR__RPSS = 0x00000100,
    ETHMAC_DMASR__RBUS = 0x00000080,
    ETHMAC_DMASR__RS = 0x00000040,
    ETHMAC_DMASR__TUS = 0x00000020,
    ETHMAC_DMASR__ROS = 0x00000010,
    ETHMAC_DMASR__TJTS = 0x00000008,
    ETHMAC_DMASR__TBUS = 0x00000004,
    ETHMAC_DMASR__TPSS = 0x00000002,
    ETHMAC_DMASR__TS = 0x00000001,
};
enum
{
    ETHMAC_MACCR__CSTF = 0x02000000,
    ETHMAC_MACCR__WD = 0x00800000,
    ETHMAC_MACCR__JD = 0x00400000,
    ETHMAC_MACCR__IFG_MASK = 0x000E0000,
    ETHMAC_MACCR__CSD = 0x00010000,
    ETHMAC_MACCR__FES = 0x00004000,
    ETHMAC_MACCR__ROD = 0x00002000,
    ETHMAC_MACCR__LM = 0x00001000,
    ETHMAC_MACCR__DM = 0x00000800,
    ETHMAC_MACCR__IPCO = 0x00000400,
    ETHMAC_MACCR__RD = 0x00000200,
    ETHMAC_MACCR__APCS = 0x00000080,
    ETHMAC_MACCR__BL_MASK = 0x00000060,
    ETHMAC_MACCR__DC = 0x00000010,
    ETHMAC_MACCR__TE = 0x00000008,
    ETHMAC_MACCR__RE = 0x00000004,
};
#endif /* ETHMAC_REGISTERS_H_ */
