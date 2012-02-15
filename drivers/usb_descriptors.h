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
 * Copyright (C) 2011 HiKoB.
 */

/*
 * usb_descriptors.h
 *
 *  Created on: Aug 30, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

enum
{
    USB_DESC_DEVICE                    = 0x01,
    USB_DESC_CONFIGURATION             = 0x02,
    USB_DESC_STRING                    = 0x03,
    USB_DESC_INTERFACE                 = 0x04,
    USB_DESC_ENDPOINT                  = 0x05,
    USB_DESC_DEVICE_QUALIFIER          = 0x06,
    USB_DESC_OTHER_SPEED_CONFIGURATION = 0x07,
    USB_DESC_INTERFACE_POWER           = 0x08
};

enum
{
    USB_CLASS_CDC          = 0x02,
    USB_CLASS_HID          = 0x03,
    USB_CLASS_MASS_STORAGE = 0x08,
    USB_CLASS_APP_SPEC     = 0xfe,
    USB_CLASS_VENDOR_SPEC  = 0xff
};

enum
{
    USB_SUB_CLASS_CDC_ACM = 0x02
};

enum
{
    USB_DEV_DESC_SIZE      = 18,
    USB_CONF_DESC_SIZE     = 9,
    USB_IFACE_DESC_SIZE    = 9,
    USB_ENDP_DESC_SIZE     = 7,
    USB_DEV_QUAL_DESC_SIZE = 10
};

enum
{
    USB_ENDPOINT0_OUT = 0x00,
    USB_ENDPOINT1_OUT = 0x01,
    USB_ENDPOINT2_OUT = 0x02,
    USB_ENDPOINT3_OUT = 0x03,
    USB_ENDPOINT4_OUT = 0x04,
    USB_ENDPOINT5_OUT = 0x05,
    USB_ENDPOINT6_OUT = 0x06,
    USB_ENDPOINT7_OUT = 0x07,
    USB_ENDPOINT0_IN  = 0x80,
    USB_ENDPOINT1_IN  = 0x81,
    USB_ENDPOINT2_IN  = 0x82,
    USB_ENDPOINT3_IN  = 0x83,
    USB_ENDPOINT4_IN  = 0x84,
    USB_ENDPOINT5_IN  = 0x85,
    USB_ENDPOINT6_IN  = 0x86,
    USB_ENDPOINT7_IN  = 0x87
};

typedef void (*usb_endpoint_callback_t)(uint8_t, bool, bool);

typedef struct
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;

    // Class specific descriptor
    const uint8_t  *class_specific;
    uint8_t  class_specific_len;

    // Endpoint callback
    usb_endpoint_callback_t endpoint_callback;
} __attribute__((packed)) usb_endp_desc_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;

    // Class specific descriptor
    const uint8_t  *class_specific;
    uint8_t  class_specific_len;

    // Table of endpoint for the given interface
    const usb_endp_desc_t *endpoint_descriptors;
} __attribute__((packed)) usb_iface_desc_t;

typedef struct
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t wTotalLength;
    uint8_t  bNumInterfaces;
    uint8_t  bConfigurationValue;
    uint8_t  iConfiguration;
    uint8_t  bmAttributes;
    uint8_t  bMaxPower;

    // Class specific descriptor
    const uint8_t *class_specific;
    uint8_t  class_specific_len;

    // Table of interfaces for the given interface
    const usb_iface_desc_t *interface_descriptors;
} __attribute__((packed)) usb_conf_desc_t;

typedef struct
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t  iManufacturer;
    uint8_t  iProduct;
    uint8_t  iSerialNumber;
    uint8_t  bNumConfigurations;

    // Class specific descriptor
    const uint8_t *class_specific;
    uint8_t  class_specific_len;

    // Table of configurations for the given device
    const usb_conf_desc_t *configuration_descriptors;
} __attribute__((packed)) usb_dev_desc_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    const uint16_t *wData;
} __attribute__((packed)) usb_string_desc_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint8_t bNumConfigurations;
    uint8_t bReserved;
} __attribute__((packed)) usb_dev_qual_desc_t;

#endif
