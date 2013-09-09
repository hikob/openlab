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
 * ethmac_descriptors.h
 *
 *  Created on: Sep 21, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef ETHMAC_DESCRIPTORS_H_
#define ETHMAC_DESCRIPTORS_H_

#include <stdint.h>

typedef struct
{
    volatile uint32_t rdes0;
    volatile uint32_t rdes1;
    volatile uint32_t rdes2;
    volatile uint32_t rdes3;
} ethmac_rx_descriptor_t;

typedef struct
{
    volatile uint32_t tdes0;
    volatile uint32_t tdes1;
    volatile uint32_t tdes2;
    volatile uint32_t tdes3;
} ethmac_tx_descriptor_t;

enum
{
    ETHMAC_RXDES_NUMBER = 4,
    ETHMAC_TXDES_NUMBER = 4,
    ETHMAC_BUFFER_SIZE = 1522,
};

extern ethmac_rx_descriptor_t ethmac_rx_des[ETHMAC_RXDES_NUMBER];
extern ethmac_tx_descriptor_t ethmac_tx_des[ETHMAC_TXDES_NUMBER];

extern ethmac_rx_descriptor_t *ethmac_current_rx_des;
extern ethmac_tx_descriptor_t *ethmac_current_tx_des;

enum
{
    ETHMAC_TDES0__OWN = 0x80000000,
    ETHMAC_TDES0__IC = 0x40000000,
    ETHMAC_TDES0__LS = 0x20000000,
    ETHMAC_TDES0__FS = 0x10000000,
    ETHMAC_TDES0__DC = 0x08000000,
    ETHMAC_TDES0__DP = 0x04000000,
    ETHMAC_TDES0__TTSE = 0x02000000,
    ETHMAC_TDES0__CIC_MASK = 0x00C00000,
    ETHMAC_TDES0__TER = 0x00200000,
    ETHMAC_TDES0__TCH = 0x00100000,

    ETHMAC_TDES0__TTSS = 0x00020000,
    ETHMAC_TDES0__IHE = 0x00010000,
    ETHMAC_TDES0__ES = 0x00008000,
    ETHMAC_TDES0__JT = 0x00004000,
    ETHMAC_TDES0__FF = 0x00002000,
    ETHMAC_TDES0__IPE = 0x00001000,
    ETHMAC_TDES0__LCA = 0x00000800,
    ETHMAC_TDES0__NC = 0x00000400,
    ETHMAC_TDES0__LCO = 0x00000200,
    ETHMAC_TDES0__EC = 0x00000100,
    ETHMAC_TDES0__VF = 0x00000080,
    ETHMAC_TDES0__CC_MASK = 0x00000078,
    ETHMAC_TDES0__ED = 0x00000004,
    ETHMAC_TDES0__UF = 0x00000002,
    ETHMAC_TDES0__DB = 0x00000001,
};

enum
{
    ETHMAC_TDES1__TBS2_MASK = 0x1FFF0000,
    ETHMAC_TDES1__TBS2_SHIFT = 16,

    ETHMAC_TDES1__TBS1_MASK = 0x00001FFF,
    ETHMAC_TDES1__TBS1_SHIFT = 0,
};

enum
{
    ETHMAC_RDES0__OWN = 0x80000000,
    ETHMAC_RDES0__AFM = 0x40000000,

    ETHMAC_RDES0__FL_MASK = 0x3FFF0000,
    ETHMAC_RDES0__FL_SHIFT = 16,

    ETHMAC_RDES0__ES = 0x00008000,
    ETHMAC_RDES0__DE = 0x00004000,
    ETHMAC_RDES0__SAF = 0x00002000,
    ETHMAC_RDES0__LE = 0x00001000,
    ETHMAC_RDES0__OE = 0x00000800,
    ETHMAC_RDES0__VLAN = 0x00000400,
    ETHMAC_RDES0__FS = 0x00000200,
    ETHMAC_RDES0__LS = 0x00000100,
    ETHMAC_RDES0__IPHCE_TSV = 0x00000080,
    ETHMAC_RDES0__LCO = 0x00000040,
    ETHMAC_RDES0__FT = 0x00000020,
    ETHMAC_RDES0__RWT = 0x00000010,
    ETHMAC_RDES0__RE = 0x00000008,
    ETHMAC_RDES0__DBE = 0x00000004,
    ETHMAC_RDES0__CE = 0x00000002,
    ETHMAC_RDES0__PCE_ESA = 0x00000001,
};

enum
{
    ETHMAC_RDES1__DIC = 0x80000000,
    ETHMAC_RDES1__RBS2_MASK = 0x1FFF0000,
    ETHMAC_RDES1__RER = 0x00008000,
    ETHMAC_RDES1__RCH = 0x00004000,
    ETHMAC_RDES1__RBS_MASK = 0x00001FFF,
};

/** Initialize the descriptors */
void ethmac_descriptors_init();

/** Reset the descriptors */
void ethmac_descriptors_reset();

#endif /* ETHMAC_DESCRIPTORS_H_ */
