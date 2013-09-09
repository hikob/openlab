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
 * hid.c
 *
 *  Created on: Aug 30, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "usb.h"
#include "libusb.h"
#include "hid.h"

#define NO_DEBUG_HEADER
#define LOG_LEVEL LOG_LEVEL_WARNING
#include "printf.h"
#include "debug.h"

#if (LOG_LEVEL < LOG_LEVEL_DEBUG)
#define DBG(x...)   printf(x)
#else
#define DBG(x...)   do { } while(0)
#endif // LOG_LEVEL

/* ************************************************************ */
/* ************************************************************ */
/* ************************************************************ */

//HID
#define usb_VendorId   0x046D
#define usb_ProductId  0xC063

/* ************************************************************ */
/* ************************************************************ */
/* ************************************************************ */

#define HID_EP                     1
#define HID_EP_ADDR                USB_ENDPOINT1_IN
#define HID_EP_SIZE                8

#define ENDPOINT_0                 0
#define ENDPOINT_0_SIZE            64


/* ************************************************************ */
/* ************************************************************ */
/* ************************************************************ */

/***** Descriptors *****/

/* endpoints handler forward declarations */
static void hid_endpoint_callback(uint8_t endp, bool rx, bool tx);

/* class handler forward declaration */
static void hid_standard_interface(usb_device_request_t *req);
static void hid_class_interface(usb_device_request_t *req);

static const usb_endp_desc_t hid_endp_desc[] =
{
    {
        .bLength            = USB_ENDP_DESC_SIZE,
        .bDescriptorType    = USB_DESC_ENDPOINT,
        .bEndpointAddress   = HID_EP_ADDR,
        .bmAttributes       = USB_ENDPOINT_INTERRUPT,
        .wMaxPacketSize     = HID_EP_SIZE,
        .bInterval          = 10, // 10 ms
        .class_specific     = 0,
        .class_specific_len = 0,
        .endpoint_callback  = hid_endpoint_callback
    }
};

static const uint8_t hid_report_descriptor[] =
{
    /* Usage Page (Generic Desktop)         */ 0x05, 0x01,
    /* Usage (Mouse)                        */ 0x09, 0x02,
    /*   Collection (Application)           */ 0xA1, 0x01,
    /*     Usage (Pointer)                  */ 0x09, 0x01,
    /*     Collection (Physical)            */ 0xA1, 0x00,
    /*       Usage Page (Buttons)           */ 0x05, 0x09,
    /*       Usage Minimum (1)              */ 0x19, 0x01,
    /*       Usage Maximum (3)              */ 0x29, 0x03,
    /*       Logical Minimum (0)            */ 0x15, 0x00,
    /*       Logical Maximum (1)            */ 0x25, 0x01,
    /*       Report Count (3)               */ 0x95, 0x03,
    /*       Report Size (1)                */ 0x75, 0x01,
    /*       Input (Data, Variable, Absolute) ; 3 button bits*/ 0x81, 0x02,
    /*       Report Count (1)               */ 0x95, 0x01,
    /*       Report Size (5)                */ 0x75, 0x05,
    /*       Input (Constant) ; 5 bit padding */ 0x81, 0x01,
    /*       Usage Page (Generic Desktop)   */ 0x05, 0x01,
    /*       Usage (X)                      */ 0x09, 0x30,
    /*       Usage (Y)                      */ 0x09, 0x31,
    /*       Logical Minimum (-127)         */ 0x15, 0x81,
    /*       Logical Maximum (127)          */ 0x25, 0x7F,
    /*       Report Size (8)                */ 0x75, 0x08,
    /*       Report Count (2)               */ 0x95, 0x02,
    /*       Input (Data, Variable, Relative) ; 2 position bytes (X & Y) */ 0x81, 0x06,
    /*     End Collection                   */ 0xC0,
    /*   End Collection                     */ 0xC0
};

static const struct
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdHID;
    uint8_t  bCountryCode;
    uint8_t  bNumDescriptors;
    uint8_t  bClassDescriptorType;
    uint16_t wDescriptorLength;
} __attribute__((packed)) hid_descriptor =
{
    .bLength              = 9,
    .bDescriptorType      = 0x21,
    .bcdHID               = 0x101,
    .bCountryCode         = 0,
    .bNumDescriptors      = 1,
    .bClassDescriptorType = 0x22,
    .wDescriptorLength    = sizeof(hid_report_descriptor)
};

#define DFU_INTERFACE        1
#define DFU_INTERFACE_STRING 4

static const usb_iface_desc_t hid_iface_desc[] =
{
    {
        .bLength              = USB_IFACE_DESC_SIZE,
        .bDescriptorType      = USB_DESC_INTERFACE,
        .bInterfaceNumber     = 0,
        .bAlternateSetting    = 0,
        .bNumEndpoints        = 1,
        .bInterfaceClass      = USB_CLASS_HID,
        .bInterfaceSubClass   = 0, // 0 = No subclass, 1 = Boot interface subclass
        .bInterfaceProtocol   = 2, // 0 = None, 1 = Keyboard, 2 = Mouse
        .iInterface           = 0,
        .class_specific       = (const uint8_t *) &hid_descriptor,
        .class_specific_len   = sizeof(hid_descriptor),
        .endpoint_descriptors = hid_endp_desc
    },
    // DFU Interface (bInterfaceNumber, iInterface)
//    DFU_RUNTIME_INTERFACE(DFU_INTERFACE, DFU_INTERFACE_STRING)
};

static const usb_conf_desc_t hid_conf_desc[] =
{
    {
        .bLength               = USB_CONF_DESC_SIZE,
        .bDescriptorType       = USB_DESC_CONFIGURATION,
        .wTotalLength          = 0,                      // This field is computed during USB initialization
        .bNumInterfaces        = sizeof(hid_iface_desc) / sizeof(usb_iface_desc_t),
        .bConfigurationValue   = 1,
        .iConfiguration        = 0,
        .bmAttributes          = 0x80,                   // Bus powered
        .bMaxPower             = 25,                     // 50mA (25 x 2mA)
        .class_specific        = 0,
        .class_specific_len    = 0,
        .interface_descriptors = hid_iface_desc
    }
};

static const usb_dev_desc_t hid_dev_desc =
{
    .bLength                   = USB_DEV_DESC_SIZE,
    .bDescriptorType           = USB_DESC_DEVICE,
    .bcdUSB                    = 0x0110,
    .bDeviceClass              = 0,
    .bDeviceSubClass           = 0,                  // Unused
    .bDeviceProtocol           = 0,                  // Unused
    .bMaxPacketSize0           = ENDPOINT_0_SIZE,
    .idVendor                  = usb_VendorId,
    .idProduct                 = usb_ProductId,
    .bcdDevice                 = 0x0100,
    .iManufacturer             = 1,
    .iProduct                  = 2,
    .iSerialNumber             = 3,
    .bNumConfigurations        = 1,
    .class_specific            = 0,
    .class_specific_len        = 0,
    .configuration_descriptors = hid_conf_desc
};

static const usb_dev_qual_desc_t hid_dev_qual_desc =
{
    .bLength            = USB_DEV_QUAL_DESC_SIZE,
    .bDescriptorType    = USB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = USB_CLASS_CDC,
    .bDeviceSubClass    = 0,
    .bDeviceProtocol    = 0,
    .bMaxPacketSize0    = ENDPOINT_0_SIZE,
    .bNumConfigurations = 1,
    .bReserved          = 0,
};

const usb_profile_t usb_hid =
{
    .device_descriptor            = &hid_dev_desc,
    .device_qualifier_descriptor  = &hid_dev_qual_desc,
    .number_of_string_descriptors = &usb_string_desc_size,
    .string_descriptors           =  usb_string_desc,

    .standard_interface = hid_standard_interface,
    .standard_endpoint  = NULL,
    .class_device       = NULL,
    .class_interface    = hid_class_interface,
    .class_endpoint     = NULL,
    .vendor_device      = NULL,
    .vendor_interface   = NULL,
    .vendor_endpoint    = NULL
};

/* ************************************************************ */
/* ************************************************************ */
/* ************************************************************ */

/* Global variables */

/* ************************************************************ */
/* **** Public Functions ************************************** */
/* ************************************************************ */

void hid_send(int8_t x, int8_t y, bool button1, bool button2, bool button3)
{
    static uint8_t buf[3];

    buf[0] = (button1 ? 0x1 : 0x0) | (button2 ? 0x2 : 0x0) | (button3 ? 0x4 : 0x0);
    buf[1] = x;
    buf[2] = y;

    usb_send(hid_endp_desc[0].bEndpointAddress & 0x7F, true, NO_CHANGE, buf, 3);
}

/* ************************************************************ */
/* ************************************************************ */
/* ************************************************************ */

/* Endpoints handlers */


static void hid_endpoint_callback(uint8_t endp, bool rx, bool tx)
{
    if (!rx && tx)
    {
    }
    else
    {
        log_error("HID callback called with: rx = %d and tx = %d", rx, tx);
    }
}

/* Device Class handlers */

static void hid_standard_interface(usb_device_request_t *req)
{
    switch (req->bRequest)
    {
        case 0x06: // GET_DESCRIPTOR

            switch (req->wValue >> 8)
            {
                case 0x21: // HID
                    log_not_implemented("HID");
                    break;

                case 0x22: // Report
                    usb_send(0, true, DATA1, hid_report_descriptor, sizeof(hid_report_descriptor));
                    break;

                case 0x23: // Physical descriptor
                    log_not_implemented("Physical descriptor");
                    break;

                default:
                    log_error("Wrong descriptor request: 0x%04X", req->wValue);
            }

            break;

        default:
            log_warning("Standard interface not handled: bRequest = 0x%02X bmRequestType = 0x%02X wValue = 0x%04X wIndex = 0x%04X wLength = 0x%04X", 
			req->bRequest, req->bmRequestType, req->wValue, req->wIndex, req->wLength);
    }
}

static void hid_class_interface(usb_device_request_t *req)
{
    uint16_t interface;
    uint16_t alternate;

    usb_get_interface(&interface, &alternate);

    if (interface == DFU_INTERFACE)
    {
        usb_dfu_class_interface(req);
        return;
    }

    switch (req->bRequest)
    {
        case 0x01: // GET_REPORT (mandatory)
            log_warning("GET_REPORT: Report Type=0x%02X Report ID=0x%02X Interface=%d", req->wValue >> 8, req->wValue & 0xFF, req->wIndex);
            usb_send(0, true, DATA1, hid_report_descriptor, req->wLength);
            break;

        case 0x02: // GET_IDLE
            log_warning("GET_IDLE");
            break;

        case 0x03: // GET_PROTOCOL (only for boot devices)
            log_warning("GET_PROTOCOL");
            break;

        case 0x09: // SET_REPORT
            log_warning("SET_REPORT");
            break;

        case 0x0A: // SET_IDLE
            log_warning("SET_IDLE: Duration=%d Report ID=0x%02X Interface=%d", req->wValue >> 8, req->wValue & 0xFF, req->wIndex);

            // Start status stage
            usb_send_status(0, true);
            break;

        case 0x0B: // SET_PROTOCOL (only for boot devices)
            log_warning("SET_PROTOCOL");
            break;

        default:
            log_error("Wrong bRequest field: 0x%02X", req->bRequest);
    }
}

/* ************************************************************ */
/* ************************************************************ */
/* ************************************************************ */
