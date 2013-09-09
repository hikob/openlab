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
 * ethmac_descriptors.c
 *
 *  Created on: Sep 21, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "ethmac_descriptors.h"

ethmac_rx_descriptor_t ethmac_rx_des[ETHMAC_RXDES_NUMBER];
ethmac_tx_descriptor_t ethmac_tx_des[ETHMAC_TXDES_NUMBER];

ethmac_rx_descriptor_t *ethmac_current_rx_des;
ethmac_tx_descriptor_t *ethmac_current_tx_des;

static uint8_t rx_buffers[ETHMAC_RXDES_NUMBER][ETHMAC_BUFFER_SIZE];
static uint8_t tx_buffers[ETHMAC_TXDES_NUMBER][ETHMAC_BUFFER_SIZE];

void ethmac_descriptors_init()
{
    unsigned i;

    // Initialize the descriptors in chain mode
    for (i = 0; i < ETHMAC_RXDES_NUMBER; i++)
    {
        ethmac_rx_des[i].rdes1 = ETHMAC_RDES1__RCH | ETHMAC_BUFFER_SIZE;
        ethmac_rx_des[i].rdes2 = (uint32_t) rx_buffers[i];
        ethmac_rx_des[i].rdes3 = (uint32_t) (&ethmac_rx_des[(i + 1)
                % ETHMAC_RXDES_NUMBER]);
    }

    for (i = 0; i < ETHMAC_TXDES_NUMBER; i++)
    {
        ethmac_tx_des[i].tdes1 = 0;
        ethmac_tx_des[i].tdes2 = (uint32_t) tx_buffers[i];
        ethmac_tx_des[i].tdes3 = (uint32_t) (&ethmac_tx_des[(i + 1)
                % ETHMAC_TXDES_NUMBER]);
    }
}

void ethmac_descriptors_reset()
{
    unsigned i;

    /* Resets the state of all descriptors.*/
    for (i = 0; i < ETHMAC_RXDES_NUMBER; i++)
    {
        ethmac_rx_des[i].rdes0 = ETHMAC_RDES0__OWN;
    }

    for (i = 0; i < ETHMAC_TXDES_NUMBER; i++)
    {
        ethmac_tx_des[i].tdes0 = ETHMAC_TDES0__TCH;
    }

    // Set current descriptors to first
    ethmac_current_rx_des = ethmac_rx_des;
    ethmac_current_tx_des = ethmac_tx_des;
}
