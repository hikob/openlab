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

/**
 * \file libusb.h
 * \date Aug 30, 2011
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 * \author Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#ifndef LIBUSB_H_
#define LIBUSB_H_

/**
 * \addtogroup lib
 * @{
 */

#include <stdbool.h>
#include <stdint.h>

#include "usb.h"
#include "usb/hid.h"
#include "usb/hid_kbd.h"
#include "usb/cdc_acm.h"
#include "usb/msc.h"
#include "usb/scsi.h"

#include "usb/scsi_mmapfs.h"
#include "usb/storage.h"
#include "usb/dfu.h"

/** @addtogroup lib
 * @{
 */

/** @defgroup libusb 
 * The USB driver is a high level interface to USB.
 * 
 * defines the configuration function and the data access function of this sensor.
 * @{
 */

#define USB_HID      &usb_hid
#define USB_HID_KBD  &usb_hid_kbd
#define USB_CDC_ACM  &usb_cdc_acm
#define USB_MSC      &usb_msc
#define USB_DFU      &usb_dfu

/*
 * the following defines have been moved to their specific .c files
 *
 * MSC
 * #define usb_VendorId   0x0483
 * #define usb_ProductId  0x5740
 *
 * HID
 * #define usb_VendorId   0x046D
 * #define usb_ProductId  0xC063
 *
 * CDC-ACM
 * #define usb_VendorId   0x16C0
 * #define usb_ProductId  0x05E1
 */

void usb_init(const usb_profile_t *profile);

/** @} */

#endif /* LIBUSB_H_ */
