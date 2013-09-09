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
 * mac_csma.h
 *
 *  Created on: Jan 20, 2013
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef MAC_CSMA_H_
#define MAC_CSMA_H_

#include "phy.h"

typedef struct
{
    phy_t phy;
} mac_csma_config_t;
extern const mac_csma_config_t mac_csma_config;

/**
 * Initialize and start the MAC layer.
 *
 * \param channel the channel to use
 */
void mac_csma_init(int channel);

/**
 * Send some data to a node
 */
int mac_csma_data_send(uint16_t dest_addr, const uint8_t *data, uint8_t length);

/** Function called when data is received */
extern void mac_csma_data_indication(uint16_t src_addr, const uint8_t *data,
        uint8_t length);

#endif /* MAC_CSMA_H_ */
