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
 * dfu.c
 *
 *  Created on: Nov 21, 2011
 *      Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#include "usb.h"
#include "libusb.h"

#include "dfu.h"
#include "storage.h"

#define NO_DEBUG_HEADER
#define LOG_LEVEL LOG_LEVEL_DEBUG
#include "printf.h"
#include "debug.h"

/* ************************************************************ */
/* ************************************************************ */
/* ************************************************************ */

//MSC
#define usb_VendorId   0x0483
#define usb_ProductId  0x5740

/* ************************************************************ */
/* ************************************************************ */
/* ************************************************************ */

/* DFU requests (Section 3, table 3.2) */
typedef enum
{
    DFU_REQ_DETACH                 = 0x00,
    DFU_REQ_DNLOAD                 = 0x01,
    DFU_REQ_UPLOAD                 = 0x02,
    DFU_REQ_GETSTATUS              = 0x03,
    DFU_REQ_CLRSTATUS              = 0x04,
    DFU_REQ_GETSTATE               = 0x05,
    DFU_REQ_ABORT                  = 0x06
} usb_dfu_request_t;

/* DFU status (Section 6) */
typedef struct
{
    uint8_t bStatus;
    uint8_t bwPollTimeout[3];
    uint8_t bState;
    uint8_t iString;
} __attribute__((packed)) usb_dfu_status_t;

/* DFU status code (Section 6) */
typedef enum
{
    DFU_STATUS_OK                  = 0x00,
    DFU_STATUS_errTARGET           = 0x01,
    DFU_STATUS_errFILE             = 0x02,
    DFU_STATUS_errWRITE            = 0x03,
    DFU_STATUS_errERASE            = 0x04,
    DFU_STATUS_errCHECK_ERASED     = 0x05,
    DFU_STATUS_errPROG             = 0x06,
    DFU_STATUS_errVERIFY           = 0x07,
    DFU_STATUS_errADDRESS          = 0x08,
    DFU_STATUS_errNOTDONE          = 0x09,
    DFU_STATUS_errFIRMWARE         = 0x0a,
    DFU_STATUS_errVENDOR           = 0x0b,
    DFU_STATUS_errUSBR             = 0x0c,
    DFU_STATUS_errPOR              = 0x0d,
    DFU_STATUS_errUNKNOWN          = 0x0e,
    DFU_STATUS_errSTALLEDPKT       = 0x0f
} usb_dfu_statuscode_t;

/* DFU state (Section 6) */
typedef enum
{
    DFU_STATE_appIDLE              = 0x00,
    DFU_STATE_appDETACH        = 0x01,
    DFU_STATE_dfuIDLE              = 0x02,
    DFU_STATE_dfuDNLOAD_SYNC       = 0x03,
    DFU_STATE_dfuDNBUSY            = 0x04,
    DFU_STATE_dfuDNLOAD_IDLE       = 0x05,
    DFU_STATE_dfuMANIFEST_SYNC     = 0x06,
    DFU_STATE_dfuMANIFEST          = 0x07,
    DFU_STATE_dfuMANIFEST_WAIT_RST = 0x08,
    DFU_STATE_dfuUPLOAD_IDLE       = 0x09,
    DFU_STATE_dfuERROR         = 0x0a
} usb_dfu_state_t;


/* ************************************************** */
/* DFU Descriptors                                    */
/*    - RunTime Mode                                  */
/* ************************************************** */

/* DFU attributes (Section 4, table 4.2) */
#define DFU_CAN_DOWNLOAD             0x01
#define DFU_CAN_UPLOAD               0x02
#define DFU_MANIFEST_TOL             0x04

#define DFU_DETACH_TIMEOUT         0xff00 // ms

const usb_dfu_func_desc_t dfu_func_desc[] =
{
    {
        .bLength            = 0x09,
        .bDescriptorType    = 0x21,  // DFU Functionnal
        .bmAttributes       = DFU_CAN_UPLOAD | DFU_CAN_DOWNLOAD,
        .wDetachTimeOut     = DFU_DETACH_TIMEOUT, // ms
        .wMaxPacketSize     = 256,   // up to 2048 | flash page size ?
        .bcdDFUVersion      = 0x0100 // DFU 1.0
    }
};

/* ************************************************** */
/* DFU Descriptors                                    */
/*    - DFU Mode                                      */
/* ************************************************** */

static const usb_iface_desc_t dfu_iface_desc[] =
{
    {
        .bLength              = USB_IFACE_DESC_SIZE,
        .bDescriptorType      = USB_DESC_INTERFACE,
        .bInterfaceNumber     = 0,
        .bAlternateSetting    = 0,  // memory segment description
        .bNumEndpoints        = 0,
        .bInterfaceClass      = USB_CLASS_APP_SPEC,
        .bInterfaceSubClass   = 1,  // = 1, Device Firmware Upgrade
        .bInterfaceProtocol   = 0,  // = 0, must not use a class specific protocol on this interface
        .iInterface           = 4,  // DFU description string

        .class_specific       = 0,
        .class_specific_len   = 0,

        .endpoint_descriptors = (const usb_endp_desc_t *)dfu_func_desc
    }
};

static const usb_conf_desc_t dfu_conf_desc[] =
{
    {
        .bLength               = USB_CONF_DESC_SIZE,
        .bDescriptorType       = USB_DESC_CONFIGURATION,
        .wTotalLength          = 0,                      // This field is computed during USB initialization
        .bNumInterfaces        = 1,
        .bConfigurationValue   = 1,
        .iConfiguration        = 0,
        .bmAttributes          = 0x80,                   // Bus powered
        .bMaxPower             = 25,                     // 50mA (25 x 2mA)
        .class_specific        = 0,
        .class_specific_len    = 0,
        .interface_descriptors = dfu_iface_desc
    }
};

/*
 *  Product ID note
 *  ===============
 *  from http://www.linux-usb.org/usb.ids
 *  VID: 0483  SGS Thomson Microelectronics
 *       PID: df11  STM Device in DFU Mode
 *
 *  from DFU documentation
 *   - add 1 to VID
 *   - set the high bit
 *   - set to 0xffff
 */

static const usb_dev_desc_t dfu_dev_desc =
{
    .bLength                   = USB_DEV_DESC_SIZE,
    .bDescriptorType           = USB_DESC_DEVICE,
    .bcdUSB                    = 0x0100,
    .bDeviceClass              = USB_CLASS_APP_SPEC,
    .bDeviceSubClass           = 1,        // Device Firmware Upgrade Code
    .bDeviceProtocol           = 0,        // Unused
    .bMaxPacketSize0           = 64,
    .idVendor                  = usb_VendorId,
    .idProduct                 = 0xFFFF,   // not  regular device ID
    .bcdDevice                 = 0x0100,
    .iManufacturer             = 1,
    .iProduct                  = 2,
    .iSerialNumber             = 3,
    .bNumConfigurations        = 1,
    .class_specific            = 0,
    .class_specific_len        = 0,
    .configuration_descriptors = dfu_conf_desc
};


static const usb_dev_qual_desc_t dfu_mode_dev_qual_desc =
{
    .bLength            = USB_DEV_QUAL_DESC_SIZE,
    .bDescriptorType    = USB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = USB_CLASS_APP_SPEC,
    .bDeviceSubClass    = 0,
    .bDeviceProtocol    = 0,
    .bMaxPacketSize0    = 64,
    .bNumConfigurations = 1,
    .bReserved          = 0,
};

static void dfu_mode_standard_interface(usb_device_request_t *req);
static void dfu_mode_class_interface(usb_device_request_t *req);

const usb_profile_t usb_dfu =
{
    .device_descriptor            = &dfu_dev_desc,
    .device_qualifier_descriptor  = &dfu_mode_dev_qual_desc,
    .number_of_string_descriptors = &usb_string_desc_size,
    .string_descriptors           =  usb_string_desc,

    .standard_interface = dfu_mode_standard_interface,
    .standard_endpoint  = NULL,
    .class_device       = NULL,
    .class_interface    = dfu_mode_class_interface,
    .class_endpoint     = NULL,
    .vendor_device      = NULL,
    .vendor_interface   = NULL,
    .vendor_endpoint    = NULL
};


/* ************************************************** */
/* DFU global variables                               */
/*                                                    */
/* ************************************************** */

/**
 * minimum time, in ms, that the host should wait
 * before sending a subsequent DFU_GETSTATUS request.
 * \todo this value must be set according to the current
 * state of the device (mass erase / ... )
 */
#define GET_STATUS_TIMEOUT  500

static uint8_t  usb_dfu_state             = DFU_STATE_appIDLE;
static uint8_t  usb_dfu_state_next        = DFU_STATE_appIDLE;
static uint32_t usb_dfu_getstatus_timeout = GET_STATUS_TIMEOUT;

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

void usb_dfu_init()
{
    usb_dfu_state             = DFU_STATE_appIDLE;
    usb_dfu_state_next        = DFU_STATE_appIDLE;
    usb_dfu_getstatus_timeout = GET_STATUS_TIMEOUT;

    //! \todo Select the internal clock to have a 1kHz tick
    // timer_select_internal_clock(tim6, (timer_get_frequency(tim6) / 1000) - 1);
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

void usb_dfu_detach_timer_cb(handler_arg_t arg, uint16_t counter)
{
    // arg and counter are not used
    (void) arg;
    (void) counter;

    log_error("DFU detach timer timeout, go back to appIDLE");

    // TODO: timer_disable(tim6);

    usb_dfu_state             = DFU_STATE_appIDLE;
    usb_dfu_state_next        = DFU_STATE_appIDLE;
    usb_dfu_getstatus_timeout = GET_STATUS_TIMEOUT;
}

void usb_dfu_class_interface(usb_device_request_t *req)
{
    switch (req->bRequest)
    {
        case DFU_REQ_DETACH:
        {
            uint16_t wTimeout = req->wValue;
            log_info("DFU_DETACH");
            log_info("wTimeout %d, starting timer", wTimeout);

            if (wTimeout > DFU_DETACH_TIMEOUT)
            {
                log_info("wTimeout abnormaly large %d (> detach timeout = %d)", wTimeout, DFU_DETACH_TIMEOUT);
            }

            /* start timer and wait for USB reset */
            usb_dfu_getstatus_timeout = wTimeout;
            usb_dfu_state = DFU_STATE_appDETACH;
            usb_dfu_state_next = DFU_STATE_appDETACH;
            // TODO: timer_enable(tim6);
            // TODO: timer_start(tim6, wTimeout, usb_dfu_detach_timer_cb, NULL);
            usb_send_status(0, true);
        }
        break;
        case DFU_REQ_DNLOAD:
            log_error("DFU_DNLOAD");
            break;
        case DFU_REQ_UPLOAD:
            log_error("DFU_UPLOAD");
            break;
        case DFU_REQ_GETSTATUS:
            log_info("DFU_GETSTATUS ");
            {
                usb_dfu_status_t status;
                status.bStatus = usb_dfu_state;
                status.bwPollTimeout[0] = (usb_dfu_getstatus_timeout >>  0) & 0xff;
                status.bwPollTimeout[1] = (usb_dfu_getstatus_timeout >>  8) & 0xff;
                status.bwPollTimeout[2] = (usb_dfu_getstatus_timeout >> 16) & 0xff;
                status.bState  = usb_dfu_state_next;
                status.iString = 0;
                usb_send(0, true, DATA1, (const uint8_t*)&status, sizeof(status));
            }
            break;
        case DFU_REQ_CLRSTATUS:
            log_error("DFU_CLRSTATUS");
            break;
        case DFU_REQ_GETSTATE:
            log_error("DFU_GETSTATE");
            break;
        case DFU_REQ_ABORT:
            log_error("DFU_ABORT");
            break;
        default:
            log_error("DFU class interface: unknown request: 0x%X", req->bRequest);
            break;
    }

}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

void usb_dfu_detach_reset_check()
{
    if (usb_dfu_state != DFU_STATE_appDETACH)
    {
        /* reset is real, go back to usb.c */
        return;
    }

    log_error("DFU_DETACH got DETACH reset");
    // TODO: timer_disable(tim6);
    log_info("stopping timer");
    /* change to dfu profile   */
    usb_init(USB_DFU);

    /* return to perform USB reset */
    return ;
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
/*  DFU Mode                                          */
/* ************************************************** */
/* ************************************************** */
/* ************************************************** */

static void dfu_mode_standard_interface(usb_device_request_t *req)
{
    switch (req->bRequest)
    {
        default:
            log_error("DFU mode : interface request not handled: bRequest = 0x%02X bmRequestType = 0x%02X wValue = 0x%04X wIndex = 0x%04X wLength = 0x%04X",
                      req->bRequest, req->bmRequestType, req->wValue, req->wIndex, req->wLength);
            break;
    }
}

static void dfu_mode_class_interface(usb_device_request_t *req)
{
    switch (req->bRequest)
    {
        default:
            log_error("DFU mode : interface request not handled: bRequest = 0x%02X bmRequestType = 0x%02X wValue = 0x%04X wIndex = 0x%04X wLength = 0x%04X",
                      req->bRequest, req->bmRequestType, req->wValue, req->wIndex, req->wLength);
            break;
    }
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
