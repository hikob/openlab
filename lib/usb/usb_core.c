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
 * Copyright (C) 2012 HiKoB.
 */

/*
 * usb_core.c
 *
 *  Created on: Jun 15, 2012
 *      Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>et
 */

#include "platform.h"
#include "usb.h"
#include "libusb.h"

#define NO_DEBUG_HEADER
#define LOG_LEVEL LOG_LEVEL_WARNING
#include "printf.h"

#if (LOG_LEVEL < LOG_LEVEL_DEBUG)
#define DBG(x...)   printf(x)
#else
#define DBG(x...)   do { } while(0)
#endif // LOG_LEVEL



void usb_init(const usb_profile_t *profile)
{
    usb_driver_init( profile );

    usb_msc_init();
    usb_dfu_init();

    platform_usb_enable();
}
