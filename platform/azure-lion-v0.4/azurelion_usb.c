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
 * agilefox_usb.c
 *
 *  Created on: Jul 1, 2011
 *      Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#include "platform.h"
#include "azurelion.h"

#include "usb_.h"
#include "libusb.h"

#define WEAK __attribute__((weak))

/* ************************************************** */
/* ************************************************** */

//WEAK const uint16_t usb_VendorId           = 0x0483;
//WEAK const uint16_t usb_ProductId          = 0x5740;

WEAK const uint8_t  scsi_vendor_string[8]    = { 'H','i','K','o','B',0,0,0 };
WEAK const uint8_t  scsi_product_string[16]  = { 'A','z','u','r','e',' ','L','i','o','n',0,0,0,0,0,0 };
WEAK const uint8_t  scsi_rev_string[4]       = { '1','.','0','0' };

static const uint16_t usb_langid_string[]    = { 0x0409 };
static const uint16_t usb_vendor_string[]    = { 'H','i','K','o','B' };
static const uint16_t usb_product_string[]   = { 'A','z','u','r','e',' ','L','i','o','n' };
static const uint16_t usb_dfu_string[]       = { 'U','S','B',' ','D','F','U',' ','1','.','0' };
static const uint16_t usb_serial_string[48];

WEAK const uint8_t        usb_string_desc_size = 5;
WEAK const usb_string_desc_t usb_string_desc[] =
{
    // Lang ID
    {
	.bLength = 2 + sizeof(usb_langid_string),
	.bDescriptorType = USB_DESC_STRING,
	.wData = usb_langid_string
    },
    // Manufacturer
    {
	.bLength = 2 + sizeof(usb_vendor_string),
	.bDescriptorType = USB_DESC_STRING,
	.wData = usb_vendor_string
    },
    // Product
    {
	.bLength = 2 + sizeof(usb_product_string),
	.bDescriptorType = USB_DESC_STRING,
	.wData = usb_product_string
    },
    // Serial
    {
	.bLength = 2 + sizeof(usb_serial_string),
	.bDescriptorType = USB_DESC_STRING,
	.wData = usb_serial_string
    },
    // DFU
    {
	.bLength = 2 + sizeof(usb_dfu_string),
	.bDescriptorType = USB_DESC_STRING,
	.wData = usb_dfu_string
    }
};

/* ************************************************** */
/* ************************************************** */

WEAK const mmap_entry_t mmapfs_files[] = 
{
    {
	.arg     = NULL,
	.name    = "EEPROM", 
	.ext     = "RAW", 
	.maxsize = 4096, 
	.info    = NULL,
	.read    = storage_eeprom_read,
	.write   = storage_eeprom_write,
	.unlink  = NULL
    },
};

WEAK const mmapfs_t _mmapfs = {
    .size  = 1, 
    .files = mmapfs_files,
};

/* ************************************************** */
/* ************************************************** */

WEAK const uint8_t  scsi_max_lun = 0;
WEAK const scsi_lun_t scsi_lun[] = {
    { 
	.type          = USB_MSC_MMAPFS,
	.info          = & _mmapfs,
	.init          = scsi_mmapfs_init,
	.read_capacity = scsi_mmapfs_read_capacity,
	.read10        = scsi_mmapfs_read10,
	.write10       = scsi_mmapfs_write10 
    },
};

/* ************************************************** */
/* ************************************************** */

void usb_lp_isr()
{
    usb_handle_interrupt();
}
