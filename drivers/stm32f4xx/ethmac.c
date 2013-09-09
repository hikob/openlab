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
 * ethmac.c
 *
 *  Created on: Sep 20, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include <string.h>

#include "ethmac_.h"
#include "ethmac_registers.h"
#include "ethmac_descriptors.h"
#include "ethmac_mii.h"

#include "rcc.h"
#include "syscfg.h"
#include "nvic_.h"

#include "printf.h"
#include "debug.h"

/** Single write */
static void mii_write(uint32_t reg, uint32_t value);
/** Single read */
static uint32_t mii_read(uint32_t reg);
/** Try to find the PHY address */
static void mii_find_phy_addr();

/** Set the address in memory */
static void set_mac_address(const uint8_t *a);

static struct
{
    uint32_t mii_cr;
    uint32_t phy_addr;

    int always_on;

    uint8_t mac_address[6];
} mac;

void ethmac_init(ethmac_mode_t mode, int always_on)
{
    // Initialize the descriptors
    ethmac_descriptors_init();

    // Configure the desired mode
    if (mode == ETHMAC_MODE_MII)
    {
        // MII mode
        syscfg_pmc_config(SYSCFG_PMC__MII_RMII_SEL, 0);
    }
    else
    {
        // RMII mode
        syscfg_pmc_config(SYSCFG_PMC__MII_RMII_SEL, 1);
    }

    // Set default PHY address
    mac.phy_addr = 0;

    // Set the always_on value
    mac.always_on = always_on;

    // Compute MII divider
    switch (rcc_sysclk_get_clock_frequency(RCC_SYSCLK_CLOCK_HCLK))
    {
        case 20000000 ... 35000000:
            mac.mii_cr = ETHMAC_MACMIIAR__CR_Div16;
            break;
        case 35000001 ... 60000000:
            mac.mii_cr = ETHMAC_MACMIIAR__CR_Div26;
            break;
        case 60000001 ... 100000000:
            mac.mii_cr = ETHMAC_MACMIIAR__CR_Div42;
            break;
        case 100000001 ... 168000000:
            mac.mii_cr = ETHMAC_MACMIIAR__CR_Div42;
            break;
    }
}

void ethmac_enable()
{
    // Enable the clocks
    rcc_ahb_enable(RCC_AHB_BIT_ETHMAC);
    rcc_ahb_enable(RCC_AHB_BIT_ETHMACTX);
    rcc_ahb_enable(RCC_AHB_BIT_ETHMACRX);

    // Configure MII
    uint32_t tmp = *ethmac_get_MACMIIAR();
    tmp &= ~ETHMAC_MACMIIAR__CR_MASK;
    tmp |= mac.mii_cr;
    *ethmac_get_MACMIIAR() = tmp;

    // Find the PHY address
    mii_find_phy_addr();

    // Reset the PHY
    log_debug("Resetting PHY... (was %04x)", mii_read(MII_BMCR));
    mii_write(MII_BMCR, MII_BMCR__RESET);
    while (mii_read(MII_BMCR) & MII_BMCR__RESET)
        ;
    log_debug("\tOK (now %04x)", mii_read(MII_BMCR));
}

void ethmac_disable()
{
    // Disable the clocks
    rcc_ahb_disable(RCC_AHB_BIT_ETHMAC);
    rcc_ahb_disable(RCC_AHB_BIT_ETHMACTX);
    rcc_ahb_disable(RCC_AHB_BIT_ETHMACRX);
}

void ethmac_start(const uint8_t* address)
{
    // Copy MAC address
    int i;
    for (i = 0; i < 6; i++)
    {
        mac.mac_address[i] = address[i];
    }

    /* MAC clocks activation and commanded reset procedure.*/
    *ethmac_get_DMABMR() |= ETHMAC_DMABMR__SR;
    while (*ethmac_get_DMABMR() & ETHMAC_DMABMR__SR)
        ;

    // Reset the descriptors
    ethmac_descriptors_reset();

    /* ISR vector enabled.*/
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_ETH);

    /* PHY in power up mode.*/
    mii_write(MII_BMCR, mii_read(MII_BMCR) & ~MII_BMCR__PDOWN);

    /* MAC configuration.*/
    *ethmac_get_MACFFR() = 0;
    *ethmac_get_MACFCR() = 0;
    *ethmac_get_MACVLANTR() = 0;

    /* MAC address setup.*/
    set_mac_address(mac.mac_address);

    /* Transmitter and receiver enabled.
     Note that the complete setup of the MAC is performed when the link
     status is detected.*/
    // No checksum offload
    *ethmac_get_MACCR() = ETHMAC_MACCR__RE | ETHMAC_MACCR__TE;

    /* DMA configuration:
     Descriptor chains pointers.*/
    *ethmac_get_DMARDLAR() = (uint32_t) ethmac_rx_des;
    *ethmac_get_DMATDLAR() = (uint32_t) ethmac_tx_des;

    /* Enabling required interrupt sources.*/
    *ethmac_get_DMASR() = (uint32_t) (*ethmac_get_DMASR());
    *ethmac_get_DMAIER() = ETHMAC_DMAIER__NISE | ETHMAC_DMAIER__AISE
            | ETHMAC_DMAIER__RIE | ETHMAC_DMAIER__TIE;

    /* DMA general settings.*/
    *ethmac_get_DMABMR() = ETHMAC_DMABMR__AAB | ETHMAC_DMABMR__RDP_1Beat
            | ETHMAC_DMABMR__PBL_1Beat;

    /* Transmit FIFO flush.*/
    *ethmac_get_DMAOMR() = ETHMAC_DMAOMR__FTF;
    while (*ethmac_get_DMAOMR() & ETHMAC_DMAOMR__FTF)
        ;

    /* DMA final configuration and start.*/
    *ethmac_get_DMAOMR() = ETHMAC_DMAOMR__DTCEFD | ETHMAC_DMAOMR__RSF
            | ETHMAC_DMAOMR__TSF | ETHMAC_DMAOMR__ST | ETHMAC_DMAOMR__SR;
}

int ethmac_get_link_status()
{
    uint32_t maccr, bmsr, bmcr;

    maccr = *ethmac_get_MACCR();

    if (mac.always_on)
    {
        maccr |= ETHMAC_MACCR__FES;
        maccr |= ETHMAC_MACCR__DM;
        /* Changes the mode in the MAC.*/
        *ethmac_get_MACCR() = maccr;
        return 1;
    }

    /* PHY CR and SR registers read.*/
    bmsr = mii_read(MII_BMSR);
    bmcr = mii_read(MII_BMCR);

    /* Check on auto-negotiation mode.*/
    if (bmcr & MII_BMCR__ANENABLE)
    {
        uint32_t lpa;

        /* Auto-negotiation must be finished without faults and link established.*/
        if ((bmsr
                & (MII_BMSR__LSTATUS | MII_BMSR__RFAULT | MII_BMSR__ANEGCOMPLETE))
                != (MII_BMSR__LSTATUS | MII_BMSR__ANEGCOMPLETE))
        {
            return 0;
        }

        /* Auto-negotiation enabled, checks the LPA register.*/
        lpa = mii_read(MII_LPA);

        /* Check on link speed.*/
        if (lpa & (MII_LPA__100HALF | MII_LPA__100FULL | MII_LPA__100BASE4))
        {
            maccr |= ETHMAC_MACCR__FES;
        }
        else
        {
            maccr &= ~ETHMAC_MACCR__FES;
        }

        /* Check on link mode.*/
        if (lpa & (MII_LPA__10FULL | MII_LPA__100FULL))
        {
            maccr |= ETHMAC_MACCR__DM;
        }
        else
        {
            maccr &= ~ETHMAC_MACCR__DM;
        }
    }
    else
    {
        /* Link must be established.*/
        if (!(bmsr & MII_BMSR__LSTATUS))
        {
            return 0;
        }

        /* Check on link speed.*/
        if (bmcr & MII_BMCR__SPEED100)
        {
            maccr |= ETHMAC_MACCR__FES;
        }
        else
        {
            maccr &= ~ETHMAC_MACCR__FES;
        }
        /* Check on link mode.*/
        if (bmcr & MII_BMCR__FULLDPLX)
        {
            maccr |= ETHMAC_MACCR__DM;
        }
        else
        {
            maccr &= ~ETHMAC_MACCR__DM;
        }
    }

    /* Changes the mode in the MAC.*/
    *ethmac_get_MACCR() = maccr;

    /* Returns the link status.*/
    return 1;
}

int32_t ethmac_tx_init(ethmac_tx_t *tx)
{
    // Get current TX descriptor
    ethmac_tx_descriptor_t *tx_des = ethmac_current_tx_des;

    // Ensure that descriptor isn't owned by the Ethernet DMA
    if (tx_des->tdes0 & (ETHMAC_TDES0__OWN))
    {
        log_error("current TX descriptor busy");
        return 0;
    }

    // Increment current descriptor
    ethmac_current_tx_des = (ethmac_tx_descriptor_t*) tx_des->tdes3;

    tx->offset = 0;
    tx->size = ETHMAC_BUFFER_SIZE;
    tx->real_des = tx_des;

    return 1;
}

void ethmac_tx_write(ethmac_tx_t *tx, const uint8_t* buffer, uint32_t len)
{
    if (len > tx->size - tx->offset)
    {
        len = tx->size - tx->offset;
    }

    if (len > 0)
    {
        memcpy(
                (uint8_t *) (((ethmac_tx_descriptor_t*) (tx->real_des))->tdes2)
                        + tx->offset, buffer, len);
        tx->offset += len;
    }
}

void ethmac_tx_send(ethmac_tx_t *tx)
{
    /* Unlocks the descriptor and returns it to the DMA engine.*/
    ((ethmac_tx_descriptor_t*) (tx->real_des))->tdes1 = tx->offset;
    ((ethmac_tx_descriptor_t*) (tx->real_des))->tdes0 = ETHMAC_TDES0__IC
            | ETHMAC_TDES0__LS | ETHMAC_TDES0__FS | ETHMAC_TDES0__TCH
            | ETHMAC_TDES0__OWN;

    /* If the DMA engine is stalled then a restart request is issued.*/
    if ((*ethmac_get_DMASR() & ETHMAC_DMASR__TPS_MASK)
            == ETHMAC_DMASR__TPS_SUSPENDED)
    {
        *ethmac_get_DMASR() = ETHMAC_DMASR__TBUS;
        *ethmac_get_DMATPDR() = 0x11; /* Any value is OK.*/
    }
}

int32_t ethmac_rx_init(ethmac_rx_t *rx)
{
    ethmac_rx_descriptor_t *rx_des;
    rx_des = ethmac_current_rx_des;

    // Iterates over frames to found a valid one
    while (!(rx_des->rdes0 & ETHMAC_RDES0__OWN))
    {
        if (!(rx_des->rdes0 & (ETHMAC_RDES0__AFM | ETHMAC_RDES0__ES))
                && (rx_des->rdes0 & ETHMAC_RDES0__FS)
                && (rx_des->rdes0 & ETHMAC_RDES0__LS))
        {
            // Found a valid one
            rx->offset = 0;
            rx->size = ((ethmac_current_rx_des->rdes0 & ETHMAC_RDES0__FL_MASK)
                    >> ETHMAC_RDES0__FL_SHIFT) - 4;
            rx->real_des = rx_des;
            ethmac_current_rx_des = (ethmac_rx_descriptor_t *) rx_des->rdes3;

            // Return OK
            return 1;
        }

        /* Invalid frame found, purging.*/
        rx_des->rdes0 = ETHMAC_RDES0__OWN;
        rx_des = (ethmac_rx_descriptor_t *) ethmac_current_rx_des->rdes3;
    }

    return 0;
}

void ethmac_rx_read(ethmac_rx_t *rx, uint8_t *buffer, uint32_t len)
{
    if (len > rx->size - rx->offset)
    {
        len = rx->size - rx->offset;
    }
    if (len > 0)
    {
        memcpy(buffer,
                (uint8_t *) (((ethmac_rx_descriptor_t*) (rx->real_des))->rdes2)
                        + rx->offset, len);
        rx->offset += len;
    }
}

void ethmac_rx_release(ethmac_rx_t *rx)
{
    // Give buffer back to the Ethernet DMA
    ((ethmac_rx_descriptor_t*) (rx->real_des))->rdes0 = ETHMAC_RDES0__OWN;

    // If the DMA engine is stalled then a restart request is issued
    if ((*ethmac_get_DMASR() & ETHMAC_DMASR__RPS_MASK)
            == ETHMAC_DMASR__RPS_SUSPENDED)
    {
        *ethmac_get_DMASR() = ETHMAC_DMASR__RBUS;
        *ethmac_get_DMARPDR() = 0x22; /* Any value is OK.*/
    }
}

uint32_t ethmac_rx_get_len(ethmac_rx_t *rx)
{
    return rx->size;
}

void ethmac_handle_interrupt()
{
    uint32_t dmasr;
    dmasr = *ethmac_get_DMASR();

    /* Clear status bits.*/
    *ethmac_get_DMASR() = dmasr;

    // Check for read
    if (dmasr & ETHMAC_DMASR__RS)
    {
        // Data received
        ethmac_frame_received_isr();
    }

    if (dmasr & ETHMAC_DMASR__TS)
    {
        // Data Transmitted
        ethmac_frame_sent_isr();
    }
}
__attribute__((weak)) void ethmac_frame_received_isr()
{
    log_debug("Frame received");
}
__attribute__((weak)) void ethmac_frame_sent_isr()
{
    log_debug("Frame sent");
}

/* -------------------------------------------------------------------------- */

static void mii_write(uint32_t reg, uint32_t value)
{
    // Set register value
    *ethmac_get_MACMIIDR() = value;
    // Set address and register
    *ethmac_get_MACMIIAR() = mac.phy_addr | (reg << ETHMAC_MACMIIAR__MR_SHIFT)
            | (1 << ETHMAC_MACMIIAR__CR_SHIFT) | ETHMAC_MACMIIAR__MW
            | ETHMAC_MACMIIAR__MB;

    // Wait until transfer ended
    while ((*ethmac_get_MACMIIAR() & ETHMAC_MACMIIAR__MB) != 0)
        ;
}

static uint32_t mii_read(uint32_t reg)
{
    // Set address and register
    *ethmac_get_MACMIIAR() = mac.phy_addr | (reg << ETHMAC_MACMIIAR__MR_SHIFT)
            | (1 << ETHMAC_MACMIIAR__CR_SHIFT) | ETHMAC_MACMIIAR__MB;
    // Wait until transfer ended
    while ((*ethmac_get_MACMIIAR() & ETHMAC_MACMIIAR__MB) != 0)
        ;
    return *ethmac_get_MACMIIDR();
}

static void mii_find_phy_addr()
{
    uint32_t i;
    for (i = 0; i < 31; i++)
    {
        mac.phy_addr = i << 11;

        uint32_t id1 = mii_read(MII_PHYSID1);
        uint32_t id2 = mii_read(MII_PHYSID2);

        if (id1 != 0xFFFF && id2 != 0xFFFF)
        {
            log_debug("Found ID %08x at address %u", (id1 << 16) + id2, i);
            return;
        }
    }

    log_error("No Address found, using 1");
    mac.phy_addr = 1 << 11;
}

static void set_mac_address(const uint8_t *a)
{
    /* MAC address configuration, only a single address comparator is used,
     hash table not used.*/
    *ethmac_get_MACA0HR() = ((uint32_t) a[5] << 8) | ((uint32_t) a[4] << 0);
    *ethmac_get_MACA0LR() = ((uint32_t) a[3] << 24) | ((uint32_t) a[2] << 16)
            | ((uint32_t) a[1] << 8) | ((uint32_t) a[0] << 0);
    *ethmac_get_MACA1HR() = 0x0000FFFF;
    *ethmac_get_MACA1LR() = 0xFFFFFFFF;
    *ethmac_get_MACA2HR() = 0x0000FFFF;
    *ethmac_get_MACA2LR() = 0xFFFFFFFF;
    *ethmac_get_MACA3HR() = 0x0000FFFF;
    *ethmac_get_MACA3LR() = 0xFFFFFFFF;
    *ethmac_get_MACHTHR() = 0;
    *ethmac_get_MACHTLR() = 0;
}

void ethmac_debug()
{
    log_printf("\n\t\t..........\n");
    log_printf("ethmac_get_DMABMR \t%08x\n", *ethmac_get_DMABMR());
    log_printf("ethmac_get_DMATPDR \t%08x\n", *ethmac_get_DMATPDR());
    log_printf("ethmac_get_DMARPDR \t%08x\n", *ethmac_get_DMARPDR());
    log_printf("ethmac_get_DMARDLAR \t%08x\n", *ethmac_get_DMARDLAR());
    log_printf("ethmac_get_DMATDLAR \t%08x\n", *ethmac_get_DMATDLAR());
    log_printf("ethmac_get_DMASR \t%08x\n", *ethmac_get_DMASR());
    log_printf("ethmac_get_DMAOMR \t%08x\n", *ethmac_get_DMAOMR());
    log_printf("ethmac_get_DMAIER \t%08x\n", *ethmac_get_DMAIER());
    log_printf("ethmac_get_DMAMFBOCR \t%08x\n", *ethmac_get_DMAMFBOCR());
    log_printf("ethmac_get_DMARSWTR \t%08x\n", *ethmac_get_DMARSWTR());
    log_printf("ethmac_get_DMACHTDR \t%08x\n", *ethmac_get_DMACHTDR());
    log_printf("ethmac_get_DMACHTBAR \t%08x\n", *ethmac_get_DMACHTBAR());
    log_printf("ethmac_get_DMACHRBAR \t%08x\n", *ethmac_get_DMACHRBAR());

    log_printf("\n");

    log_printf("ethmac_get_MACCR \t%08x\n", *ethmac_get_MACCR());
    log_printf("ethmac_get_MACFFR \t%08x\n", *ethmac_get_MACFFR());
    log_printf("ethmac_get_MACHTHR \t%08x\n", *ethmac_get_MACHTHR());
    log_printf("ethmac_get_MACHTLR \t%08x\n", *ethmac_get_MACHTLR());
    log_printf("ethmac_get_MACFCR \t%08x\n", *ethmac_get_MACFCR());
    log_printf("ethmac_get_MACDBGR \t%08x\n", *ethmac_get_MACDBGR());
    log_printf("ethmac_get_MACSR \t%08x\n", *ethmac_get_MACSR());

    log_printf("\n\t\t..........\n");
}
