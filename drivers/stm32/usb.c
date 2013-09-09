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
 * usb.c
 *
 *  Created on: Aug 30, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 *              Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#include "platform.h"
#include "rcc.h"
#include "nvic.h"
#include "nvic_.h"
#include "usb.h"
#include "usb_.h"
#include "usb_registers.h"

#define NO_DEBUG_HEADER
#define LOG_LEVEL LOG_LEVEL_INFO
#include "printf.h"
#include "debug.h"

#ifdef INFO_HEADER
#undef INFO_HEADER
#define INFO_HEADER()              printf("\x1b[33m  ")
#endif


#define MIN(a,b) (((a) < (b)) ? (a) : (b))

typedef enum
{
    USB_STATE_ATTACHED,
    USB_STATE_POWERED,
    USB_STATE_DEFAULT,
    USB_STATE_ADDRESS,
    USB_STATE_CONFIGURED,
    USB_STATE_SUSPENDED
} usb_state_t;


/**
 * USB driver global variables
 *
 */


/* local buffer used for endpoint0 : size at least 100 */
#define USB_BUF_SIZE 128
static uint8_t                 usb_local_buf[USB_BUF_SIZE] __attribute__((aligned(4)));

/* USB endpoints configuration and information */
static const uint8_t          *descriptor_buf[8]          = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
static uint16_t                descriptor_len[8]          = {0, 0, 0, 0, 0, 0, 0, 0};
static uint16_t                descriptor_max_pkt_size[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static usb_data_callback_t     data_callback[8]           = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
static usb_endpoint_callback_t endpoint_callback[7]       = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/* USB profile and current configuration / interface and alternative */
static const usb_profile_t    *usb_profile                = NULL;
static uint8_t                 usb_configuration          = 0;
static uint8_t                 usb_interface              = 0;
static uint8_t                 usb_interface_alt          = 0;

/* USB internals */
static uint16_t                memory_start;       // usb memory start in STM internal buffer
static usb_state_t             usb_state;          // fsm state 
static uint8_t                 usb_address;        // usb assigned address


/**
 *
 *
 */

typedef enum
{
    ENDP0_IN,
    ENDP0_OUT,
    ENDP0_SETUP
} endpoint0_callback_t;


/**
 * Initialize and init USB macrocell registers
 *
 * called by the USB driver code,
 * contains platform specific code to make USB device visible.
 *
 * This function is defined "weak" and can
 * be redefined when USB is in use in the platform
 */
__attribute__((weak)) void platform_usb_config()
{
    rcc_apb_enable(RCC_APB_BUS_USB, RCC_APB_BIT_USB);

    *usb_get_CNTR() = USB_CNTR__FRES | USB_CNTR__PDWN;
    *usb_get_BTABLE() = 0;
    *usb_get_ISTR() = 0;

    *usb_get_CNTR() = USB_CNTR__CTRM | USB_CNTR__PMAOVRM | USB_CNTR__ERRM
                      | USB_CNTR__WKUPM | USB_CNTR__SUSPM | USB_CNTR__RESETM;
}

/**
 * Initialize and init USB pinouts
 *
 * called by the USB driver code,
 * contains platform specific code to make USB device visible.
 *
 * This function is defined "weak" and must
 * be redefined when USB is in use in the platform
 */
__attribute__((weak)) void platform_usb_enable() { }


void usb_driver_init( const usb_profile_t *profile )
{
    uint16_t i;

    usb_profile = profile;

    platform_usb_config();

    usb_address = 0;
    usb_configuration = 0;
    usb_interface = 0;
    usb_interface_alt = 0;
    memory_start = 0;
    usb_state = USB_STATE_POWERED;

    for (i = 0; i < sizeof(endpoint_callback) / sizeof(endpoint_callback[0]); i++)
    {
        endpoint_callback[i] = NULL;
    }

    for (i = 0; i < sizeof(data_callback) / sizeof(data_callback[0]); i++)
    {
        data_callback[i] = NULL;
    }
}

inline void usb_set_next_data_callback(uint8_t endp, usb_data_callback_t cb)
{
    data_callback[endp] = cb;
}

static void usb_write_pma(uint8_t addr, const uint8_t *buf, uint16_t len)
{
    uint16_t i;
    uint16_t m;
    volatile uint16_t *packet_memory;
    const uint16_t *data;

    // cast to void to ignore warning: cast increases required alignment of target type
    data          = (const void *)buf; 
    packet_memory = usb_get_packet_memory(*usb_get_ADDRn_TX(addr));

    // upper bound 
    m = (len / 2) + (len & 1); 

    for (i = 0; i < m; i ++)
    {
        *packet_memory = data[i];
        packet_memory += 2;
    }
}

uint16_t usb_write_packet(uint8_t endp, const uint8_t *buf, uint16_t len)
{
    usb_write_pma(endp, buf, len);

    *usb_get_COUNTn_TX(endp) = len;

    usb_set_stat_tx(endp, STAT_TX_VALID);

    return len;
}

static void usb_read_pma(uint8_t endp, uint8_t *buf, uint16_t len)
{
    uint16_t i;
    uint16_t m;
    volatile uint16_t *packet_memory;
    uint16_t *data;

    // cast to void to ignore warning: cast increases required alignment of target type
    data          = (void *)buf; 
    packet_memory = usb_get_packet_memory(*usb_get_ADDRn_RX(endp & 0x7));

    // lower bound 
    m = (len / 2);

    for (i = 0; i < m; i++)
    {
	data[i] = *packet_memory;
	packet_memory += 2;
    }

    if (len & 1)
    {
        buf[len - 1] = (uint8_t) * packet_memory;
    }
}

void usb_recv(uint8_t endp, uint8_t *buf, uint16_t len)
{
    //! \todo do some additional check
    return usb_read_pma(endp,buf,len);
}

uint16_t usb_recv_get_len(uint8_t endp)
{
    return usb_get_rx_len(endp);
}

void usb_recv_set_status(uint8_t endp, stat_bits_t stat)
{
    usb_set_stat_rx(endp, stat);
}

static bool send_next_chunck(uint8_t endp)
{
    usb_send(endp, false, NO_CHANGE, NULL, 0);

    return false;
}

void usb_start_stage(uint8_t endp, bool in)
{
    if (in)
    {
        usb_set_dtog_tx(endp, 1);
        usb_set_stat_tx(endp, STAT_TX_VALID);
        usb_set_stat_rx(endp, STAT_RX_NAK);
    }
    else
    {
        usb_set_dtog_rx(endp, 1);
        usb_set_stat_tx(endp, STAT_TX_NAK);
        usb_set_stat_rx(endp, STAT_RX_VALID);
    }
}

static bool last_chunk_sent(uint8_t endp)
{
    usb_start_stage(endp, false);

    return true;
}

static bool status_stage_ended(uint8_t endp)
{
    // Start receive stage (SETUP or OUT)
    usb_set_dtog_rx(endp, 0);
    usb_set_stat_rx(endp, STAT_RX_VALID);

    return true;
}

void usb_send_status(uint8_t endp, bool in)
{
    if (in)
    {
        // Start with DATA1 token
        usb_set_dtog_tx(endp, 1);

        // Send empty package
        usb_set_next_data_callback(endp, status_stage_ended);
        usb_write_packet(endp, NULL, 0);
    }
    else
    {
        // Start with DATA1 token
        usb_set_dtog_rx(endp, 1);

        // Get ready to receive an empty packet
        usb_set_next_data_callback(endp, status_stage_ended);
        usb_set_stat_tx(endp, STAT_TX_NAK);
        usb_set_stat_rx(endp, STAT_RX_VALID);
    }
}

void usb_send(uint8_t endp, bool first_time, usb_data_token_t first_token, const uint8_t *buf, uint16_t len)
{
    if (first_time)
    {
        descriptor_len[endp] = len;
        descriptor_buf[endp] = buf;

        if (first_token != NO_CHANGE)
        {
            usb_set_dtog_tx(endp, first_token);
        }
    }

    if (descriptor_len[endp] > descriptor_max_pkt_size[endp])
    {
        usb_write_packet(endp, descriptor_buf[endp], descriptor_max_pkt_size[endp]);
        descriptor_buf[endp] += descriptor_max_pkt_size[endp];
        descriptor_len[endp] -= descriptor_max_pkt_size[endp];
        usb_set_next_data_callback(endp, send_next_chunck);
    }
    else
    {
        usb_write_packet(endp, descriptor_buf[endp], descriptor_len[endp]);
        descriptor_buf[endp] = NULL;
        descriptor_len[endp] = 0;
        usb_set_next_data_callback(endp, last_chunk_sent);
    }
}

void usb_send_set_status(uint8_t endp, stat_bits_t stat)
{
    usb_set_stat_tx(endp,stat);
}

uint16_t usb_get_max_packet_size(uint8_t endp)
{
    return descriptor_max_pkt_size[endp & 0x7f];
}

static void usb_endpoint_init(uint8_t endp, usb_endp_type_t type, uint16_t len, usb_endpoint_callback_t callback)
{
    uint8_t dir = endp & 0x80, addr = endp & 0x7F;
    uint16_t l = len;
    uint16_t t;

#if 0
    #warning "USB refactoring :: DFU"
    /* TODO usb_dfu_detach_reset_check(); */
#endif

    switch (type)
    {
        case USB_ENDPOINT_CONTROL:
            t = 0x0200;
            break;

        case USB_ENDPOINT_ISO:
            t = 0x0400;
            break;

        case USB_ENDPOINT_BULK:
            t = 0x0000;
            break;

        case USB_ENDPOINT_INTERRUPT:
            t = 0x0600;
            break;

        default:
            t = 0;
            log_error("usb_endpoint_init:: unknown endpoint type");
            break;
    }

    log_info("Endpoint init: 0x%02x (type: %d), start: %d len: %d, callback: 0x%X", endp, type, memory_start, len, callback);
    if (memory_start >= USB_PMA_SIZE)
    {
	log_error("USB memory exceeded, new start address is at %d",memory_start);
    }

    *usb_get_EPnR(addr) = ((*usb_get_EPnR(addr)) & 0x8980) | t | addr;

    if (type == USB_ENDPOINT_CONTROL)
    {
        *usb_get_ADDRn_TX(addr) = memory_start;
        memory_start += len;
        *usb_get_ADDRn_RX(addr) = memory_start;
        memory_start += len;

        if (l >= 63)
        {
            if (l & 0x1F)
            {
                l += 32;
            }

            l = 0x8000 | ((l & 0xFFE0) << 5);
        }
        else
        {
            if (l & 0x1)
            {
                l++;
            }

            l <<= 9;
        }

        *usb_get_COUNTn_RX(addr) = l;
        *usb_get_COUNTn_TX(addr) = 0;

        usb_set_stat_tx(addr, STAT_TX_NAK);
        usb_set_stat_rx(addr, STAT_RX_VALID);

        usb_set_dtog_tx(addr, 0);
        usb_set_dtog_rx(addr, 0);
    }
    else
    {
        if (dir)
        {
            *usb_get_ADDRn_TX(addr) = memory_start;
            *usb_get_COUNTn_TX(addr) = 0;
            usb_set_stat_rx(addr, STAT_RX_DISABLED);
            usb_set_stat_tx(addr, STAT_TX_NAK);
            usb_set_dtog_tx(addr, 0);
            memory_start += len;
        }
        else
        {
            *usb_get_ADDRn_RX(addr) = memory_start;

            if (l >= 63)
            {
                if (l & 0x1F)
                {
                    l += 32;
                }

                l = 0x8000 | ((l & 0xFFE0) << 5);
            }
            else
            {
                if (l & 0x1)
                {
                    l++;
                }

                l <<= 9;
            }

            *usb_get_COUNTn_RX(addr) = l;

            usb_set_stat_tx(addr, STAT_TX_NAK);
            usb_set_stat_rx(addr, STAT_RX_VALID);
            usb_set_dtog_rx(addr, 0);
            memory_start += len;
        }

        endpoint_callback[addr - 1] = callback;
    }

    descriptor_max_pkt_size[addr] = len;
}

static uint16_t fill_buf(const uint8_t *src, uint16_t start, uint16_t len)
{
    uint16_t i;

    for (i = 0; i < len; i++)
    {
	uint16_t off = start + i;
	if (off >= USB_BUF_SIZE)
	{
	    log_error("usb: endpoint 0 local buffer overflow of %d bytes", USB_BUF_SIZE - (start + len));
	    break;
	}

        usb_local_buf[ off ] = src[i];
    }

    return i;
}

static void fill_buf_string(const usb_string_desc_t *string_descriptor)
{
    usb_local_buf[0] = string_descriptor->bLength;
    usb_local_buf[1] = string_descriptor->bDescriptorType;
    fill_buf((const uint8_t *)string_descriptor->wData, 2, string_descriptor->bLength);
}

static void fill_buf_configuration(const usb_conf_desc_t *configuration_descriptor)
{
    uint16_t i, j, p;
    const usb_iface_desc_t *interface;
    const usb_endp_desc_t *endpoint;
    uint16_t wTotalLength = 0;
    usb_conf_desc_t *conf = (usb_conf_desc_t *)usb_local_buf;

    wTotalLength = configuration_descriptor->bLength;

    // Copy configuration descriptor
    p = fill_buf((const uint8_t *)configuration_descriptor, 0, configuration_descriptor->bLength);

    // If there is a class or vendor specific configuration decriptor, append it after the descriptor
    if (configuration_descriptor->class_specific_len != 0)
    {
        wTotalLength += configuration_descriptor->class_specific_len;
        p += fill_buf(configuration_descriptor->class_specific, p, configuration_descriptor->class_specific_len);
    }

    for (i = 0; i < configuration_descriptor->bNumInterfaces; i++)
    {
        interface = &(configuration_descriptor->interface_descriptors[i]);
        wTotalLength += interface->bLength;

        // Copy interface descriptor #i
        p += fill_buf((const uint8_t *)interface, p, interface->bLength);

        // If there is a class or vendor specific interface descriptor, append it after the descriptor
        if (interface->class_specific_len != 0)
        {
            wTotalLength += interface->class_specific_len;
            p += fill_buf(interface->class_specific, p, interface->class_specific_len);
        }

        for (j = 0; j < interface->bNumEndpoints; j++)
        {
            endpoint = &(interface->endpoint_descriptors[j]);
            wTotalLength += endpoint->bLength;

            // Copy endpoint descriptor #j of endpoint #i
            p += fill_buf((const uint8_t *)endpoint, p, endpoint->bLength);

            // If there is a class or vendor specific endpoint descriptor, append it after the descriptor
            if (endpoint->class_specific_len != 0)
            {
                wTotalLength += endpoint->class_specific_len;
                p += fill_buf(endpoint->class_specific, p, endpoint->class_specific_len);
            }
        }
    }

    conf->wTotalLength = wTotalLength;

}

static void register_enpoint_callbacks()
{
    uint16_t i, j, k;
    const usb_conf_desc_t *configuration;
    const usb_iface_desc_t *interface;
    const usb_endp_desc_t *endpoint;

    for (i = 0; i < usb_profile->device_descriptor->bNumConfigurations; i++)
    {
        configuration = &(usb_profile->device_descriptor->configuration_descriptors[i]);

        for (j = 0; j < configuration->bNumInterfaces; j++)
        {
            interface = &(configuration->interface_descriptors[j]);

            for (k = 0; k < interface->bNumEndpoints; k++)
            {
                endpoint = &(interface->endpoint_descriptors[k]);

                usb_endpoint_init(endpoint->bEndpointAddress, endpoint->bmAttributes, endpoint->wMaxPacketSize, endpoint->endpoint_callback);
            }
        }
    }
}

static void select_configuration(uint16_t index)
{
    log_info("SET_CONFIGURATION (%d)", index);
    usb_configuration = index;
}

bool address_received(uint8_t endp)
{
    *usb_get_DADDR() = 0x80 | usb_address;
    usb_state = USB_STATE_ADDRESS;

    register_enpoint_callbacks();

    return true;
}

void usb_set_interface(uint16_t interface, uint16_t alternate)
{
    log_info("Standard interface, set interface %d, alternate config %d", interface, alternate);
    usb_interface     = interface;
    usb_interface_alt = alternate;
}

void usb_get_interface(uint16_t *interface, uint16_t *alternate)
{
    // This function is called quite often, the following log is very verbose !
    // log_info("Standard interface, get interface %d, alternate config %d", usb_interface, usb_interface_alt);
    *interface = usb_interface;
    *alternate = usb_interface_alt;
}

void usb_default_process_class_endpoint(usb_device_request_t *req)
{
    switch (req->bRequest)
    {
        default:
            log_warning("Class endpoint request not handled: bRequest = 0x%02X "
                        "bmRequestType = 0x%02X wValue = 0x%04X wIndex = 0x%04X wLength = 0x%04X",
                        req->bRequest, req->bmRequestType, req->wValue, req->wIndex, req->wLength);
            break;
    }
}

void usb_default_process_standard_interface(usb_device_request_t *req)
{
    switch (req->bRequest)
    {
        case 0x0B: // SET_INTERFACE
            usb_set_interface(req->wIndex, req->wValue);
            usb_send_status(0, true);
            break;

        default:
            log_warning("Standard interface not handled: bRequest = 0x%02X "
                        "bmRequestType = 0x%02X wValue = 0x%04X wIndex = 0x%04X wLength = 0x%04X",
                        req->bRequest, req->bmRequestType, req->wValue, req->wIndex, req->wLength);
            break;
    }
}

static void process_standard_device(usb_device_request_t *header)
{
    uint8_t idx;

    switch (header->bRequest)
    {
        case 0x00: // GET_STATUS
            log_info("GET_STATUS");

            switch (header->bmRequestType)
            {
                case 0x80:
                case 0x81:
                case 0x82:
                    usb_local_buf[0] = 0;
                    usb_local_buf[1] = 0;
                    usb_send(0, true, 1, usb_local_buf, 2);
                    break;

                default:
                    log_error("unknown bmRequestType");
            }

            break;

        case 0x01: // CLEAR_FEATURE
            log_error("CLEAR_FEATURE not implemented");
            break;

        case 0x03: // SET_FEATURE
            log_error("SET_FEATURE not implemented");
            break;

        case 0x05: // SET_ADDRESS
            usb_address = header->wValue & 0xFF;

            log_info("SET_ADDRESS: 0x%02X (%d)", usb_address, usb_address);

            // Start status stage
            usb_send_status(0, true);

            usb_set_next_data_callback(0, address_received);
            return;

        case 0x06: // GET_DESCRIPTOR
            idx = header->wValue & 0xFF;

            switch (header->wValue >> 8)
            {
                case 0x01: // DEVICE_DESCRIPTOR
                    log_info("GET_DESCRIPTOR => DEVICE_DESCRIPTOR (%d)", idx);

                    // The next transaction will be IN so we write the device descriptor as requested
                    if (usb_profile->device_descriptor->class_specific_len == 0)
                    {
                        //usb_send(0, true, DATA1, (const uint8_t *)(usb_profile->device_descriptor), USB_DEV_DESC_SIZE);
                        usb_send(0, true, DATA1, (const uint8_t *)(usb_profile->device_descriptor), header->wLength);
                    }
                    else
                    {
                        // If there is class or vendor specific data, append them after the descriptor
                        fill_buf((const uint8_t *)(usb_profile->device_descriptor), 0, USB_DEV_DESC_SIZE);
                        fill_buf(usb_profile->device_descriptor->class_specific, USB_DEV_DESC_SIZE, usb_profile->device_descriptor->class_specific_len);
                        usb_send(0, true, DATA1, usb_local_buf, USB_DEV_DESC_SIZE + usb_profile->device_descriptor->class_specific_len);
                    }

                    break;

                case 0x02: // CONFIG_DESCRIPTOR
                    log_info("GET_DESCRIPTOR => CONFIG_DESCRIPTOR (%d)", idx);

                    // The next transaction will be IN so we write the configuration descriptors as requested
                    if (idx < usb_profile->device_descriptor->bNumConfigurations)
                    {
                        fill_buf_configuration(&(usb_profile->device_descriptor->configuration_descriptors[idx]));
//---------------------------------------------------------------------------------------------------------

                        usb_conf_desc_t *conf = (usb_conf_desc_t *) usb_local_buf;
                        usb_send(0, true, DATA1, usb_local_buf,
                                MIN(conf->wTotalLength, header->wLength));

//---------------------------------------------------------------------------------------------------------
                    }
                    else
                    {
                        log_error("Requested configuration index is out of bound");
                        // TODO: Send error
                    }

                    break;

                case 0x03: // STRING_DESCRIPTOR
                    log_info("GET_DESCRIPTOR => STRING_DESCRIPTOR (%d)", idx);

                    // The next transaction will be IN so we write the string descriptors as requested
                    if (idx <= *(usb_profile->number_of_string_descriptors))
                    {
                        fill_buf_string(&(usb_profile->string_descriptors[idx]));
//---------------------------------------------------------------------------------------------------------

                        usb_send(0, true, DATA1, usb_local_buf,
                                MIN(usb_profile->string_descriptors[idx].bLength, header->wLength));

//---------------------------------------------------------------------------------------------------------
                    }
                    else
                    {
                        log_error("Requested string index is out of bound");
                        // TODO: Send error
                    }

                    break;

                case 0x06: // DEVICE_QUALIFIER_DESCRIPTOR
                    log_info("GET_DESCRIPTOR => DEVICE_QUALIFIER_DESCRIPTOR (%d)", idx);

                    usb_send(0, true, DATA1, (const uint8_t *)(usb_profile->device_qualifier_descriptor), USB_DEV_QUAL_DESC_SIZE);
                    break;

                case 0x07: // DEVICE_OTHER_SPEED_CONFIG
                    log_info("GET_DESCRIPTOR => DEVICE_OTHER_SPEED_CONFIGURATION (%d)", idx);

                    // The next transaction will be IN so we write the configuration descriptors as requested
                    if (idx < usb_profile->device_descriptor->bNumConfigurations)
                    {
                        fill_buf_configuration(&(usb_profile->device_descriptor->configuration_descriptors[idx]));
                        (header->wLength > sizeof(usb_local_buf)) ?
                        		usb_send(0, true, DATA1, usb_local_buf, sizeof(usb_local_buf)) :
                        		usb_send(0, true, DATA1, usb_local_buf, header->wLength);
                    }
                    else
                    {
                        log_error("Requested configuration index is out of bound");
                        // TODO: Send error
                    }

                    break;

                case 0x08: // DEVICE_INTERFACE_POWER
                    log_error("GET_DESCRIPTOR => DEVICE_INTERFACE_POWER (%d) [unsupported]", idx);
                    usb_set_stat_tx(0, STAT_TX_STALL);
                    break;

                case 0x0A: // DEVICE_DEBUG_DESCRIPTOR (USB 2.0 ECN Revision)
                    log_error("GET_DESCRIPTOR => DEVICE_DEBUG_DESCRIPTOR (%d) [unsupported]", idx);
                    usb_set_stat_tx(0, STAT_TX_STALL);
                    break;

                default:
                    log_error("GET_DESCRIPTOR => Unknown wValue: 0x%04X", header->wValue);
                    usb_set_stat_tx(0, STAT_TX_STALL);
                    break;
            }

            return;

        case 0x07: // SET_DESCRIPTOR
            log_error("SET_DESCRIPTOR not implemented");
            // This command is optionnal, it is not implemented yet
            break;

        case 0x08: // GET_CONFIGURATION

            if (header->wValue == 0 && header->wIndex == 0 && header->wLength == 1)
            {
//                usb_send(0, true, DATA1, &usb_configuration, header->wLength);
//                usb_send(0, true, DATA1, &usb_configuration, USB_CONF_DESC_SIZE);
            	(header->wLength > sizeof(usb_local_buf)) ?
            			usb_send(0, true, DATA1, usb_local_buf, sizeof(usb_local_buf)) :
            			usb_send(0, true, DATA1, usb_local_buf, header->wLength);
            }
            else
            {
            	log_error("error in GET_CONFIGURATION");
                usb_set_stat_tx(0, STAT_TX_STALL);
            }

            break;

        case 0x09: // SET_CONFIGURATION

        	log_not_implemented("SET_CONFIG !!!");
            switch (usb_state)
            {
                case USB_STATE_ADDRESS:

                    if (header->wValue != 0)
                    {
                        if (header->wValue <= usb_profile->device_descriptor->bNumConfigurations)
                        {
                            select_configuration(header->wValue);
                            usb_state = USB_STATE_CONFIGURED;
                        }
                        else
                        {
                            // TODO: Send a request error
                        	log_error("error in address state");
                        }
                    }

                    break;

                case USB_STATE_CONFIGURED:

                    if (header->wValue != 0)
                    {
                        if (header->wValue <= usb_profile->device_descriptor->bNumConfigurations)
                        {
                            select_configuration(header->wValue);
                        }
                        else
                        {
                            // TODO: Send a request error
                        	log_error("error in config state");
                        }
                    }
                    else
                    {
                        usb_state = USB_STATE_ADDRESS;
                        usb_configuration = 0;
                    }

                    break;

                default:
                    // TODO: Send error
                	log_error("SET_CONFIGURATION !!");
                    break;
            }

            // Start status stage
            usb_send_status(0, true);
            return;

        case 0x0A: // GET_INTERFACE
            log_error("GET_INTERFACE");
            break;

        case 0x0B: // SET_INTERFACE
            log_error("SET_INTERFACE");
            break;

        case 0x0C: // SYNCH_FRAME
            log_error("SYNCH_FRAME");
            break;

        default:
            log_error("Wrong request");
    }
}

static void endpoint0_callback(endpoint0_callback_t type)
{
    usb_device_request_t setup_header;
    uint16_t len, i;

    switch (type)
    {
        case ENDP0_IN:
        case ENDP0_OUT:
            log_debug((type == ENDP0_IN) ? "EP0_IN" : "EP0_OUT");
            if (data_callback[0] != NULL)
            {
                if (data_callback[0](0))
                {
                    data_callback[0] = NULL;
                }
            }
	    else
	    {
	    }
            break;

        case ENDP0_SETUP:
            log_debug("EP0_SETUP");

            len = usb_get_rx_len(0);

            if (len == 8)
            {
                usb_read_pma(0, (uint8_t *)&setup_header, 8);

                //log_debug("wLength: %d bRequest: 0x%02X bmRequestType: 0x%02X", setup_header.wLength, setup_header.bRequest, setup_header.bmRequestType);
                log_debug("Command : wLength: %d bRequest: 0x%02X bmRequestType: 0x%02X wValue: 0x%04x wIndex: 0x%04x",
                          setup_header.wLength, setup_header.bRequest, setup_header.bmRequestType, setup_header.wValue, setup_header.wIndex);


                switch (setup_header.bmRequestType & 0x7F)
                {
                    case 0x00: // Standard device requests
                        process_standard_device(&setup_header);
                        break;

                    case 0x01: // Standard interface requests

                        if (usb_profile->standard_interface)
                        {
                            usb_profile->standard_interface(&setup_header);
                        }
                        else
                        {
                            log_warning("Standard interface request not handled");
                        }

                        break;

                    case 0x02: // Standard endpoint requests

                        if (usb_profile->standard_endpoint)
                        {
                            usb_profile->standard_endpoint(&setup_header);
                        }
                        else
                        {
                            log_warning("Standard endpoint request not handled");
                        }

                        break;

                    case 0x20: // Class device requests

                        if (usb_profile->class_device)
                        {
                            usb_profile->class_device(&setup_header);
                        }
                        else
                        {
                            log_warning("Class device resquest not handled");
                        }

                        break;

                    case 0x21: // Class interface requests

                        if (usb_profile->class_interface)
                        {
                            usb_profile->class_interface(&setup_header);
                        }
                        else
                        {
                            log_warning("Class interface request not handled");
                        }

                        break;

                    case 0x22: // Class endpoint requests

                        if (usb_profile->class_endpoint)
                        {
                            usb_profile->class_endpoint(&setup_header);
                        }
                        else
                        {
                            log_warning("Class endpoint request not handled");
                        }

                        break;

                    case 0x40: // Vendor device requests

                        if (usb_profile->vendor_device)
                        {
                            usb_profile->vendor_device(&setup_header);
                        }
                        else
                        {
                            log_warning("Vendor device request not handled");
                        }

                        break;

                    case 0x41: // Vendor interface requests

                        if (usb_profile->vendor_interface)
                        {
                            usb_profile->vendor_interface(&setup_header);
                        }
                        else
                        {
                            log_warning("Vendor interface request not handled");
                        }

                        break;

                    case 0x42: // Vendor endpoint requests

                        if (usb_profile->vendor_endpoint)
                        {
                            usb_profile->vendor_endpoint(&setup_header);
                        }
                        else
                        {
                            log_warning("Vendor endpoint request not handled");
                        }

                        break;

                    default:
                        log_error("Reserved value not treated...");
                        break;
                }

            }
            else
            {
                log_error("EndPoint0: wrong setup header size: %d\r\n\tData content:", len);
		len = (len > USB_BUF_SIZE) ? USB_BUF_SIZE : len;
                usb_read_pma(0, usb_local_buf, len);

                for (i = 0; i < len; i++)
                {
                    printf("0x%02X ", usb_local_buf[i]);
                }

                usb_set_stat_tx(0, STAT_TX_STALL);
                usb_set_stat_rx(0, STAT_RX_STALL);

                data_callback[0] = NULL;
            }

            break;
    }
}

static inline void usb_correct_transfer()
{
    uint16_t istr;
    uint32_t endp, endp_reg;
    bool rx, tx;

    while ((istr = *usb_get_ISTR()) & USB_ISTR__CTR)
    {
        endp = istr & USB_ISTR__EP_ID_MASK & 0x7;
        endp_reg = *usb_get_EPnR(endp);
        rx = endp_reg & USB_EPnR__CTR_RX;
        tx = endp_reg & USB_EPnR__CTR_TX;

        // Set both status to NAK
        usb_set_stat_tx(endp, STAT_TX_NAK);
        usb_set_stat_rx(endp, STAT_RX_NAK);

        if (endp == 0)
        {
            if (istr & USB_ISTR__DIR)
            {
                // Clear correct transfer flags for reception CTR_RX
                *usb_get_EPnR(endp) &= ~(USB_EPnR__ALL_TOGGLE | USB_EPnR__CTR_RX);

                // Call the corresponding endpoint
                if (endp_reg & USB_EPnR__SETUP)
                {
                    // SETUP = 1
                    endpoint0_callback(ENDP0_SETUP);
                }
                else
                {
                    // SETUP = 0
                    endpoint0_callback(ENDP0_OUT);
                }
            }
            else
            {
                // Clear correct transfer flags for transmission CTR_TX
                *usb_get_EPnR(endp) &= ~(USB_EPnR__ALL_TOGGLE | USB_EPnR__CTR_TX);

                // Call the corresponding endpoint
                endpoint0_callback(ENDP0_IN);
            }
        }
        else
        {
            // Clear correct transfer flags (CTR_RX and CTR_TX)
            *usb_get_EPnR(endp) &= ~(USB_EPnR__ALL_TOGGLE | USB_EPnR__CTR_TX | USB_EPnR__CTR_RX);

            // Call the corresponding callback
            if (endpoint_callback[endp - 1])
            {
                endpoint_callback[endp - 1](endp, rx, tx);
            }
            else
            {
                log_warning("Endpoint %d request not handled (rx = %d, tx = %d)", endp, rx, tx);
            }
        }
    }
}

void usb_handle_interrupt()
{
    static uint16_t resume_cpt = 0, resume_state = 0;
    uint16_t istr = *usb_get_ISTR();

    // log_debug("USB irq %04x",istr);

    // Overrun
    if (istr & USB_ISTR__PMAOVR)
    {
        // Acknowledge interrupt
        *usb_get_ISTR() = ~USB_ISTR__PMAOVR;

        log_error("PMA Overrun");
    }

    // Error
    if (istr & USB_ISTR__ERR)
    {
        // Acknowledge interrupt
        *usb_get_ISTR() = ~USB_ISTR__ERR;

        log_error("Error on bus");
    }

    // Reset request
    if (istr & USB_ISTR__RESET)
    {
        // Acknowledge interrupt
        *usb_get_ISTR() = ~USB_ISTR__RESET;

        // Restore packet memory start address
        memory_start = 0x40;
        // USB address is 0 and EF = 1
        *usb_get_DADDR() = USB_DADDR__EF;

        // As a reset occured we must enable the USB function
        // and implement the default control endpoint (endpoint 0)
        usb_endpoint_init(0, USB_ENDPOINT_CONTROL, usb_profile->device_descriptor->bMaxPacketSize0, (usb_endpoint_callback_t)0);

	// Set default configuration / interface
	usb_configuration = 0;
	usb_interface     = 0;
	usb_interface_alt = 0;

        // The USB device enters the "Default" state
        usb_state = USB_STATE_DEFAULT;
    }

    // Correct transfer
    if (istr & USB_ISTR__CTR)
    {
        usb_correct_transfer();
    }

    // Suspend mode request
    if (istr & USB_ISTR__SUSP)
    {
        // We do not support suspend mode so we have to resume
        resume_cpt = 2;
        resume_state = 1;
        // Acknowledge interrupt
        *usb_get_ISTR() = ~USB_ISTR__SUSP;
    }

    // Wakeup
    if (istr & USB_ISTR__WKUP)
    {
        // TODO call corresponding callback
        // Acknowledge interrupt
        *usb_get_ISTR() = ~USB_ISTR__WKUP;
    }

    // Expected start of frame
    if (istr & USB_ISTR__ESOF)
    {
        // This interrupt is used as a 1ms-timer
        if (resume_cpt == 0)
        {
            if (resume_state == 1)
            {
                *usb_get_CNTR() |= USB_CNTR__RESUME;
                resume_state = 0;
                resume_cpt = 10;
            }
            else
            {
                *usb_get_CNTR() &= ~USB_CNTR__RESUME;
                resume_state = 0;
            }
        }
        else
        {
            resume_cpt--;
        }

        // Acknowledge interrupt
        *usb_get_ISTR() = ~USB_ISTR__ESOF;
    }


    // Clear other interrupts
    *usb_get_ISTR() = 0;
}

