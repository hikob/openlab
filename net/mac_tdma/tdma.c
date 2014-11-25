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
 * Copyright (C) 2013 HiKoB.
 */

/*
 * tdma.c
 *
 * \date Jan 09, 2013
 * \author: Damien Hedde <damien.hedde.at.hikob.com>
 */

#include "platform.h"
#include "FreeRTOS.h"
#include "unique_id.h"

#include <string.h>

#include "tdma_internal.h"

#include "debug.h"

static xSemaphoreHandle tdma_mutex;
struct tdma_global tdma_data;

void mac_tdma_init (uint16_t addr)
{
    if (tdma_mutex == NULL)
    {
        tdma_mutex = xSemaphoreCreateMutex();
    }

    /* Init libs. */
    soft_timer_init();
    packet_init();

    /* Init tdma modules. */
    tdma_frame_init();
    tdma_slot_init();

    /* eventually generate address */
    if (addr == 0x0000 || addr == 0xffff)
    {
        addr = uid->uid16[0] ^ uid->uid16[1] ^ uid->uid16[2]
            ^ uid->uid16[3] ^ uid->uid16[4] ^ uid->uid16[5];
        if (addr == 0x0000 || addr == 0xffff)
        {
            addr ^= 0x00ff;
        }
        log_info("Generating address from id -> %04x", addr);
    }

    /* Init structure */
    tdma_data.addr = addr;
    tdma_data.state = TDMA_IDLE;
    tdma_data.slot_cb = NULL;
}

void tdma_get ()
{
    while (xSemaphoreTake(tdma_mutex, configTICK_RATE_HZ) != pdPASS)
    {
        log_error("Failed to get TDMA semaphore");
    }
}

void tdma_release ()
{
    if (xSemaphoreGive(tdma_mutex) != pdPASS)
    {
        log_error("Failed to release the TDMA semaphore");
        HALT();
    }
}

bool mac_tdma_is_connected ()
{
    bool res;
    switch (tdma_data.state)
    {
        case TDMA_NODE:
        case TDMA_COORD:
            res = true;
            break;
        default:
            res = false;
            break;
    }
    return res;
}

void mac_tdma_set_recv_handler(mac_tdma_rx_handler_t handler)
{
    tdma_data.data_rx_handler = handler;
}

uint16_t mac_tdma_get_address ()
{
    return tdma_data.addr;
}

uint16_t mac_tdma_get_coordinator ()
{
    return tdma_data.pan.coord;
}

void mac_tdma_set_slot_callback(mac_tdma_sl_callback_t cb)
{
    tdma_data.slot_cb = cb;
}
