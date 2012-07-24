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
 * scsi.c
 *
 *  Created on: May 23, 2012
 *      Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "scsi.h"

#define NO_DEBUG_HEADER
#define LOG_LEVEL LOG_LEVEL_WARNING
//#define LOG_LEVEL LOG_LEVEL_DEBUG-1
#include "printf.h"

#if (LOG_LEVEL < LOG_LEVEL_DEBUG)
#define DBG(x...)   printf(x)
#define UNUSED
#else
#define DBG(x...)   do { } while(0)
#define UNUSED      __attribute__((unused))
#endif // LOG_LEVEL

/**
 *
 * The driver handles SCSI command subset for USB Mass Storage Class (MSC)
 * 
 * [SPC] SCSI Primary Commands - 2  (SPC-2), (t10.org)
 * [MMC] Multimedia Command Set - 5 (MMC-5), (t10.org)
 * 
 * This driver implements the following commands w.r.t SCSI INQUIRY requests
 * - PERIPHERAL DEVICE TYPE = 00h (direct access block device)
 * - VERSION = 04h (SPC-2) 
 * 
 * see http://en.wikipedia.org/wiki/SCSI_command
 * and http://www.seagate.com/staticfiles/support/disc/manuals/Interface%20manuals/100293068c.pdf
 */



typedef enum {
    SCSI_CDB6,
    SCSI_CDB10
} scsi_cdb_type_t;


typedef enum {
    SCSI_TEST_UNIT_READY              = 0x00,
    SCSI_REQUEST_SENSE                = 0x03,
    SCSI_INQUIRY                      = 0x12,
    SCSI_MODE_SENSE6                  = 0x1a,
    SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL = 0x1e,
    SCSI_READ_CAPACITY10              = 0x25, /* defined in MMC5 */
    SCSI_READ10                       = 0x28, /* defined in MMC5 */
    SCSI_WRITE10                      = 0x2a  /* defined in MMC5 */
} scsi_cdb_op_t;


#define BLOCKSIZE                    512ul


/* **************************************************
 * SCSI Global variables and definitions
 * ************************************************** */

#define SCSI_SENSE_REPLY_FIXED_SIZE    17
#define SCSI_SENSE_REPLY_SIZE           8


typedef scsi_cmdret_t (*scsi_cdb_cmd_t)( SCSI_PARAMS );

static  scsi_cmdret_t scsi_cmd_test_unit_ready             ( SCSI_PARAMS );
static  scsi_cmdret_t scsi_cmd_request_sense               ( SCSI_PARAMS );
static  scsi_cmdret_t scsi_cmd_inquiry                     ( SCSI_PARAMS );
static  scsi_cmdret_t scsi_cmd_mode_sense6                 ( SCSI_PARAMS );
static  scsi_cmdret_t scsi_cmd_prevent_allow_medium_removal( SCSI_PARAMS );
static  scsi_cmdret_t scsi_cmd_read_capacity10             ( SCSI_PARAMS );
static  scsi_cmdret_t scsi_cmd_read10                      ( SCSI_PARAMS );
static  scsi_cmdret_t scsi_cmd_write10                     ( SCSI_PARAMS );
static  scsi_cmdret_t scsi_cmd_default_handler             ( SCSI_PARAMS );

typedef struct {
    scsi_cdb_op_t     cdb_op; 
    scsi_cdb_type_t   cdb_type;
    scsi_cdb_cmd_t    cdb_cmd;
} scsi_cdb_desc_t;

static const scsi_cdb_desc_t scsi_cdb_list[] = {
    { SCSI_INQUIRY,                      SCSI_CDB6,  scsi_cmd_inquiry                      },
    { SCSI_REQUEST_SENSE,                SCSI_CDB6,  scsi_cmd_request_sense                },
    { SCSI_TEST_UNIT_READY,              SCSI_CDB6,  scsi_cmd_test_unit_ready              },
    { SCSI_MODE_SENSE6,                  SCSI_CDB6,  scsi_cmd_mode_sense6                  },
    { SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL, SCSI_CDB6,  scsi_cmd_prevent_allow_medium_removal },
    { SCSI_READ_CAPACITY10,              SCSI_CDB10, scsi_cmd_read_capacity10              },
    { SCSI_READ10,                       SCSI_CDB10, scsi_cmd_read10                       },
    { SCSI_WRITE10,                      SCSI_CDB10, scsi_cmd_write10                      }
};



/* ********************************************************************** */
/* Debug helpers and misc functions                                       */
/* ********************************************************************** */


void scsi_dump_cmd(const uint8_t* cmd, uint8_t len)
{
    int i;
    if (len > 0)
    {
	DBG("SCSI data ");
	for(i=0; i<len; i++)
	{
	    DBG("%02x:", cmd[i]);
	}
    }
}

bool isprintable(uint8_t c)
{
    return  (' ' <= c && c <= '~');
}

void scsi_dump_blk(uint32_t lba, uint8_t *d, uint16_t len)
{
    uint16_t  c,l;
    const uint8_t COL = 16;

    uint32_t UNUSED la = lba * BLOCKSIZE;

    printf("\n");
    for(l=0; l<len/COL; l++)
    {
	printf("%08x  ",la + l*COL);
	for(c=0;c<COL;c++)
	{
	    printf("%02x ",d[ l*COL+c ]);
	    if (c==7)
		printf(" ");
	}
	printf(" |");
	for(c=0;c<COL;c++) 
	{
	    printf("%c", isprintable(d[ l*COL+c ]) ? d[ l*COL+c ] : '.');
	}
	printf("|\n");
    }
}


/* **********************************************************************
 * ********************************************************************** 
 * 
 * SCSI Commands                                      
 * 
 * ********************************************************************** 
 * ********************************************************************** */


void scsi_init()
{
    int lun;
    for(lun=0; lun < scsi_max_lun; lun++)
    {
	if (scsi_lun[lun].init)
	{
	    scsi_lun[lun].init(lun);
	}
    }
}


static scsi_cmdret_t scsi_cmd_default_handler( SCSI_PARAMS )
{
    log_warning("SCSI Default handler Lun %d", lun);
    scsi_dump_cmd(cmd, sizeof(scsi_cdb6_t));

    /* send illegal request (page ) */
    memset(data, 0, SCSI_SENSE_REPLY_FIXED_SIZE);
    data[0]    = 0x80 | 0x70;       // Fixed format, current error
    data[2]    = 0x05;              // Sense key ILLEGAL REQUEST
    data[12]   = 0x20;              // ASC : Additional Sense Code   == INVALID COMMAND
    *datalen   = SCSI_SENSE_REPLY_SIZE;
    *status    = SCSI_CHECK_CONDITION;
    return SCSI_CMD_DONE;
}


scsi_cmdret_t scsi_process_command(uint8_t cont, uint8_t lun, uint8_t* cmd, uint8_t cmdlen, 
				   uint8_t *data, uint32_t datamax, uint32_t *datalen, 
				   scsi_status_t *status)
{
    int i;

    for(i=0; i < sizeof(scsi_cdb_list)/sizeof(scsi_cdb_desc_t); i++)
    {
	if (cmd[0] == scsi_cdb_list[i].cdb_op)
	{
	    return scsi_cdb_list[i].cdb_cmd(cont,lun,cmd,data,datamax,datalen,status);
	}
    }
    return scsi_cmd_default_handler(cont,lun,cmd,data,datamax,datalen,status);
}



/* ********************************************************************** */
/* INQUIRY, page 168                                                      */
/* ********************************************************************** */


static const uint8_t scsi_inquiry_answer[] = 
{
    0x00,                // peripheral qualifier | peripheral device type = Direct access block device
    0x80,                // RMB = 1, this is a removable media
    0x04,                // Version, The device complies to ANSI INCITS 351-2001 (SPC-2).
    0x02,                // ACA = 0 | HiSup = 0 | Format = 2 (SPC-2)
      31,                // bytes (additional length)
    0x00,                // SCC = 0 | ACC = 0 | TPGS = 0 | 3PC = 0 | Protect = 0
    0x00,                // BQue = 0 | EncServ = 0 | VS = 0 | MultiP = 0 | MChngr = 0 | Addr16 = 0
    0x00,                // WBus16 = 0 | SYNC = 0 | LINKED = 0 | CmdQue = 0 | VS = 0
//    SCSI_VENDOR_STRING,  //  8 bytes : T10 Vendor ID, MSB -> LSB
//    SCSI_PRODUCT_STRING, // 16 bytes : Product ID, MSB -> LSB
//    SCSI_REV_STRING      //  4 bytes : Product Revision, MSB -> LSB
};

static inline int inq_size()
{
    return 
	sizeof(scsi_inquiry_answer) +
	sizeof(scsi_vendor_string)  +
	sizeof(scsi_product_string) +
	sizeof(scsi_rev_string);
}

static scsi_cmdret_t scsi_cmd_inquiry( SCSI_PARAMS )
{
    uint8_t *ptr;
    const scsi_cdb6_t * UNUSED p = (const scsi_cdb6_t *)cmd;
    log_info("SCSI Inquiry Lun %d (EVPD = %d, Length = %d)", lun, p->cdb_info & 1, p->length);
    
    ptr  = data;
    memcpy(ptr, scsi_inquiry_answer, sizeof(scsi_inquiry_answer) );
    ptr += sizeof(scsi_inquiry_answer);
    memcpy(ptr, scsi_vendor_string,  sizeof(scsi_vendor_string)  );
    ptr += sizeof(scsi_vendor_string);
    memcpy(ptr, scsi_product_string, sizeof(scsi_product_string) );
    ptr += sizeof(scsi_product_string);
    memcpy(ptr, scsi_rev_string,     sizeof(scsi_rev_string)     );
    
    *datalen   = inq_size();
    *status    = SCSI_GOOD;
    return SCSI_CMD_DONE;
}


/* ********************************************************************** */
/* TEST UNIT READY, page 258                                              */
/* ********************************************************************** */


static scsi_cmdret_t scsi_cmd_test_unit_ready( SCSI_PARAMS )
{
    log_info("SCSI Test Unit Ready Lun %d", lun);
    *datalen   = 0;
    *status    = SCSI_GOOD;
    return SCSI_CMD_DONE;
}


/* ********************************************************************** */
/* MODE SENSE(6), page 190                                                */
/* ********************************************************************** */


static scsi_cmdret_t scsi_cmd_mode_sense6( SCSI_PARAMS )
{
    uint8_t dbd          = (cmd[1] & 0x08) >> 3;
    uint8_t pc           = (cmd[2] & 0xc0) >> 6;
    uint8_t page_code    = (cmd[2] & 0x3f); 
    uint8_t subpage_code = (cmd[3]       );
    uint8_t UNUSED allocation = (cmd[4]       );

    log_info("SCSI Mode Sense(6) Lun %d, (%d, %d, %x, %x, %d)", lun,dbd,pc,page_code,subpage_code,allocation);

    // SCSI Mode Sense(6) Lun 0, DBD 0, PC 0, page_code 3F, subpage 0, allocation 192
    if (dbd == 0 && pc == 0 && page_code == 0x3f && subpage_code == 0)
    {
	// set SENSE reply to 0
	memset(data, 0, SCSI_SENSE_REPLY_SIZE);
	*datalen  = SCSI_SENSE_REPLY_SIZE;
	*status   = SCSI_GOOD;
	return SCSI_CMD_DONE;
    }
    else
    {
	log_warning("SCSI mode sense, un-implemented request");
	*status   = SCSI_CHECK_CONDITION;
    }

    return SCSI_CMD_ERROR;
}


/* ********************************************************************** */
/* PREVENT ALLOW MEDIUM REMOVAL, page 210                                 */
/* ********************************************************************** */


static scsi_cmdret_t scsi_cmd_prevent_allow_medium_removal( SCSI_PARAMS )
{
    uint8_t UNUSED prevent = cmd[4] & 0x3;
    log_info("SCSI Prevent Allow Medium Removal %d - %s",prevent,(prevent & 0x1) ? "prevent":"allow");
    *status = SCSI_GOOD;
    return SCSI_CMD_DONE;
}


/* ********************************************************************** */
/* REQUEST SENSE, page 247                                                */
/* ********************************************************************** */


static scsi_cmdret_t scsi_cmd_request_sense( SCSI_PARAMS )
{
    uint8_t desc = cmd[1] & 0x1;

    log_info("SCSI Request Sense %d",desc);
    if (desc == 0) /* fixed format */
    {
	memset(data,0,SCSI_SENSE_REPLY_FIXED_SIZE);
	data[0]    = 0x80 | 0x70;  // Fixed format, current error
	data[7]    = 0x0a;         // additional sense length
	data[12]   = 0x00;         // ASC : Additional Sense Code == NO ADDITIONAL SENSE INFORMATION  = 00
	*datalen   = SCSI_SENSE_REPLY_FIXED_SIZE;
	*status    = SCSI_GOOD;
    }
    else
    {
	*datalen   = 0;
	*status    = SCSI_CHECK_CONDITION;
    }
    return SCSI_CMD_DONE;
}


/* ********************************************************************** */
/* READ CAPACITY(10), MMC5 page 435                                       */
/* ********************************************************************** */


static scsi_cmdret_t scsi_cmd_read_capacity10( SCSI_PARAMS )
{
    uint32_t bcount;
    uint16_t bsize;

    if (scsi_lun[lun].read_capacity)
    {
	scsi_lun[lun].read_capacity(lun, &bcount, &bsize);
    }
    else
    {
	bcount = 0;
	bsize  = 0;
    }

    bcount   = bcount - 1; // we need the last accessible sector

    data[0]  = bcount  >> 24 & 0xff;
    data[1]  = bcount  >> 16 & 0xff;
    data[2]  = bcount  >>  8 & 0xff;
    data[3]  = bcount  >>  0 & 0xff;
    data[4]  = bsize   >> 24 & 0xff;
    data[5]  = bsize   >> 16 & 0xff;
    data[6]  = bsize   >>  8 & 0xff;
    data[7]  = bsize   >>  0 & 0xff;

    *datalen = 8;
    *status  = SCSI_GOOD;

    log_info("SCSI Read Capacity(10) Lun %d: %d * %d / 0x%08x", lun, bcount, bsize, bcount);
    return SCSI_CMD_DONE;
}


/* ********************************************************************** */
/* READ10, MMC5 page 425                                                  */
/* ********************************************************************** */

static scsi_cmdret_t scsi_cmd_read10( SCSI_PARAMS )
{
    if (scsi_lun[lun].read10)
    {
	return scsi_lun[lun].read10(cont,lun,cmd,data,datamax,datalen,status);
    }

    *datalen  = 0;
    *status   = SCSI_CHECK_CONDITION;
    return SCSI_CMD_DONE;
}

/* ********************************************************************** */
/* WRITE10, MMC5 page 425                                                  */
/* ********************************************************************** */

static scsi_cmdret_t scsi_cmd_write10( SCSI_PARAMS )
{
    if (scsi_lun[lun].write10)
    {
	return scsi_lun[lun].write10(cont,lun,cmd,data,datamax,datalen,status);
    }

    *datalen  = 0;
    *status   = SCSI_CHECK_CONDITION;
    return SCSI_CMD_DONE;
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
