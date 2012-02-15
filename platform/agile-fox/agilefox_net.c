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
 * agilefox_net.c
 *
 *  Created on: Dec 21, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "platform.h"

#include "rf2xx.h"
#include "lib/net_timer.h"
#include "phy_rf2xx/phy_rf2xx.h"

/* Phy Instantiation */
static phy_rf2xx_t phy_rf231;
phy_t phy = &phy_rf231;


void platform_net_setup()
{
    // Setup the NET_TIMER library, timer 3
    net_timer_init(tim3);

    // Setup the PHY libraries
    phy_rf2xx_init(&phy_rf231, rf231, tim3, TIMER_CHANNEL_3);
}
