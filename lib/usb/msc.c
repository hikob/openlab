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
 * msc.c
 *
 *  Created on: May 18, 2012
 *      Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

/*
 *
 * The driver handles MSC using the following protocols
 *   - Mass Storage Bulk-Only (BBB)  
 *     (BBB refers Bulk/Bulk/Bulk for Command/Data/Status phases)
 *
 * Universal Serial Bus Mass Storage Class specs:
 * http://www.usb.org/developers/devclass_docs/usb_msc_overview_1.2.pdf
 * http://www.usb.org/developers/devclass_docs/usbmassbulk_10.pdf
 * http://www.usb.org/developers/devclass_docs/usb_msc_cbi_1.1.pdf
 * http://www.usb.org/developers/devclass_docs/usbmass-ufi10.pdf
 *
 * USB and SCSI data
 * http://www.shakthimaan.com/downloads/usb/usb-msc-0.2.pdf
 *
 * Over these wire protocols it handles the following command protocols
 *   - SCSI
 *
 *
 *
 */

#include "platform.h"
#include "usb.h"
#include "libusb.h"
#include "msc.h"
#include "scsi.h"
#include "event.h"

#define NO_DEBUG_HEADER
#define LOG_LEVEL LOG_LEVEL_WARNING
//#define LOG_LEVEL LOG_LEVEL_DEBUG-1
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

/* ************************************************************ */
/* ************************************************************ */
/* ************************************************************ */

#define MSC_EP_IN              1
#define MSC_EP_OUT             2

#define MSC_ENDPOINT_IN        USB_ENDPOINT1_IN
#define MSC_ENDPOINT_OUT       USB_ENDPOINT2_OUT

#define ENDPOINT_0              0
#define ENDPOINT_0_SIZE        64

#define MSC_ENDPOINT_IN_SIZE   64
#define MSC_ENDPOINT_OUT_SIZE  64

/* ************************************************************ */
/* ************************************************************ */
/* ************************************************************ */

/* Override string descriptors to fit Win#$*!@ requirement */
#define usb_VendorId   0x058f
#define usb_ProductId  0x6387
#define usb_BcdDevice  0x102
static const uint16_t usb_langid_string[]    = { 0x0409 };
static const uint16_t usb_vendor_string[]    = { 'G','e','n','e','r','i','c'};
static const uint16_t usb_product_string[]   = { 'M', 'a', 's','s',' ','S','t','o','r','a','g','e'};
static const uint16_t usb_serial_string[]  = { 'H','i','K','o','B'};
static const uint8_t usb_msc_string_desc_size = 4;
static const usb_string_desc_t usb_msc_string_desc[] =
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
};

typedef enum 
{
    MSC_ADSC                     = 0x00,
    MSC_BULK_ONLY_GET_MAX_LUN    = 0xFE,
    MSC_BULK_ONLY_STORAGE_RESET  = 0xFF
} usb_msc_request_t;

typedef enum {
     USB_SUB_CLASS_SCSI     = 0x06
} usb_msc_subclass_t;


/***** Descriptors *****/

/* endpoints handler forward declarations */
static void usb_msc_data_in (uint8_t endp, bool rx, bool tx);
static void usb_msc_data_out(uint8_t endp, bool rx, bool tx);

/* class handler forward declaration */
static void usb_msc_class_interface(usb_device_request_t *req);
static void usb_msc_standard_endpoint(usb_device_request_t *header);


static const usb_endp_desc_t usb_msc_endp_desc[] =
{
    {
        .bLength               = USB_ENDP_DESC_SIZE,
        .bDescriptorType       = USB_DESC_ENDPOINT,
        .bEndpointAddress      = MSC_ENDPOINT_IN,
        .bmAttributes          = USB_ENDPOINT_BULK,
        .wMaxPacketSize        = MSC_ENDPOINT_IN_SIZE,
        .bInterval             = 0,
        .class_specific        = 0,
        .class_specific_len    = 0,
        .endpoint_callback     = usb_msc_data_in
    },
    {
        .bLength               = USB_ENDP_DESC_SIZE,
        .bDescriptorType       = USB_DESC_ENDPOINT,
        .bEndpointAddress      = MSC_ENDPOINT_OUT,
        .bmAttributes          = USB_ENDPOINT_BULK,
        .wMaxPacketSize        = MSC_ENDPOINT_OUT_SIZE,
        .bInterval             = 0,
        .class_specific        = 0,
        .class_specific_len    = 0,
        .endpoint_callback     = usb_msc_data_out
    }
};

/* ************************************************** */
/* INTERFACES (MSC + DFU)                             */
/* ************************************************** */

#define DFU_INTERFACE        1
#define DFU_INTERFACE_STRING 4

static const usb_iface_desc_t usb_msc_iface_desc[] =
{
    {
        .bLength               = USB_IFACE_DESC_SIZE,
        .bDescriptorType       = USB_DESC_INTERFACE,
        .bInterfaceNumber      = 0,
        .bAlternateSetting     = 0,
        .bNumEndpoints         = 2,
        .bInterfaceClass       = USB_CLASS_MSC,
        .bInterfaceSubClass    = USB_SUB_CLASS_SCSI,
        .bInterfaceProtocol    = 0x50, // Bulk Only
        .iInterface            = 0,
        .class_specific        = 0,    // func_desc
        .class_specific_len    = 0,    // sizeof func_desc
        .endpoint_descriptors  = usb_msc_endp_desc
    }, 
    // DFU Interface (bInterfaceNumber, iInterface)
    //DFU_RUNTIME_INTERFACE(DFU_INTERFACE, DFU_INTERFACE_STRING)
};

/* ************************************************** */
/* CONFIGURATION                                      */
/* DEVICE DESCRIPTOR                                  */
/* DEVICE QUALIFIER (for other device speed)          */
/* ************************************************** */

static const usb_conf_desc_t usb_msc_conf_desc[] =
{
    {
        .bLength               = USB_CONF_DESC_SIZE,
        .bDescriptorType       = USB_DESC_CONFIGURATION,
        .wTotalLength          = 0,                      // This field is computed during USB initialization
        .bNumInterfaces        = sizeof(usb_msc_iface_desc) / sizeof(usb_iface_desc_t),
        .bConfigurationValue   = 1,
        .iConfiguration        = 0,
        .bmAttributes          = 0x80,                   // 
        .bMaxPower             = 25,                     // x 2mA
        .class_specific        = 0,
        .class_specific_len    = 0,
        .interface_descriptors = usb_msc_iface_desc
    }
};

static const usb_dev_desc_t usb_msc_dev_desc =
{
    .bLength                   = USB_DEV_DESC_SIZE,
    .bDescriptorType           = USB_DESC_DEVICE,
    .bcdUSB                    = 0x0200,
    .bDeviceClass              = 0,                  // Defined at Interface level
    .bDeviceSubClass           = 0,                  // Defined at Interface level
    .bDeviceProtocol           = 0,                  // Defined at Interface level
    .bMaxPacketSize0           = ENDPOINT_0_SIZE,
    .idVendor                  = usb_VendorId,
    .idProduct                 = usb_ProductId,
    .bcdDevice                 = usb_BcdDevice,
    .iManufacturer             = 1,
    .iProduct                  = 2,
    .iSerialNumber             = 3,
    .bNumConfigurations        = 1,
    .class_specific            = 0,
    .class_specific_len        = 0,
    .configuration_descriptors = usb_msc_conf_desc
};

static const usb_dev_qual_desc_t usb_msc_dev_qual_desc =
{
    .bLength                   = USB_DEV_QUAL_DESC_SIZE,
    .bDescriptorType           = USB_DESC_DEVICE,
    .bcdUSB                    = 0x0200,
    .bDeviceClass              = 0,
    .bDeviceSubClass           = 0,
    .bDeviceProtocol           = 0,
    .bMaxPacketSize0           = ENDPOINT_0_SIZE,
    .bNumConfigurations        = 1,
    .bReserved                 = 0,
};

/* ************************************************** */
/* PROFILE                                            */
/* ************************************************** */

const usb_profile_t usb_msc =
{
    .device_descriptor            = &usb_msc_dev_desc,
    .device_qualifier_descriptor  = &usb_msc_dev_qual_desc,
    .number_of_string_descriptors = &usb_msc_string_desc_size,
    .string_descriptors           =  usb_msc_string_desc,

    .standard_interface = usb_default_process_standard_interface,
    .standard_endpoint  = usb_msc_standard_endpoint,
    .class_device       = NULL,
    .class_interface    = usb_msc_class_interface,
    .class_endpoint     = usb_default_process_class_endpoint,
    .vendor_device      = NULL,
    .vendor_interface   = NULL,
    .vendor_endpoint    = NULL
};

/* ********************************************************************** */
/* ********************************************************************** */
/* USB Class internals                                                    */
/* ********************************************************************** */
/* ********************************************************************** */

/* Global variables */

typedef enum {
#if defined(__BIG_ENDIAN__)
    CBW_Signature = 0x55534243,
    CSW_Signature = 0x55534253
#else
    CBW_Signature = 0x43425355,
    CSW_Signature = 0x53425355 
#endif
} msc_cmd_signatures_t;


// Command Block Wrapper
typedef struct {
    uint32_t dCBWSignature;           // 43425355h (little endian)
    uint32_t dCBWTag;                 // Tag for answer identification
    uint32_t dCBWDataTransferLength;  // Expected transfer length before CSW
    uint8_t  dCBWFlags;               // Bit7 = [0:out,1:in]
    uint8_t  dCBWLun;                 // Lun    & 0x0f = Lun number
    uint8_t  dCBWCBLength;            // Length & 0x1f = CBCWBlock length
    uint8_t  CBWCB[16];               // CBWCBlock data
} __attribute__((packed)) msc_cbw_t;  // 31 bytes


// Command Block Status
typedef struct {
    uint32_t dCSWSignature; // 53425355h (little endian)
    uint32_t dCSWTag;
    uint32_t dCSWDataResidue;
    uint8_t  dCSWStatus;
} __attribute__((packed)) msc_csw_t; // 13 bytes


/* MSC Commands return code */
typedef enum {
    CSW_Command_Passed      = 0x00,
    CSW_Command_Failed      = 0x01,
    CSW_Phase_Error         = 0x02
    /* > 0x02 Reserved */
} msc_csw_status_t;


/* MSC FSM states */
typedef enum {
    MSC_STATE_NONE          = 0,
    MSC_STATE_READY         = 1, 
    MSC_STATE_DATA_IN       = 2,
    MSC_STATE_DATA_OUT      = 3,
    MSC_STATE_STATUS        = 4,
    MSC_STATE_STATUS_sent   = 5,
    //MSC_STATE_STATUS_tosend = 6
} msc_state_t;


/* ***************************** */
/* MSC Global Variable           */
/* ***************************** */

static scsi_params_t  params;

/* MSC command, status and state   */
static msc_cbw_t         cbw;                         // 31
static msc_state_t       msc_state = MSC_STATE_NONE;  //  4
static msc_csw_status_t  msc_csw_status;              //  4

/*
 * Data Buffers
 * 
 * when the buffer has been used (write + read) 
 * it must be released by setting len = 0 
 */

/* data buffers, 512 is a SD-Card block size */
#define MSC_BUFFER_SIZE  512

typedef struct {
    uint8_t  data[MSC_BUFFER_SIZE];
    uint16_t idx;
    uint16_t len;
} msc_data_buff_t;

/*
 * msc_buff is a circular array of buffers. 2 should do it.
 * At this point of dev&test we are strictly sequential and ordered.
 * Next release may have asyn double buffered I/O
 */
#define MSC_BUFFER_COUNT   1

static msc_data_buff_t msc_buff[MSC_BUFFER_COUNT] __attribute__((aligned(4)));;
static int8_t          msc_buff_read;
static int8_t          msc_buff_write;

/* ************************************************************ */
/* **** Public Functions ************************************** */
/* ************************************************************ */


void usb_msc_reset()
{
    uint8_t i;

    for(i=0; i<MSC_BUFFER_COUNT; i++)
    {
	msc_buff[i].idx = 0;
	msc_buff[i].len = 0;
    }
    msc_buff_read       = 0;
    msc_buff_write      = 0;
    msc_state           = MSC_STATE_NONE;
}


void usb_msc_init()
{
    usb_msc_reset();
    scsi_init();
}


/* ************************************************************ */
/* ************************************************************ */
/* ************************************************************ */


void msc_cbw_log(msc_cbw_t *c)
{
    log_info("MSC CBW lun %d, tag %08x, data transfer len %d, flags %02x (%s), len %d", 
	     c->dCBWLun, c->dCBWTag, c->dCBWDataTransferLength, 
	     c->dCBWFlags, (c->dCBWDataTransferLength > 0) ? ( (c->dCBWFlags & 0x80) ? "in" : "out" ) : "ignored",
	     c->dCBWCBLength);
}


static inline uint16_t min(uint16_t a, uint16_t b) { return a < b ? a : b; }


/*
 * Buffer: mark the current read buffer as free and move to 
 * the next one
 */
void usb_msc_close_read_buff()
{
    if (msc_buff[msc_buff_read].len != 0)
    {
	log_error("MSC Closing data on read buffer %d which has len = %d",msc_buff_read,msc_buff[msc_buff_read].len);
	return;
    }

    msc_buff[msc_buff_read].idx = 0;
    msc_buff[msc_buff_read].len = 0;

    msc_buff_read = msc_buff_read + 1;
    if (msc_buff_read >= MSC_BUFFER_COUNT)
    {
	msc_buff_read = 0;
    }
}

/*
 * Buffer: commands can use a write buffer the way they need to.
 * Once a buffer is filled with information it must be closed so
 * that it can be used on the read side.
 */
void usb_msc_close_write_buff(uint16_t length)
{
    if (msc_buff[msc_buff_write].len != 0)
    {
	log_error("MSC Registering data on write buffer %d which has len = %d",msc_buff_write,msc_buff[msc_buff_write].len);
	return;
    }

    msc_buff[msc_buff_write].idx = 0;
    msc_buff[msc_buff_write].len = length;

    msc_buff_write = msc_buff_write + 1;
    if (msc_buff_write >= MSC_BUFFER_COUNT)
    {
	msc_buff_write = 0;
    }
}

/*
 * USB IN: use the current read buffer as a data source for IN packets.
 * Returns true when the buffer is empty.
 */
bool usb_msc_send_data_to_endpoint()
{
    uint16_t  len;
    uint16_t  idx;
    uint8_t  *data;

    if (msc_buff[msc_buff_read].len == 0)
    {
	return true;
    }

    len  = min( msc_buff[msc_buff_read].len, usb_get_max_packet_size( MSC_EP_IN ) );
    idx  = msc_buff[ msc_buff_read ].idx;
    data = msc_buff[ msc_buff_read ].data;

    usb_send( MSC_EP_IN , true, NO_CHANGE, &data[idx], len);
    DBG("i");
    msc_buff[msc_buff_read].len = msc_buff[msc_buff_read].len - len;
    msc_buff[msc_buff_read].idx = msc_buff[msc_buff_read].idx + len;

    if (msc_buff[msc_buff_read].len == 0)
    {
	usb_msc_close_read_buff();
	return true;
    }
    return false;
}

/*
 * USB OUT: write from out endpoint to buffer.
 * returns true if buffer is full or if #read < #max which
 * means EOP. On EOP the write buffer is closed.
 */
bool usb_msc_read_data_from_endpoint()
{
    uint16_t  rlen,plen,blen;
    uint16_t  idx;
    uint8_t  *data;

    if (msc_buff[msc_buff_write].idx == MSC_BUFFER_SIZE)
    {
	return true;
    }

    idx  = msc_buff[ msc_buff_read ].idx;
    data = msc_buff[ msc_buff_read ].data;
    plen = usb_get_max_packet_size( MSC_EP_OUT );
    rlen = usb_recv_get_len( MSC_EP_OUT );
    blen = MSC_BUFFER_SIZE - msc_buff[msc_buff_write].idx;

    if (rlen > blen)
    {
	log_error("MSC out called with too small buffer (%d/%d bytes)",rlen,blen);
	rlen = blen;
    }

    usb_recv( MSC_EP_OUT, &data[idx], rlen);
    DBG("o");
    msc_buff[msc_buff_write].idx = msc_buff[msc_buff_write].idx + rlen;
    usb_recv_set_status( MSC_EP_OUT , STAT_RX_VALID);

    if ((msc_buff[msc_buff_write].idx ==  MSC_BUFFER_SIZE) || (rlen < plen))
    {
	usb_msc_close_write_buff( msc_buff[msc_buff_write].idx );
	return true;
    }
    return false;
}

/* ********************************************************************** */
/* ********************************************************************** */
/* ********************************************************************** */

void usb_msc_build_csw(uint8_t *b, uint32_t t, msc_csw_status_t s)
{
    msc_csw_t *csw = (msc_csw_t *) b;
    csw->dCSWSignature   = CSW_Signature;
    csw->dCSWTag         = t;
    csw->dCSWDataResidue = 0;
    csw->dCSWStatus      = s;
}

/* ********************************************************************** */
/* ********************************************************************** */
/* ********************************************************************** */

scsi_params_t usb_msc_build_scsi_params(uint8_t count, int8_t msc_buff_direction, uint32_t datamax, uint32_t *datalen, scsi_status_t *status)
{
    scsi_params_t scsi_params = {
        .cont    =  count,
        .lun     =  cbw.dCBWLun,
        .cmd     =  (uint8_t*)cbw.CBWCB,
        .data    =  msc_buff[msc_buff_direction].data,
        .datamax =  datamax,
        .datalen =  datalen,
        .status  =  status,
    };
    return scsi_params;
}

/* ********************************************************************** */
/* ********************************************************************** */
/* ********************************************************************** */


#define MSC_EV     ((void*)0)
#define MSC_EV_IN  ((void*)1)
#define MSC_EV_OUT ((void*)2)

void usb_msc_ev_handler( handler_arg_t arg );

event_status_t msc_ev_post(handler_arg_t arg)
{
    //DBG("+");
    return event_post(EVENT_QUEUE_APPLI, usb_msc_ev_handler, arg);
}

event_status_t msc_ev_post_from_isr(handler_arg_t arg)
{
    //DBG("!");
    return event_post_from_isr(EVENT_QUEUE_APPLI, usb_msc_ev_handler, arg);
}

int usb_msc_scsi_handler(bool firstcall)
{
    int            handler_ret = 0;
    scsi_cmdret_t  ret;
    scsi_status_t  status;
    uint32_t       datalen;
    static uint8_t cnum;

    /*
     * Do the actual SCSI Command
     */
    cnum = (firstcall) ? 0 : cnum + 1;
    switch (msc_state)
    {
    case MSC_STATE_DATA_OUT:
	params = usb_msc_build_scsi_params(cnum, msc_buff_read, msc_buff[msc_buff_read].len, &datalen, &status);
	ret = scsi_process_command(params);
	break;
    default:
	params = usb_msc_build_scsi_params(cnum, msc_buff_write, MSC_BUFFER_SIZE, &datalen, &status);
	ret = scsi_process_command(params);
	break;
    }

    /*
     * Command result and further action
     */
    switch (status)
    {
    case SCSI_GOOD:
	if (cbw.dCBWDataTransferLength > 0)
	{
	    if (cbw.dCBWFlags & 0x80)
	    {
                /*
		 * Data are flowing from Host to Device
		 */
		msc_csw_status    = CSW_Command_Passed;
		msc_state         = MSC_STATE_DATA_IN;

		if (datalen > cbw.dCBWDataTransferLength)
		{
		    log_debug("MSC SCSI datalen %d > cbw.dCBWDataTransferLength %d", datalen, cbw.dCBWDataTransferLength);
		    datalen = cbw.dCBWDataTransferLength;
		}
		else if (datalen < cbw.dCBWDataTransferLength)
		{
		    if (ret == SCSI_CMD_PARTIAL)
		    {
			/* The return is only a part of the full request, 
			 * we'll have to come back */
			// log_debug("MSC SCSI command made partially, returning %d bytes (%d)", 
			// datalen, cbw.dCBWDataTransferLength - datalen);
		    }
		    else 
		    {
			/* we are done on our side although the request 
			 * was bigger, let's pretend it's ok */
			cbw.dCBWDataTransferLength = datalen;
		    }
		}
		else 
		{
		    /* ok : datalen == cbw.dCBWDataTransferLength */
		    //log_debug("MSC SCSI command complete with the correct amount of data");
		}

		cbw.dCBWDataTransferLength = cbw.dCBWDataTransferLength - datalen;
		if (cbw.dCBWDataTransferLength == 0)
		{
		    //log_info("Last packet");
		}
		usb_msc_close_write_buff(datalen);
		handler_ret = datalen;
	    }
	    else
	    {
		/*
		 * Data are flowing from Device to Host
		 * can we assert(msc_state == MSC_STATE_DATA_OUT) ?
		 */
		if (msc_state != MSC_STATE_DATA_OUT) // == MSC_STATE_READY ??
		{
		    msc_csw_status    = CSW_Command_Passed;
		    msc_state         = MSC_STATE_DATA_OUT;
		}
		else
		{
		    if (datalen != msc_buff[msc_buff_read].len)
		    {
			log_error("datalen != msc_buff[msc_buff_read].len !!");
			cbw.dCBWDataTransferLength = datalen;
		    }
		    msc_buff[msc_buff_read].len = 0;
		    usb_msc_close_read_buff();
		    cbw.dCBWDataTransferLength = cbw.dCBWDataTransferLength - datalen;
		    if (cbw.dCBWDataTransferLength == 0)
		    {
			msc_csw_status    = CSW_Command_Passed;
			msc_state         = MSC_STATE_STATUS;

			msc_state         = MSC_STATE_STATUS_sent;
			usb_msc_build_csw( msc_buff[msc_buff_write].data, cbw.dCBWTag, msc_csw_status);
			usb_msc_close_write_buff(sizeof(msc_csw_t));
			if (! usb_msc_send_data_to_endpoint())
			{
			    // log_error("this should not happen");
			}
		    }
		    else
		    {
			DBG("+");
			msc_csw_status    = CSW_Command_Passed;
			msc_state         = MSC_STATE_DATA_OUT;
		    }
		}
	    }
	}
	else // Data are done, send Status
	{
	    msc_csw_status = CSW_Command_Passed;
	    msc_state      = MSC_STATE_STATUS_sent;
	    usb_msc_build_csw( msc_buff[msc_buff_write].data, cbw.dCBWTag, msc_csw_status);
	    usb_msc_close_write_buff(sizeof(msc_csw_t));
	    if (! usb_msc_send_data_to_endpoint())
	    {
		// log_error("this should not happen");
	    }
	}
	break;
	
    case SCSI_CHECK_CONDITION:
	msc_csw_status            = CSW_Command_Failed;
	msc_state                 = MSC_STATE_STATUS;
	msc_ev_post( MSC_EV );
	//log_info("MSC Check Condition");
	break;
	
    default:
	msc_csw_status            = CSW_Command_Failed;
	msc_state                 = MSC_STATE_STATUS;
	msc_ev_post( MSC_EV );
	//log_error("MSC Unhandled SCSI return status");
	break;
    }
    return handler_ret;
}

/* ********************************************************************** 
 * MSC Main Event Handler                                                 
 * ********************************************************************** */

void usb_msc_ev_handler( handler_arg_t arg )
{
    int scsi_ret;

    switch (msc_state)
    {
    case MSC_STATE_NONE:
	break;

    case MSC_STATE_READY:
	// first SCSI Command call, fill data buffer with answer
	scsi_ret = usb_msc_scsi_handler(true); 
	if (scsi_ret)
	{
	    if (usb_msc_send_data_to_endpoint()) 
	    {
		// buffer sent in 1 packet, skip to STATUS
		msc_state     = MSC_STATE_STATUS;
	    }
	}
	break;

    case MSC_STATE_DATA_IN:
	if (usb_msc_send_data_to_endpoint()) 
	{
	    if (cbw.dCBWDataTransferLength > 0) 
	    {
		/* SCSI command not complete, precharge next buffer but do not
		 * write to endpoint */
		usb_msc_scsi_handler(false); 
	    }
	    else
	    {
		// all requested information transferred, going to Status
		msc_state = MSC_STATE_STATUS;
	    }
	}
	break;

    case MSC_STATE_DATA_OUT:
	if (usb_msc_read_data_from_endpoint())
	{
	    scsi_ret = usb_msc_scsi_handler(false);
	}
	break;

    /* case MSC_STATE_STATUS_tosend: */
    /* 	msc_state = MSC_STATE_STATUS; */
    /* 	break; */

    case MSC_STATE_STATUS:
	//DBG("S");
	msc_state = MSC_STATE_STATUS_sent;
	usb_msc_build_csw( msc_buff[msc_buff_write].data, cbw.dCBWTag, msc_csw_status);
	usb_msc_close_write_buff(sizeof(msc_csw_t));
	if (usb_msc_send_data_to_endpoint())
	{
	    //DBG("s\n");
	}
	else
	{
	    log_error(" STATUS cannot be sent ??");
	}
	//DBG("\n");
	break;

    case MSC_STATE_STATUS_sent:
	msc_state = MSC_STATE_NONE;
	break;
    }
}


/* **********************************************************************
 * MSC Host Input handler                                                
 * This function is called from an IRQ context.
 * ********************************************************************** */


static void usb_msc_data_in(uint8_t endp, bool rx, bool tx)
{
    if (!tx && rx)
    {
        log_error("MSC data in (to host) callback called with: rx = %d and tx = %d", rx, tx);
	return;
    }

    msc_ev_post_from_isr( MSC_EV_IN );
}


/* **********************************************************************
 * MSC Host Output handler
 * This function is called from an IRQ context.
 * ********************************************************************** */


static void usb_msc_data_out(uint8_t endp, bool rx, bool tx)
{
    uint16_t       len;
    event_status_t evs;
    
    if (!rx && tx)
    {
        log_error("MSC data out (from host) callback called with: rx = %d and tx = %d", rx, tx);
	return; 
    }

    switch (msc_state)
    {

    case MSC_STATE_NONE:
    case MSC_STATE_STATUS:
    case MSC_STATE_STATUS_sent:

	len = usb_recv_get_len( MSC_EP_OUT );
	if (len == sizeof(cbw))
	{
	    usb_recv( MSC_EP_OUT, (uint8_t*)&cbw,sizeof(cbw) );

	    if (cbw.dCBWSignature == CBW_Signature)
	    {
		msc_state = MSC_STATE_READY;
		evs = msc_ev_post_from_isr( MSC_EV_OUT );
		if (evs == EVENT_OK)
		{
		    usb_recv_set_status(endp, STAT_RX_VALID);
		    DBG("r");
		    //msc_cbw_log(&cbw);
		}
		else
		{
		    usb_recv_set_status(endp, STAT_RX_STALL);
		    log_error("MSC post event on full queue");
		}
	    }
	    else
	    {
		usb_recv_set_status(endp, STAT_RX_STALL);
		log_error("MSC wrong CBW Signature (0x%08x)",cbw.dCBWSignature);
	    }
	}
	else
	{
	    usb_recv_set_status(endp, STAT_RX_STALL); 
	    log_error("MSC wrong packet size (received %d bytes)",len);
	}
	break;
	
    case MSC_STATE_READY:
    case MSC_STATE_DATA_OUT:
	msc_ev_post_from_isr( MSC_EV_OUT );
	break;

    case MSC_STATE_DATA_IN:
	log_error("MSC data_out with inconsistent state DATA_IN");
	msc_cbw_log(&cbw);
	msc_state = MSC_STATE_NONE;
	usb_recv_set_status(endp, STAT_RX_STALL);
	HALT(); // optionx
	break;
    }
}


/* ********************************************************************** */
/* MSC Class Interface                                                    */
/* ********************************************************************** */


static void usb_msc_class_interface(usb_device_request_t *req)
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
    case MSC_ADSC:
	log_error("MSC: Accept Device Specific Commands (ADSC) not implemented");
	break;

    case MSC_BULK_ONLY_GET_MAX_LUN:
	usb_send( ENDPOINT_0, true, DATA1, &scsi_max_lun, 1);
	log_info("MSC Get Max Lun (%d)", usb_msc_max_lun);
	break;

    case MSC_BULK_ONLY_STORAGE_RESET:
	usb_msc_reset();
	usb_send_set_status( ENDPOINT_0, STAT_TX_VALID);
	log_info("USB MSC Storage Reset");
	break;

    default:
	log_error("Unknown request: 0x%X", req->bRequest);
	break;
    }
}


/* ********************************************************************** */
/*                                                                        */
/* ********************************************************************** */


static void usb_msc_standard_endpoint(usb_device_request_t *req)
{
    switch (req->bRequest)
    {
        case 0x01: // CLEAR_FEATURE, 
	    usb_msc_reset();
	    log_warning("Clear feature not/badly implemented");
	    break;
        default:
	    log_warning("MSC Standard endpoint not handled: bRequest = 0x%02X "
			"bmRequestType = 0x%02X wValue = 0x%04X wIndex = 0x%04X wLength = 0x%04X",
			req->bRequest, req->bmRequestType, req->wValue, req->wIndex, req->wLength);
	    break;
    }
}


/* ********************************************************************** */
/* ********************************************************************** */
/* ********************************************************************** */
/* ********************************************************************** */
