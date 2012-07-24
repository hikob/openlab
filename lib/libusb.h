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
 * libusb.h
 *
 *  Created on: Aug 30, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 *      Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#ifndef LIBUSB_H_
#define LIBUSB_H_

#include <stdbool.h>
#include <stdint.h>

#include "usb.h"
#include "usb/hid.h"
#include "usb/cdc_acm.h"
#include "usb/msc.h"
#include "usb/scsi.h"


#define USB_HID      &usb_hid
#define USB_CDC_ACM  &usb_cdc_acm
#define USB_MSC      &usb_msc

#define usb_VendorId   0x0483
#define usb_ProductId  0x5740


void usb_init(const usb_profile_t *profile);

#endif
