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
 * usb.h
 *
 *  Created on: Aug 30, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef USB_H_
#define USB_H_

#include <stdbool.h>
#include <stdint.h>
#include "usb_descriptors.h"

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
    const usb_dev_desc_t *device_descriptor;
    const usb_dev_qual_desc_t *device_qualifier_descriptor;
    uint8_t number_of_string_descriptors;
    const usb_string_desc_t *string_descriptors;

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

void usb_init(const usb_profile_t *profile);

void usb_send(uint8_t endp, bool first_time, usb_data_token_t first_token, const uint8_t *buf, uint16_t len);

void usb_set_next_data_callback(uint8_t endp, usb_data_callback_t cb);

void usb_read_pma(uint8_t endp, uint8_t *buf, uint16_t len);

uint16_t usb_get_max_packet_size(uint8_t endp);

void usb_start_stage(uint8_t endp, bool in);
void usb_send_status(uint8_t endp, bool in);

void usb_get_interface(uint16_t *interface, uint16_t *alternate);
void usb_set_interface(uint16_t  interface, uint16_t  alternate);

void usb_default_process_class_endpoint(usb_device_request_t *req);
void usb_default_process_standard_interface(usb_device_request_t *req);

#endif
