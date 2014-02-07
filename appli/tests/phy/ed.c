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
 * ed.c
 *
 *  Created on: March 6, 2013
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "printf.h"

#include "phy.h"

#include "event.h"

// Task function
static void do_ed(handler_arg_t arg);

#if 1
#define PHY platform_phy
#else
extern phy_t phy_868;
#define PHY phy_868
#endif

int main()
{
    // Initialize the platform
    platform_init();

    printf("PHY SimpleRX test\n");
    event_post(EVENT_QUEUE_APPLI, do_ed, NULL );

    platform_run();
    return 0;
}

static void do_ed(handler_arg_t arg)
{
    phy_idle(PHY);

    // Set channel
    phy_set_channel(PHY, 15);

    // Request ED
    int32_t ed;

    if (phy_ed(PHY, &ed) == PHY_SUCCESS)
    {
        printf("ED: %ddBm\n", ed);
    }
    else
    {
        printf("ED failed...\n");
    }

    event_post(EVENT_QUEUE_APPLI, do_ed, NULL);
}

