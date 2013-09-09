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
 * dfu.h
 *
 *  Created on: Nov 21, 2011
 *      Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#ifndef __DFU_H
#define __DFU_H

/* ****************************************************
 * DFU Functionnal Descriptor                         *
 * used both for Runtime and DFU Mode                 *
 * ************************************************** */

typedef struct
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bmAttributes;
    uint16_t wDetachTimeOut;
    uint16_t wMaxPacketSize;
    uint16_t bcdDFUVersion;
} __attribute__((packed)) usb_dfu_func_desc_t;

/* ****************************************************
 * DFU Functionnal Descriptor                         *
 * used both for Runtime and DFU Mode                 *
 * ************************************************** */

const extern usb_dfu_func_desc_t dfu_func_desc[];

/* ****************************************************
 * This interface must be registered as the last interface in
 * a runtime descriptor set.
 *
 * This interface must be modified by user:
 *    dfu_runtime_interface.bInterfaceNumber = xxx
 *    dfu_runtime_interface.iInterface       = xxx
 * must be set according to the place where DFU is registered.
 * DFU is usually registered as the last interface of a device.
 * ************************************************** */

#define DFU_RUNTIME_INTERFACE(iface,istring)	\
    {								         \
        .bLength              = USB_IFACE_DESC_SIZE,			 \
        .bDescriptorType      = USB_DESC_INTERFACE,			 \
	.bInterfaceNumber     = iface,   /* must be set by owner      */ \
	.bAlternateSetting    = 0,       /* must be 0                 */ \
	.bNumEndpoints        = 0,       /* Use only the control pipe */ \
	.bInterfaceClass      = USB_CLASS_APP_SPEC,			 \
	.bInterfaceSubClass   = 1,       /* 1 = Boot interface        */ \
	.bInterfaceProtocol   = 0,       /* 0 = None                  */ \
	.iInterface           = istring, /* must be set by owner      */ \
	.class_specific       = 0,					 \
	.class_specific_len   = 0,					 \
	.endpoint_descriptors = (const usb_endp_desc_t*)dfu_func_desc    \
    }

/* ****************************************************
 * DFU full profile.
 * This profile is set when the device enters DFU mode.
 * ************************************************** */

extern const usb_profile_t usb_dfu;

/* ****************************************************
 *
 *
 * ************************************************** */

void usb_dfu_init();
void usb_dfu_detach_reset_check();
void usb_dfu_class_interface(usb_device_request_t *req);

#endif
