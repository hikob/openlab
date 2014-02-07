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
 * azurelion_net.c
 *
 *  Created on: Nov 4, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include "azurelion.h"

#include "rf2xx.h"
#include "phy_rf2xx/phy_rf2xx.h"

#include "debug.h"

/* Phy Instantiation */
static phy_rf2xx_t phy_rf231, phy_rf212;
phy_t platform_phy = &phy_rf231, phy_868 = &phy_rf212;

void platform_net_setup()
{
    // Check if SLP_TR if bound to the timer channel output
    if (al04_has_slptr_on_timer)
    {
        log_info("PHY set for SLPTR");
        phy_rf231.timer_is_slptr = 1;
    }

    // Setup the PHY library, with the same timer as the radio chip
    phy_rf2xx_init(&phy_rf231, rf231, TIM_3, TIMER_CHANNEL_1);

    // Setup the PHY library for 868MHz
    phy_rf2xx_init(&phy_rf212, rf212, TIM_3, TIMER_CHANNEL_2);
}
