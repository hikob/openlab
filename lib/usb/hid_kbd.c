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
 * hid_kbd.c
 *
 *  Created on: Jul 5, 2013
 *      Author: Liam Beguin
 *
 *      ref : http://www.usb.org/developers/devclass_docs/HID1_11.pdf
 */

#include "usb.h"
#include "libusb.h"
#include <string.h>


#define NO_DEBUG_HEADER
#define LOG_LEVEL 0
#include "printf.h"
#include "debug.h"

#if (LOG_LEVEL < LOG_LEVEL_DEBUG)
#define DBG(x...)   printf(x)
#else
#define DBG(x...)   do { } while(0)
#endif // LOG_LEVEL

//HID KBD
#define usb_VendorId   0x413C
#define usb_ProductId  0x2003

/* ************************************************************ */
/* ************************************************************ */
/* ************************************************************ */

#define HID_EP      1
#define HID_EP_ADDR USB_ENDPOINT1_IN
#define HID_EP_SIZE 8

#define ENDPOINT_0      0
#define ENDPOINT_0_SIZE 8


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
        .endpoint_callback  = hid_endpoint_callback,
    }
};

static const uint8_t hid_report_descriptor[69] =
{
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0x05, 0x08,                    //   USAGE_PAGE (LEDs)
    0x19, 0x01,                    //   USAGE_MINIMUM (Num Lock)
    0x29, 0x03,                    //   USAGE_MAXIMUM (Scroll Lock)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x03,                    //   REPORT_COUNT (3)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x05,                    //   REPORT_COUNT (5)
    0x91, 0x01,                    //   OUTPUT (Cnst,Ary,Abs)
    0xc0,                          // END_COLLECTION
};

const  hid_descriptor_t hid_descriptor =
{
    .bLength              = 9,
    .bDescriptorType      = 0x21,
    .bcdHID               = 0x101,
    .bCountryCode         = 0,
    .bNumDescriptors      = 1,
    .bClassDescriptorType = 0x22,
    .wDescriptorLength    = sizeof(hid_report_descriptor),
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
        .bInterfaceSubClass   = 1, // 0 = No subclass, 1 = Boot interface subclass
        .bInterfaceProtocol   = 1, // 0 = None, 1 = Keyboard, 2 = Mouse
        .iInterface           = 0,
        .class_specific       = (const uint8_t *) &hid_descriptor,
        .class_specific_len   = sizeof(hid_descriptor),
        .endpoint_descriptors = hid_endp_desc,
    },
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
        .interface_descriptors = hid_iface_desc,
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
    .bcdDevice                 = 0x0000,
    .iManufacturer             = 1,
    .iProduct                  = 2,
    .iSerialNumber             = 3,
    .bNumConfigurations        = 1,
    .class_specific            = 0,
    .class_specific_len        = 0,
    .configuration_descriptors = hid_conf_desc,
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

const usb_profile_t usb_hid_kbd =
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
    .vendor_endpoint    = NULL,
};

/* ************************************************************ */
/* ************************************************************ */
/* ************************************************************ */

/* Global variables */
static uint8_t buf[8] = {0};
static bool state = false;

/* ************************************************************ */
/* **** Public Functions ************************************** */
/* ************************************************************ */


bool kbd_get_state(void){
    return state;
}

/* Fixme this conversion  (ASCII -> HiD) is linked to the configuration on the keyboard (azerty, qwerty, ...)  */
void kbd_send(char letter){

    bool maj = false;

    /* if UPPER CASE change to Lower Case */
    if(letter >= 0x41 && letter <= 0x5A){
        DBG('M');
        letter += 0x20;
        maj = true;
    }
    /* change ASCII letter to the corresponding HID code */
    if(letter >= 0x61 && letter <= 0x7A){
        DBG('m');
        letter -= 0x5D;
    }
    /* NUMBERS  converting ASCII to numbers */
    else if(letter >= 0x30 && letter <= 0x39){
        DBG('N');
        letter -= 0x30;

        /* numbers to HID */
        (letter == 0x30) ? (letter = 0x27) : (letter += 0xB0) ;

    }else{

        switch(letter){
            /*  TAB  */
            case 0x09:
                letter = 0x2B;
                break;

                /* SPACE */
            case 0x20:
                letter = 0x2C;
                break;

                /* BACKSPACE */
            case 0x2A:
                log_debug("BACKSPACE");
                break;

                /* minus */
            case 0x2D:
                letter = 0x56;
                break;

                /**
                 * Other specific characters can be added here
                 **/

            case 255:
                // log_error("\tno char");
            default:
                buf[0] = 0x00;
                letter = 0x00;
                break;
        }
    }

    /* CASE */
    (maj) ? (buf[0] = 0x02) : (buf[0] = 0x00);

    buf[2] = letter;
    // log_not_implemented("buf : %x:%x:%x:%x:%x:%x:%x:%x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);


    usb_send(hid_endp_desc[0].bEndpointAddress & 0x7F, true, NO_CHANGE, buf, 8);
}

/* ************************************************************ */
/* ************************************************************ */
/* ************************************************************ */

/* Endpoints handlers */



static void hid_endpoint_callback(uint8_t endp, bool rx, bool tx)
{
    if (!rx && tx) {}
    else{
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
                    log_warning("GET_REPORT: Report Type=0x%02X Report ID=0x%02X Interface=%d", req->wValue >> 8, req->wValue & 0xFF, req->wIndex);
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

            usb_send(hid_endp_desc[0].bEndpointAddress & 0x7F, true, NO_CHANGE, buf, 8);
            break;

        case 0x02: // GET_IDLE - required for keyboards
            log_warning("GET_IDLE");
            break;

        case 0x03: // GET_PROTOCOL (only for boot devices)
            log_warning("GET_PROTOCOL");
            break;

        case 0x09: // SET_REPORT
            log_warning("SET_REPORT");
            state = true;
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
