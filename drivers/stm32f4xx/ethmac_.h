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
 * ethmac_.h
 *
 *  Created on: Sep 20, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef ETHMAC__H_
#define ETHMAC__H_

#include "ethmac.h"

typedef enum
{
    ETHMAC_MODE_MII = 0,
    ETHMAC_MODE_RMII = 1,
} ethmac_mode_t;

/**
 * Initialize the Ethernet MAC driver
 *
 * \param mode the PHY communication mode
 * \param always_on 1 if link is always on, 0 if not
 *      (set to 1 when connected to a switch, 0 when connected to a PHY)
 */
void ethmac_init(ethmac_mode_t mode, int always_on);

/** Handle an interrupt */
void ethmac_handle_interrupt();

#endif /* ETHMAC__H_ */
