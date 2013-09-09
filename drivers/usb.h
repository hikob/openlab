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
 * \file usb.h
 * \date Aug 30, 2011
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 * \author Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#ifndef USB_H_
#define USB_H_

/**
 * \addtogroup drivers
 * @{
 */

/**
 * \defgroup USB USB driver
 *
 * This driver provides all functions required to perform low level USB 
 * transfers, either synchronously or asynchronously.
 *
 *@{
 */

#include <stdbool.h>
#include <stdint.h>

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
    USB_CLASS_CDC                      = 0x02,
    USB_CLASS_HID                      = 0x03,
    USB_CLASS_MSC                      = 0x08,
    USB_CLASS_HUB                      = 0x09,
    USB_CLASS_APP_SPEC                 = 0xfe,
    USB_CLASS_VENDOR_SPEC              = 0xff
};

enum
{
    USB_SUB_CLASS_CDC_ACM              = 0x02
};

enum
{
    USB_DEV_DESC_SIZE                  = 18,
    USB_CONF_DESC_SIZE                 = 9,
    USB_IFACE_DESC_SIZE                = 9,
    USB_ENDP_DESC_SIZE                 = 7,
    USB_DEV_QUAL_DESC_SIZE             = 10
};

enum
{
    USB_ENDPOINT0_OUT                  = 0x00,
    USB_ENDPOINT1_OUT                  = 0x01,
    USB_ENDPOINT2_OUT                  = 0x02,
    USB_ENDPOINT3_OUT                  = 0x03,
    USB_ENDPOINT4_OUT                  = 0x04,
    USB_ENDPOINT5_OUT                  = 0x05,
    USB_ENDPOINT6_OUT                  = 0x06,
    USB_ENDPOINT7_OUT                  = 0x07,
    USB_ENDPOINT0_IN                   = 0x80,
    USB_ENDPOINT1_IN                   = 0x81,
    USB_ENDPOINT2_IN                   = 0x82,
    USB_ENDPOINT3_IN                   = 0x83,
    USB_ENDPOINT4_IN                   = 0x84,
    USB_ENDPOINT5_IN                   = 0x85,
    USB_ENDPOINT6_IN                   = 0x86,
    USB_ENDPOINT7_IN                   = 0x87
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


typedef enum
{
    USB_ENDPOINT_CONTROL   = 0x00,
    USB_ENDPOINT_ISO       = 0x01,
    USB_ENDPOINT_BULK      = 0x02,
    USB_ENDPOINT_INTERRUPT = 0x03
} usb_endp_type_t;

typedef struct
{
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} __attribute__((packed, aligned(4))) usb_device_request_t;

typedef void (*usb_device_request_callback_t)(usb_device_request_t *header);
typedef bool (*usb_data_callback_t)(uint8_t endp);

typedef struct
{
    const usb_dev_desc_t      *device_descriptor;
    const usb_dev_qual_desc_t *device_qualifier_descriptor;
    const uint8_t             *number_of_string_descriptors;
    const usb_string_desc_t   *string_descriptors;

    usb_device_request_callback_t standard_interface;
    usb_device_request_callback_t standard_endpoint;
    usb_device_request_callback_t class_device;
    usb_device_request_callback_t class_interface;
    usb_device_request_callback_t class_endpoint;
    usb_device_request_callback_t vendor_device;
    usb_device_request_callback_t vendor_interface;
    usb_device_request_callback_t vendor_endpoint;
} usb_profile_t;

typedef enum
{
    STAT_TX_DISABLED = 0x0000,
    STAT_TX_STALL    = 0x0010,
    STAT_TX_NAK      = 0x0020,
    STAT_TX_VALID    = 0x0030,
    STAT_RX_DISABLED = 0x0000,
    STAT_RX_STALL    = 0x1000,
    STAT_RX_NAK      = 0x2000,
    STAT_RX_VALID    = 0x3000
} stat_bits_t;

typedef enum
{
    DATA0 = 0,
    DATA1 = 1,
    NO_CHANGE
} usb_data_token_t;


/**
 * Enable the platform USB, MUST be implemented by the platform
 */
extern void platform_usb_enable();


/**
 * USB Driver init 
 */

void     usb_driver_init( const usb_profile_t * );

extern const uint8_t           usb_string_desc_size;
extern const usb_string_desc_t usb_string_desc[];

/**
 * USB Send and Receive 
 */

uint16_t usb_get_max_packet_size(uint8_t endp);

void     usb_send(uint8_t endp, bool first_time, usb_data_token_t first_token, const uint8_t *buf, uint16_t len);
void     usb_send_set_status(uint8_t endp, stat_bits_t stat);

uint16_t usb_recv_get_len(uint8_t endp);
void     usb_recv(uint8_t endp, uint8_t *buf, uint16_t len);
void     usb_recv_set_status(uint8_t endp, stat_bits_t stat);

/**
 * Internal USB active function and interface
 */ 

void     usb_set_interface(uint16_t  interface, uint16_t  alternate);
void     usb_get_interface(uint16_t *interface, uint16_t *alternate);

/**
 * Default handlers
 */

void     usb_default_process_class_endpoint(usb_device_request_t *req);
void     usb_default_process_standard_interface(usb_device_request_t *req);

/**
 *
 */

void     usb_set_next_data_callback(uint8_t endp, usb_data_callback_t cb);
void     usb_start_stage(uint8_t endp, bool in);
void     usb_send_status(uint8_t endp, bool in);

/**
 * @}
 * @}
 */

#endif /* USB_H_ */
