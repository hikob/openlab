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
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 *
 */

#ifndef __USB_HID_H
#define __USB_HID_H

extern const usb_profile_t usb_hid;

/**
 * USB HID Application Interface
 */

void hid_send(int8_t x, int8_t y, bool button1, bool button2, bool button3);

#endif

