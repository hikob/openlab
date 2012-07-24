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
 * usb_registers.h
 *
 *  Created on: Aug 30, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef USB_REGISTERS_H_
#define USB_REGISTERS_H_

#include "memmap.h"

static inline volatile uint32_t *usb_get_EPnR(uint32_t i)
{
    return mem_get_reg32(USB_BASE_ADDRESS + USB_EPnR_OFFSET + 4 * i);
}

static inline volatile uint32_t *usb_get_CNTR()
{
    return mem_get_reg32(USB_BASE_ADDRESS + USB_CNTR_OFFSET);
}

static inline volatile uint32_t *usb_get_ISTR()
{
    return mem_get_reg32(USB_BASE_ADDRESS + USB_ISTR_OFFSET);
}

static inline volatile uint32_t *usb_get_FNR()
{
    return mem_get_reg32(USB_BASE_ADDRESS + USB_FNR_OFFSET);
}

static inline volatile uint32_t *usb_get_DADDR()
{
    return mem_get_reg32(USB_BASE_ADDRESS + USB_DADDR_OFFSET);
}

static inline volatile uint32_t *usb_get_BTABLE()
{
    return mem_get_reg32(USB_BASE_ADDRESS + USB_BTABLE_OFFSET);
}

static inline volatile uint32_t *usb_get_ADDRn_TX(uint32_t i)
{
    return mem_get_reg32(USB_PMA_BASE_ADDRESS + 16 * i + USB_ADDRn_TX_OFFSET);
}

static inline volatile uint32_t *usb_get_COUNTn_TX(uint32_t i)
{
    return mem_get_reg32(USB_PMA_BASE_ADDRESS + 16 * i + USB_COUNTn_TX_OFFSET);
}

static inline volatile uint32_t *usb_get_ADDRn_RX(uint32_t i)
{
    return mem_get_reg32(USB_PMA_BASE_ADDRESS + 16 * i + USB_ADDRn_RX_OFFSET);
}

static inline volatile uint32_t *usb_get_COUNTn_RX(uint32_t i)
{
    return mem_get_reg32(USB_PMA_BASE_ADDRESS + 16 * i + USB_COUNTn_RX_OFFSET);
}

static inline volatile uint16_t *usb_get_packet_memory(uint32_t i)
{
    return mem_get_reg16(USB_PMA_BASE_ADDRESS + 2 * i);
}

enum
{
    USB_EPnR__EA           = 0x000F,
    USB_EPnR__STAT_TX_MASK = 0x0030,
    USB_EPnR__DTOG_TX      = 0x0040,
    USB_EPnR__CTR_TX       = 0x0080,
    USB_EPnR__EP_KIND      = 0x0100,
    USB_EPnR__EP_TYPE_MASK = 0x0600,
    USB_EPnR__SETUP        = 0x0800,
    USB_EPnR__STAT_RX_MASK = 0x3000,
    USB_EPnR__DTOG_RX      = 0x4000,
    USB_EPnR__CTR_RX       = 0x8000,

    USB_EPnR__ALL_TOGGLE   = 0x7070
};

enum
{
    USB_CNTR__FRES    = 0x0001,
    USB_CNTR__PDWN    = 0x0002,
    USB_CNTR__LP_MODE = 0x0004,
    USB_CNTR__FSUSP   = 0x0008,
    USB_CNTR__RESUME  = 0x0010,
    USB_CNTR__ESOFM   = 0x0100,
    USB_CNTR__SOFM    = 0x0200,
    USB_CNTR__RESETM  = 0x0400,
    USB_CNTR__SUSPM   = 0x0800,
    USB_CNTR__WKUPM   = 0x1000,
    USB_CNTR__ERRM    = 0x2000,
    USB_CNTR__PMAOVRM = 0x4000,
    USB_CNTR__CTRM    = 0x8000
};

enum
{
    USB_ISTR__EP_ID_MASK = 0x000F,
    USB_ISTR__DIR        = 0x0010,
    USB_ISTR__ESOF       = 0x0100,
    USB_ISTR__SOF        = 0x0200,
    USB_ISTR__RESET      = 0x0400,
    USB_ISTR__SUSP       = 0x0800,
    USB_ISTR__WKUP       = 0x1000,
    USB_ISTR__ERR        = 0x2000,
    USB_ISTR__PMAOVR     = 0x4000,
    USB_ISTR__CTR        = 0x8000
};

enum
{
    USB_FNR__FN_MASK   = 0x07FF,
    USB_FNR__LSOF_MASK = 0x1800,
    USB_FNR__LCK       = 0x2000,
    USB_FNR__RXDM      = 0x4000,
    USB_FNR__RXDP      = 0x8000
};

enum
{
    USB_DADDR__ADD_MASK = 0x007F,
    USB_DADDR__EF       = 0x0080
};

enum
{
    USB_COUNTn_RX__COUNTn_RX_MASK = 0x03FF,
    USB_COUNTn_RX__NUM_BLOCK_MASK = 0x7C00,
    USB_COUNTn_RX__BLSIZE         = 0x8000
};

static inline void usb_set_stat_tx(uint8_t addr, stat_bits_t stat)
{
    *usb_get_EPnR(addr) = ((*usb_get_EPnR(addr)) & ((~USB_EPnR__ALL_TOGGLE) | USB_EPnR__STAT_TX_MASK)) ^ stat; // STAT_TX = stat
}

static inline void usb_set_stat_rx(uint8_t addr, stat_bits_t stat)
{
    *usb_get_EPnR(addr) = ((*usb_get_EPnR(addr)) & ((~USB_EPnR__ALL_TOGGLE) | USB_EPnR__STAT_RX_MASK)) ^ stat; // STAT_RX = stat
}

static inline void usb_set_dtog_tx(uint8_t addr, uint8_t bit)
{
    *usb_get_EPnR(addr) = ((*usb_get_EPnR(addr)) & ((~USB_EPnR__ALL_TOGGLE) | USB_EPnR__DTOG_TX)) ^(bit ? USB_EPnR__DTOG_TX : 0); // DTOG_RX = bit
}

static inline void usb_set_dtog_rx(uint8_t addr, uint8_t bit)
{
    *usb_get_EPnR(addr) = ((*usb_get_EPnR(addr)) & ((~USB_EPnR__ALL_TOGGLE) | USB_EPnR__DTOG_RX)) ^(bit ? USB_EPnR__DTOG_RX : 0); // DTOG_RX = bit
}

static inline uint16_t usb_get_rx_len(uint8_t addr)
{
    return (*usb_get_COUNTn_RX(addr)) & USB_COUNTn_RX__COUNTn_RX_MASK;
}

#endif
