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
#include "debug.h"

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
 * see [Global ] http://en.wikipedia.org/wiki/SCSI_command
 * and [Ref Man] http://www.seagate.com/staticfiles/support/disc/manuals/Interface%20manuals/100293068c.pdf
 * and [ MMC-5 ] http://www.13thmonkey.org/documentation/SCSI/mmc5r02c.pdf
 * and [ SPC-2 ] http://www.13thmonkey.org/documentation/SCSI/spc2r20.pdf
 *
 */



typedef enum {
    SCSI_CDB6,
    SCSI_CDB10
} scsi_cdb_type_t;


//power types used for start_stop_unit (table 169 in Seagate)
enum{
	START_VALID     = 0x00,
	ACTIVE          = 0x01,
	IDLE            = 0x02,
	STANDBY         = 0x03,
	/* 0x05 is obsolete */
	LU_CONTROL      = 0x07,
	FORCE_ILDE_0    = 0x0A,
	FORCE_STANDBY_0 = 0x0B,
	//all others are reserved
};

typedef enum {
    SCSI_TEST_UNIT_READY              = 0x00,
    SCSI_REQUEST_SENSE                = 0x03,
    SCSI_INQUIRY                      = 0x12,
    SCSI_MODE_SELECT6                 = 0x15,
    SCSI_MODE_SENSE6                  = 0x1a,
    SCSI_START_STOP                   = 0x1b, /* page 205 in Seagate */
    SCSI_PREVENT_ALLOW_MEDIUM_REMOVAL = 0x1e,
    SCSI_READ_FORMAT_CAPACITIES       = 0x23, /* defined in MMC5 */
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
#define SCSI_SENSE_CACHING_REPLY_SIZE  19
#define SCSI_SENSE_INFOEXC_REPLY_SIZE  12


typedef scsi_cmdret_t (*scsi_cdb_cmd_t)(scsi_params_t scsi_params);

static  scsi_cmdret_t scsi_cmd_test_unit_ready             (scsi_params_t scsi_params);
static  scsi_cmdret_t scsi_cmd_request_sense               (scsi_params_t scsi_params);
static  scsi_cmdret_t scsi_cmd_inquiry                     (scsi_params_t scsi_params);
static  scsi_cmdret_t scsi_cmd_mode_select6                (scsi_params_t scsi_params);
static  scsi_cmdret_t scsi_cmd_mode_sense6                 (scsi_params_t scsi_params);
static  scsi_cmdret_t scsi_cmd_start_stop_unit             (scsi_params_t scsi_params);
static  scsi_cmdret_t scsi_cmd_prevent_allow_medium_removal(scsi_params_t scsi_params);
static  scsi_cmdret_t scsi_cmd_read_capacity10             (scsi_params_t scsi_params);
static  scsi_cmdret_t scsi_cmd_read10                      (scsi_params_t scsi_params);
static  scsi_cmdret_t scsi_cmd_write10                     (scsi_params_t scsi_params);
static  scsi_cmdret_t scsi_cmd_default_handler             (scsi_params_t scsi_params);
static  scsi_cmdret_t scsi_cmd_read_format_capacities      (scsi_params_t scsi_params);

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
    { SCSI_WRITE10,                      SCSI_CDB10, scsi_cmd_write10                      },
    { SCSI_MODE_SELECT6,                 SCSI_CDB6,  scsi_cmd_mode_select6                 },
    { SCSI_START_STOP,                   SCSI_CDB6,  scsi_cmd_start_stop_unit              },
    { SCSI_READ_FORMAT_CAPACITIES,       SCSI_CDB10, scsi_cmd_read_format_capacities       },
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
	    if(i == len-1)
		DBG("%02x\n", cmd[i]);
	    else
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
    for(lun=0; lun <= scsi_max_lun; lun++)
    {
	if (scsi_lun[lun].init)
	{
	    scsi_lun[lun].init(lun);
	}
    }
}


static scsi_cmdret_t scsi_cmd_default_handler(scsi_params_t scsi_params)
{
    log_warning("SCSI Default handler Lun %d", scsi_params.lun);
    scsi_dump_cmd(scsi_params.cmd, sizeof(scsi_cdb6_t));

    /* send illegal request (page ) */
    memset(scsi_params.data, 0, SCSI_SENSE_REPLY_FIXED_SIZE);
    scsi_params.data[0]     = 0x80 | 0x70;       // Fixed format, current error
    scsi_params.data[2]     = 0x05;              // Sense key ILLEGAL REQUEST
    scsi_params.data[12]    = 0x20;              // ASC : Additional Sense Code   == INVALID COMMAND
    *scsi_params.datalen    = SCSI_SENSE_REPLY_SIZE;
    *scsi_params.status     = SCSI_CHECK_CONDITION;
    return SCSI_CMD_DONE;
}


scsi_cmdret_t scsi_process_command(scsi_params_t scsi_params)
{
    int i;

    for(i=0; i < sizeof(scsi_cdb_list)/sizeof(scsi_cdb_desc_t); i++)
    {
	if (scsi_params.cmd[0] == scsi_cdb_list[i].cdb_op)
	{
	    return scsi_cdb_list[i].cdb_cmd(scsi_params);
	}
    }
    return scsi_cmd_default_handler(scsi_params);
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

static scsi_cmdret_t scsi_cmd_inquiry(scsi_params_t scsi_params)
{
    uint8_t *ptr;
    const scsi_cdb6_t * UNUSED p = (const scsi_cdb6_t *)scsi_params.cmd;
    log_info("SCSI Inquiry Lun %d (EVPD = %d, Length = %d)", scsi_params.lun, p->cdb_info & 1, p->length);
    
    ptr  = scsi_params.data;
    memcpy(ptr, scsi_inquiry_answer, sizeof(scsi_inquiry_answer) );
    ptr += sizeof(scsi_inquiry_answer);
    memcpy(ptr, scsi_vendor_string,  sizeof(scsi_vendor_string)  );
    ptr += sizeof(scsi_vendor_string);
    memcpy(ptr, scsi_product_string, sizeof(scsi_product_string) );
    ptr += sizeof(scsi_product_string);
    memcpy(ptr, scsi_rev_string,     sizeof(scsi_rev_string)     );
    
    *scsi_params.datalen   = inq_size();
    *scsi_params.status    = SCSI_GOOD;
    return SCSI_CMD_DONE;
}


/* ********************************************************************** */
/* TEST UNIT READY, page 258                                              */
/* ********************************************************************** */


static scsi_cmdret_t scsi_cmd_test_unit_ready(scsi_params_t scsi_params)
{
    log_info("SCSI Test Unit Ready Lun %d", scsi_params.lun);
    scsi_params.datalen   = 0;
    scsi_params.status    = SCSI_GOOD;
    return SCSI_CMD_DONE;
}

/* ********************************************************************** */
/* MODE SELECT(6)   added but not called                                  */
/* ********************************************************************** */

static scsi_cmdret_t scsi_cmd_mode_select6(scsi_params_t scsi_params)
{
    uint8_t pf __attribute__((__unused__)) = (scsi_params.cmd[1] & 0x10) >> 4;
    uint8_t sp __attribute__((__unused__)) = (scsi_params.cmd[1] & 0x01);
    uint8_t param_list_length __attribute__((__unused__)) = (scsi_params.cmd[4]);
    uint8_t control __attribute__((__unused__)) = (scsi_params.cmd[5]);

    log_warning("SCSI Mode Select lun %d, (%x, %x, %x, %x), unimplemented", scsi_params.lun, pf, sp, param_list_length, control);

    return SCSI_CMD_ERROR;
}

/* ********************************************************************** */
/* READ FORMAT CAPACITIES    (Table 458 in MMC-5)                         */
/* ********************************************************************** */

static scsi_cmdret_t scsi_cmd_read_format_capacities(scsi_params_t scsi_params)
{
    uint16_t allocation_length = scsi_params.cmd[7] << 4 & scsi_params.cmd[8];
    uint8_t control __attribute__((__unused__)) = scsi_params.cmd[9];

    if (allocation_length == 0)
    {
	// no data shall be transfered (not an error)
	return SCSI_CMD_DONE;
    }
    else
    {
	// unimplemented TODO
	log_warning("SCSI read format capacities lun %d,(%x, %x) unimplemented", scsi_params.lun, allocation_length, control);
    }
    return SCSI_CMD_ERROR;
}

/* ********************************************************************** */
/* START STOP UNIT    (Table 168 in Seagate)                              */
/* ********************************************************************** */

static scsi_cmdret_t scsi_cmd_start_stop_unit(scsi_params_t scsi_params)
{
    uint8_t immed 						 = (scsi_params.cmd[1] & 0x01);
    uint8_t power_condition_modifier __attribute__((__unused__)) = (scsi_params.cmd[3] & 0x0F);
    uint8_t power_condition __attribute__((__unused__)) 	 = (scsi_params.cmd[4] & 0xF0) >> 4;
    uint8_t no_flush __attribute__((__unused__)) 		 = (scsi_params.cmd[4] & 0x04) >> 2;
    uint8_t loej __attribute__((__unused__)) 			 = (scsi_params.cmd[4] & 0x02) >> 1;
    uint8_t start __attribute__((__unused__)) 			 = (scsi_params.cmd[4] & 0x01);
    uint8_t control __attribute__((__unused__)) 		 = (scsi_params.cmd[5]);

    if (immed == 0)
    {
	// return status after operation is completed
	log_warning("SCSI start stop unit lun %d, (%x, %x, %x, %x, %x, %x, %x), unimplemented", scsi_params.lun, immed, power_condition_modifier,
		power_condition, no_flush, loej, start, control);
	// TODO process command

	return SCSI_CMD_DONE;
    }
    else
    {
	// return status as soon as the CDB has been validated
	// TODO maybe add some additional check
	return SCSI_CMD_DONE;
    }
}

/* ********************************************************************** */
/* MODE SENSE(6), page 190 (page 97 in Seagate)                           */
/* ********************************************************************** */


static scsi_cmdret_t scsi_cmd_mode_sense6(scsi_params_t scsi_params)
{
    uint8_t dbd               = (scsi_params.cmd[1] & 0x08) >> 3;
    uint8_t pc                = (scsi_params.cmd[2] & 0xc0) >> 6;
    uint8_t page_code         = (scsi_params.cmd[2] & 0x3f); 
    uint8_t subpage_code      = (scsi_params.cmd[3]       );
    uint8_t UNUSED allocation = (scsi_params.cmd[4]       );

    log_not_implemented("SCSI Mode Sense(6) Lun %d, (%d, %d, %x, %x, %d)", scsi_params.lun,dbd,pc,page_code,subpage_code,allocation);

    // SCSI Mode Sense(6) Lun 0, DBD 0, PC 0, page_code 3F, subpage 0, allocation 192
    if (dbd == 0 && pc == 0 && page_code == 0x3f && subpage_code == 0)
    {
	// set SENSE reply to 0
	memset(scsi_params.data, 0, SCSI_SENSE_REPLY_SIZE);
	*scsi_params.datalen  = SCSI_SENSE_REPLY_SIZE;
	*scsi_params.status   = SCSI_GOOD;
	return SCSI_CMD_DONE;
    }
    //  Information Exception Control (=> INFOEXC) pageCode 0x1C (table 283 in Seagate) TODO
    else if (dbd == 0 && pc == 0 && page_code == 0x1C && subpage_code == 0)
    {
	memset(scsi_params.data, 0, SCSI_SENSE_INFOEXC_REPLY_SIZE);
	scsi_params.data[0]   = 0x1C;
	scsi_params.data[1]   = 0x0A;
	*scsi_params.datalen  = SCSI_SENSE_INFOEXC_REPLY_SIZE;
	*scsi_params.status   = SCSI_GOOD;

	return SCSI_CMD_DONE;
    }
    //  Caching parameter page (table 270 in Seagate) TODO
    else if (dbd == 0 && pc == 0 && page_code == 0x08 && subpage_code == 0)
    {
	memset(scsi_params.data, 0, SCSI_SENSE_CACHING_REPLY_SIZE);
	scsi_params.data[0]   = 0x08;
	scsi_params.data[1]   = 0x12;
	scsi_params.data[2]   = 0x85;
	scsi_params.data[9]   = 0x02;
	scsi_params.data[11]  = 0x08;
	scsi_params.data[13]  = 0x08;
	*scsi_params.datalen  = SCSI_SENSE_CACHING_REPLY_SIZE;
	*scsi_params.status   = SCSI_GOOD;

	return SCSI_CMD_DONE;
    }
    else
    {
	log_warning("SCSI mode sense6, un-implemented request");
	*scsi_params.status   = SCSI_CHECK_CONDITION;
    }

    return SCSI_CMD_ERROR;
}


/* ********************************************************************** */
/* PREVENT ALLOW MEDIUM REMOVAL, page 210                                 */
/* ********************************************************************** */


static scsi_cmdret_t scsi_cmd_prevent_allow_medium_removal(scsi_params_t scsi_params)
{
    uint8_t UNUSED prevent = scsi_params.cmd[4] & 0x3;
    log_info("SCSI Prevent Allow Medium Removal %d - %s",prevent,(prevent & 0x1) ? "prevent":"allow");
    scsi_params.status = SCSI_GOOD;
    return SCSI_CMD_DONE;
}


/* ********************************************************************** */
/* REQUEST SENSE, page 247                                                */
/* ********************************************************************** */


static scsi_cmdret_t scsi_cmd_request_sense(scsi_params_t scsi_params)
{
    uint8_t desc = scsi_params.cmd[1] & 0x1;

    log_info("SCSI Request Sense %d",desc);
    if (desc == 0) /* fixed format */
    {
	memset(scsi_params.data,0,SCSI_SENSE_REPLY_FIXED_SIZE);
	scsi_params.data[0]     = 0x80 | 0x70;  // Fixed format, current error
	scsi_params.data[7]     = 0x0a;         // additional sense length
	scsi_params.data[12]    = 0x00;         // ASC : Additional Sense Code == NO ADDITIONAL SENSE INFORMATION  = 00
	*scsi_params.datalen    = SCSI_SENSE_REPLY_FIXED_SIZE;
	*scsi_params.status     = SCSI_GOOD;
    }
    else
    {
	*scsi_params.datalen   = 0;
	*scsi_params.status    = SCSI_CHECK_CONDITION;
    }
    return SCSI_CMD_DONE;
}


/* ********************************************************************** */
/* READ CAPACITY(10), MMC5 page 435                                       */
/* ********************************************************************** */


static scsi_cmdret_t scsi_cmd_read_capacity10(scsi_params_t scsi_params)
{
    uint32_t bcount;
    uint16_t bsize;

    if (scsi_lun[scsi_params.lun].read_capacity)
    {
	scsi_lun[scsi_params.lun].read_capacity(scsi_params.lun, &bcount, &bsize);
    }
    else
    {
	bcount = 0;
	bsize  = 0;
    }

    bcount   = bcount - 1; // we need the last accessible sector

    scsi_params.data[0]  = bcount  >> 24 & 0xff;
    scsi_params.data[1]  = bcount  >> 16 & 0xff;
    scsi_params.data[2]  = bcount  >>  8 & 0xff;
    scsi_params.data[3]  = bcount  >>  0 & 0xff;
    scsi_params.data[4]  = bsize   >> 24 & 0xff;
    scsi_params.data[5]  = bsize   >> 16 & 0xff;
    scsi_params.data[6]  = bsize   >>  8 & 0xff;
    scsi_params.data[7]  = bsize   >>  0 & 0xff;

    *scsi_params.datalen = 8;
    *scsi_params.status  = SCSI_GOOD;

    log_info("SCSI Read Capacity(10) Lun %d: %d * %d / 0x%08x", scsi_params.lun, bcount, bsize, bcount);
    return SCSI_CMD_DONE;
}


/* ********************************************************************** */
/* READ10, MMC5 page 425                                                  */
/* ********************************************************************** */

static scsi_cmdret_t scsi_cmd_read10(scsi_params_t scsi_params)
{
    if (scsi_lun[scsi_params.lun].read10)
    {
	return scsi_lun[scsi_params.lun].read10(scsi_params);
    }

    *scsi_params.datalen  = 0;
    *scsi_params.status   = SCSI_CHECK_CONDITION;
    return SCSI_CMD_DONE;
}

/* ********************************************************************** */
/* WRITE10, MMC5 page 425                                                  */
/* ********************************************************************** */

static scsi_cmdret_t scsi_cmd_write10(scsi_params_t scsi_params)
{
    if (scsi_lun[scsi_params.lun].write10)
    {
	return scsi_lun[scsi_params.lun].write10(scsi_params);
    }

    *scsi_params.datalen  = 0;
    *scsi_params.status   = SCSI_CHECK_CONDITION;
    return SCSI_CMD_DONE;
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
