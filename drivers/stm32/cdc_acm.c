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
 * cdc_acm.c
 *
 *  Created on: Aug 30, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "usb.h"
#include "usb_registers.h"
#include "cdc_acm.h"

#define NO_DEBUG_HEADER
#define LOG_LEVEL LOG_LEVEL_ERROR
#include "printf.h"

#ifndef NULL
#define NULL (void*)0
#endif

enum
{
    SEND_ENCAPSULATED_COMMAND = 0x00,
    GET_ENCAPSULATED_RESPONSE = 0x01,
    SET_COMM_FEATURE          = 0x02,
    GET_COMM_FEATURE          = 0x03,
    CLEAR_COMM_FEATURE        = 0x04,
    // 0x05 - 0x0F: Reserved
    SET_AUX_LINE_STATE        = 0x10,
    SET_HOOK_STATE            = 0x11,
    PULSE_SETUP               = 0x12,
    SEND_PULSE                = 0x13,
    SET_PULSE_TIME            = 0x14,
    RING_AUX_JACK             = 0x15,
    // 0x16 - 0x1F: Reserved
    SET_LINE_CODING           = 0x20,
    GET_LINE_CODING           = 0x21,
    SET_CONTROL_LINE_STATE    = 0x22,
    SEND_BREAK                = 0x23,
    // 0x24 - 0x2F: Reserved
    SET_RINGER_PARMS          = 0x30,
    GET_RINGER_PARMS          = 0x31,
    SET_OPERATION_PARMS       = 0x32,
    GET_OPERATION_PARMS       = 0x33,
    SET_LINE_PARMS            = 0x34,
    GET_LINE_PARMS            = 0x35,
    DIAL_DIGITS               = 0x36,
    SET_UNIT_PARAMETER        = 0x37,
    GET_UNIT_PARAMETER        = 0x38,
    CLEAR_UNIT_PARAMETER      = 0x39,
    GET_PROFILE               = 0x3A,
    // 0x3B - 0x3F: Reserved
    SET_ETHERNET_MULTICAST_FILTERS               = 0x40,
    SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER = 0x41,
    GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER = 0x42,
    SET_ETHERNET_PACKET_FILTER                   = 0x43,
    GET_ETHERNET_STATISTIC                       = 0x44,
    // 0x45 - 0x4F: Reserved
    SET_ATM_DATA_FORMAT                          = 0x50,
    GET_ATM_DEVICE_STATISTICS                    = 0x51,
    SET_ATM_DEFAULT_VC                           = 0x52,
    GET_ATM_VC_STATISTICS                        = 0x53
    // 0x54 - 0xFF: Reserved
};

/***** Descriptors *****/

static void cdc_acm_comm(uint8_t endp, bool rx, bool tx);

static const usb_endp_desc_t cdc_acm_comm_endp_desc[] =
{
    {
        .bLength            = USB_ENDP_DESC_SIZE,
        .bDescriptorType    = USB_DESC_ENDPOINT,
        .bEndpointAddress   = USB_ENDPOINT3_IN,
        .bmAttributes       = USB_ENDPOINT_INTERRUPT,
        .wMaxPacketSize     = 16,
        .bInterval          = 255,

        .class_specific     = 0,
        .class_specific_len = 0,

        .endpoint_callback = cdc_acm_comm
    }
};

static void cdc_acm_data_in(uint8_t endp, bool rx, bool tx);
static void cdc_acm_data_out(uint8_t endp, bool rx, bool tx);

static const usb_endp_desc_t cdc_acm_data_endp_desc[] =
{
    {
        .bLength            = USB_ENDP_DESC_SIZE,
        .bDescriptorType    = USB_DESC_ENDPOINT,
        .bEndpointAddress   = USB_ENDPOINT1_OUT,
        .bmAttributes       = USB_ENDPOINT_BULK,
        .wMaxPacketSize     = 16,
        .bInterval          = 1,

        .class_specific     = 0,
        .class_specific_len = 0,

        .endpoint_callback  = cdc_acm_data_in
    },
    {
        .bLength            = USB_ENDP_DESC_SIZE,
        .bDescriptorType    = USB_DESC_ENDPOINT,
        .bEndpointAddress   = USB_ENDPOINT2_IN,
        .bmAttributes       = USB_ENDPOINT_BULK,
        .wMaxPacketSize     = 16,
        .bInterval          = 1,

        .class_specific     = 0,
        .class_specific_len = 0,

        .endpoint_callback  = cdc_acm_data_out
    }
};

static const struct
{
    // Header functional descriptor
    uint8_t bFunctionLength_header;
    uint8_t bDescriptorType_header;
    uint8_t bDescriptorSubtype_header;
    uint16_t bcdCDC_header;
    // Call Management functional descriptor
    uint8_t bFunctionLength_call;
    uint8_t bDescriptorType_call;
    uint8_t bDescriptorSubtype_call;
    uint8_t bmCapabilities_call;
    uint8_t bDataInterface_call;
    // Abstract Control Management functional descriptor
    uint8_t bFunctionLength_acm;
    uint8_t bDescriptorType_acm;
    uint8_t bDescriptorSubtype_acm;
    uint8_t bmCapabilities_acm;
    // Union functional descriptor
    uint8_t bFunctionLength_union;
    uint8_t bDescriptorType_union;
    uint8_t bDescriptorSubtype_union;
    uint8_t bMasterInterface_union;
    uint8_t bSlaveInterface0_union;
} __attribute__((packed)) cdc_acm_func_desc =
{
    // Header
    .bFunctionLength_header    = 5,
    .bDescriptorType_header    = 0x24,   // CS_INTERFACE
    .bDescriptorSubtype_header = 0,      // Header
    .bcdCDC_header             = 0x0110, // 1.1
    // Call Management
    .bFunctionLength_call      = 5,
    .bDescriptorType_call      = 0x24,   // CS_INTERFACE
    .bDescriptorSubtype_call   = 1,      // Call Management
    .bmCapabilities_call       = 0,
    .bDataInterface_call       = 1,
    // Abstract Control Management
    .bFunctionLength_acm       = 4,
    .bDescriptorType_acm       = 0x24,   // CS_INTERFACE
    .bDescriptorSubtype_acm    = 2,      // Abstract Control Management
    .bmCapabilities_acm        = 0,
    // Union
    .bFunctionLength_union     = 5,
    .bDescriptorType_union     = 0x24,   // CS_INTERFACE
    .bDescriptorSubtype_union  = 6,      // Union
    .bMasterInterface_union    = 0,      // Communication class interface
    .bSlaveInterface0_union    = 1       // Data class interface
};

#define USB_CDC_ACM_FUNC_DESC_SIZE 19

static const usb_iface_desc_t cdc_acm_iface_desc[] =
{
    // Comm interface
    {
        .bLength              = USB_IFACE_DESC_SIZE,
        .bDescriptorType      = USB_DESC_INTERFACE,
        .bInterfaceNumber     = 0,
        .bAlternateSetting    = 0,
        .bNumEndpoints        = 1,
        .bInterfaceClass      = USB_CLASS_CDC,
        .bInterfaceSubClass   = USB_SUB_CLASS_CDC_ACM,
        .bInterfaceProtocol   = 1, // Common AT
        .iInterface           = 0,

        .class_specific       = (const uint8_t *)&cdc_acm_func_desc,
        .class_specific_len   = USB_CDC_ACM_FUNC_DESC_SIZE,

        .endpoint_descriptors = cdc_acm_comm_endp_desc
    },
    // Data interface
    {
        .bLength              = USB_IFACE_DESC_SIZE,
        .bDescriptorType      = USB_DESC_INTERFACE,
        .bInterfaceNumber     = 1,
        .bAlternateSetting    = 0,
        .bNumEndpoints        = 2,
        .bInterfaceClass      = 0xA, // Data
        .bInterfaceSubClass   = 0,
        .bInterfaceProtocol   = 0,
        .iInterface           = 0,

        .class_specific        = 0,
        .class_specific_len    = 0,

        .endpoint_descriptors = cdc_acm_data_endp_desc
    }
};

static const usb_conf_desc_t cdc_acm_conf_desc[] =
{
    {
        .bLength               = USB_CONF_DESC_SIZE,
        .bDescriptorType       = USB_DESC_CONFIGURATION,
        .wTotalLength          = 0,                      // This field is computed during USB initialization
        .bNumInterfaces        = sizeof(cdc_acm_iface_desc) / sizeof(usb_iface_desc_t),
        .bConfigurationValue   = 1,
        .iConfiguration        = 0,
        .bmAttributes          = 0x80,                   // Bus powered
        .bMaxPower             = 25,                     // 50mA (25 x 2mA)

        .class_specific        = 0,
        .class_specific_len    = 0,

        .interface_descriptors = cdc_acm_iface_desc
    }
};

static const usb_dev_desc_t cdc_acm_dev_desc =
{
    .bLength                   = USB_DEV_DESC_SIZE,
    .bDescriptorType           = USB_DESC_DEVICE,
    .bcdUSB                    = 0x0200,
    .bDeviceClass              = USB_CLASS_CDC,
    .bDeviceSubClass           = 0,                  // Unused
    .bDeviceProtocol           = 0,                  // Unused
    .bMaxPacketSize0           = 64,
    .idVendor                  = 0x0483,             // STMicroelectronics vendor ID
    .idProduct                 = 0x5740,
    .bcdDevice                 = 0x0200,
    .iManufacturer             = 1,
    .iProduct                  = 2,
    .iSerialNumber             = 3,
    .bNumConfigurations        = 1,

    .class_specific            = 0,
    .class_specific_len        = 0,

    .configuration_descriptors = cdc_acm_conf_desc
};

static const usb_dev_qual_desc_t cdc_acm_dev_qual_desc =
{
    .bLength            = USB_DEV_QUAL_DESC_SIZE,
    .bDescriptorType    = USB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = USB_CLASS_CDC,
    .bDeviceSubClass    = 0,
    .bDeviceProtocol    = 0,
    .bMaxPacketSize0    = 64,
    .bNumConfigurations = 1,
    .bReserved          = 0,
};

static const uint16_t cdc_acm_langid_string[]  = {0x0409};
static const uint16_t cdc_acm_vendor_string[]  = {'H', 'i', 'K', 'o', 'B'};
static const uint16_t cdc_acm_product_string[] = {'A', 'g', 'i', 'l', 'e', ' ', 'F', 'o', 'x'};
static uint16_t cdc_acm_serial_string[48];

static const usb_string_desc_t cdc_acm_string_desc[] =
{
    // Lang ID
    {
        .bLength = 2 + sizeof(cdc_acm_langid_string),
        .bDescriptorType = USB_DESC_STRING,
        .wData = cdc_acm_langid_string
    },
    // Manufacturer
    {
        .bLength = 2 + sizeof(cdc_acm_vendor_string),
        .bDescriptorType = USB_DESC_STRING,
        .wData = cdc_acm_vendor_string
    },
    // Product
    {
        .bLength = 2 + sizeof(cdc_acm_product_string),
        .bDescriptorType = USB_DESC_STRING,
        .wData = cdc_acm_product_string
    },
    // Serial
    {
        .bLength = 2 + sizeof(cdc_acm_serial_string),
        .bDescriptorType = USB_DESC_STRING,
        .wData = cdc_acm_serial_string
    }
};

typedef struct
{
    uint32_t dwDTERate;
    uint8_t bCharFormat;
    uint8_t bParityType;
    uint8_t bDataBits;
} __attribute__((packed)) cdc_acm_line_coding_t;

/***** Public methods *****/

static uint8_t tx_buf[128] __attribute__((aligned(4))), rx_buf[128] __attribute__((aligned(4)));
static uint16_t tx_len = 0, tx_in = 0, tx_out = 0;//, rx_in = 0, rx_out = 0;
static bool tx_empty = true;
static cdc_acm_data_callback_t rx_cb = NULL;

void cdc_acm_register_rx_callback(cdc_acm_data_callback_t cb_rx)
{
    rx_cb = cb_rx;
}

uint16_t cdc_acm_send(uint8_t *buf, uint16_t len)
{
    uint16_t i = 0;

    if(tx_empty || (tx_in != tx_out))
    {
        for(i = 0; i < len; i++)
        {
            tx_buf[tx_in] = buf[i];
            tx_in++;

            if(tx_in >= sizeof(tx_buf))
            {
                tx_in = 0;
            }

            if(tx_in == tx_out)
            {
                i++;
                break;
            }
        }
    }

    if(tx_empty)
    {
        tx_empty = false;

        if(tx_in > tx_out)
        {
            tx_len = tx_in - tx_out;
        }
        else
        {
            tx_len = sizeof(tx_buf) - tx_out;
        }

        if(tx_len > usb_get_max_packet_size(2))
        {
            tx_len = usb_get_max_packet_size(2);
        }

        usb_send(2, true, NO_CHANGE, &(tx_buf[tx_out]), tx_len);
    }

    return i;
}

/***** Global usb peripheral behavior *****/

static void cdc_acm_class_interface(usb_device_request_t *req);

const usb_profile_t cdc_acm =
{
    .device_descriptor = &cdc_acm_dev_desc,

    .device_qualifier_descriptor = &cdc_acm_dev_qual_desc,

    .number_of_string_descriptors = sizeof(cdc_acm_string_desc) / sizeof(usb_string_desc_t),
    .string_descriptors = cdc_acm_string_desc,

    .standard_interface = usb_default_process_standard_interface,
    .standard_endpoint  = NULL,
    .class_device       = NULL,
    .class_interface    = cdc_acm_class_interface,
    .class_endpoint     = usb_default_process_class_endpoint,
    .vendor_device      = NULL,
    .vendor_interface   = NULL,
    .vendor_endpoint    = NULL
};

static void cdc_acm_comm(uint8_t endp, bool rx, bool tx)
{
    log_error("CDC ACM comm callback (%d, %d)", rx, tx);
}

static void cdc_acm_data_in(uint8_t endp, bool rx, bool tx)
{
    uint16_t len;

    if(rx && !tx)
    {
        if(rx_cb)
        {
            len = usb_get_rx_len(endp);
            usb_read_pma(endp, rx_buf, len);
            rx_cb(rx_buf, len);
        }
        else
        {
            log_warning("No receive callback available");
        }

        usb_set_stat_rx(endp, STAT_RX_VALID);
    }
    else
    {
        log_error("CDC ACM data in callback called with: rx = %d and tx = %d", rx, tx);
    }
}

static void cdc_acm_data_out(uint8_t endp, bool rx, bool tx)
{
    if(tx && !rx)
    {
        tx_out += tx_len;

        if(tx_out >= sizeof(tx_buf))
        {
            tx_out = 0;
        }

        if(tx_out == tx_in)
        {
            tx_empty = true;
        }
        else
        {
            if(tx_in > tx_out)
            {
                tx_len = tx_in - tx_out;
            }
            else
            {
                tx_len = sizeof(tx_buf) - tx_out;
            }

            if(tx_len > usb_get_max_packet_size(endp))
            {
                tx_len = usb_get_max_packet_size(endp);
            }

            usb_send(2, true, NO_CHANGE, &(tx_buf[tx_out]), tx_len);
        }
    }
    else
    {
        log_error("CDC ACM data out callback called with: rx = %d and tx = %d", rx, tx);
    }
}

static bool set_line_coding(uint8_t endp)
{
    cdc_acm_line_coding_t line_coding;
    uint16_t len;

    len = usb_get_rx_len(0);

    if(len == 7)
    {
        usb_read_pma(0, (uint8_t *)&line_coding, 7);

        log_info("Bit rate: %d", line_coding.dwDTERate);

        switch(line_coding.bCharFormat)
        {
            case 0:
                log_info("Char format: 1 stop bit");
                break;
            case 1:
                log_info("Char format: 1.5 stop bits");
                break;
            case 2:
                log_info("Char format: 2 stop bits");
                break;
            default:
                log_error("Unknown char format");
        }

        switch(line_coding.bParityType)
        {
            case 0:
                log_info("Parity: None");
                break;
            case 1:
                log_info("Parity: Odd");
                break;
            case 2:
                log_info("Parity: Even");
                break;
            case 3:
                log_info("Parity: Mark");
                break;
            case 4:
                log_info("Parity: Space");
                break;
            default:
                log_error("Unknown parity type");
        }

        log_info("Data bits: %d", line_coding.bDataBits);
    }
    else
    {
        log_error("Wrong line coding data size");
    }

    // Start status stage
    usb_send_status(0, true);

    return true;
}

static void cdc_acm_class_interface(usb_device_request_t *req)
{
    uint16_t interface;
    uint16_t alternate;
    usb_get_interface(&interface, &alternate);

    switch(req->bRequest)
    {
        case SET_LINE_CODING:

            if(req->wLength != 7)
            {
                log_error("Wrong line coding structure size");
            }

            // Prepare for the incoming data stage
            usb_set_next_data_callback(0, set_line_coding);
            usb_start_stage(0, false);
            break;

        case GET_LINE_CODING:

            if(req->wLength != 7)
            {
                log_error("Wrong line coding structure size");
            }

            // TODO
            log_not_implemented("GET_LINE_CODING");
            break;

        case SET_CONTROL_LINE_STATE:

            if(req->wValue & 0x1)
            {
                log_info("DTE is present");
            }
            else
            {
                log_info("DTE is not present");
            }

            if(req->wValue & 0x2)
            {
                log_info("Carrier control activated");
            }
            else
            {
                log_info("Carrier control deactivated");
            }

            // Start status stage
            usb_send_status(0, true);
            break;

        default:
            log_error("Unknown request: 0x%X", req->bRequest);
            break;
    }
}
