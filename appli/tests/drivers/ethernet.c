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
 * ethernet.c
 *
 *  Created on: Sep 21, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "platform.h"
#include "printf.h"
#include "ethmac.h"

#include "soft_timer.h"
#include "debug.h"

int main()
{
    platform_init();

    // Start the MAC layer
    ethmac_enable();
    uint8_t addr[] =
    { 1, 2, 3, 4, 5, 6 };
    ethmac_start(addr);
    ethmac_debug();

    // Wait a little
    soft_timer_delay_s(10);

    while (1)
    {
        ethmac_debug();
        // Get link status
        if (ethmac_get_link_status())
        {
            soft_timer_delay_s(1);

            ethmac_tx_t tx;
            if (ethmac_tx_init(&tx))
            {
                log_info("Sending...");

                ethmac_tx_write(&tx, (const uint8_t *)"hello, world!", 13);
                ethmac_tx_send(&tx);
            }

        }
        // Wait a little
        soft_timer_delay_s(10);
    }

    return 0;
}
