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
 * scsi.h
 *
 *  Created on: May 23, 2012
 *      Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#ifndef __USB_SCSI_H
#define __USB_SCSI_H

#if defined(__BIG_ENDIAN__)
#define _swab16(x) x
#define _swab32(x) x
#else
#define _swab16(x)						\
    ((uint16_t)((((uint16_t)(x) & (uint16_t)0x00ffU) << 8) |	\
		(((uint16_t)(x) & (uint16_t)0xff00U) >> 8)))

#define _swab32(x)							\
    ((uint32_t)((((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) |	\
		(((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) |	\
		(((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) |	\
		(((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24)))
#endif

static inline uint16_t msbtohost16(uint16_t s) { return _swab16(s); }
static inline uint32_t msbtohost32(uint32_t l) { return _swab32(l); }


/**
 * SCSI Common constants and enums.
 */

typedef enum {
    SCSI_GOOD                 = 0x00,
    SCSI_CHECK_CONDITION      = 0x02,
    SCSI_CONDITION_MET        = 0x04, 
    SCSI_BUSY                 = 0x08,
    SCSI_RESERVATION_CONFLICT = 0x18,
    SCSI_TASK_SET_FULL        = 0x28,
    SCSI_ACA_ACTIVE           = 0x30,
    SCSI_TASK_ABORTED         = 0x40
} scsi_status_t;


typedef enum {
    SCSI_CMD_DONE             = 0,    /* check SCSI status */
    SCSI_CMD_PARTIAL          = 1,
    SCSI_CMD_ERROR            = 2
} scsi_cmdret_t;


/* 4.3.2 fixed length CDB format [page 49] */
typedef struct 
{
    uint8_t  operation_code;
    uint8_t  cdb_info;        // 
    uint16_t lba;             // logical block address (MSB first)
    uint8_t  length;          // * length
    uint8_t  control;
} __attribute__((packed)) scsi_cdb6_t;


typedef struct
{
    uint8_t  operation_code;
    uint8_t  cdb_info;
    uint32_t lba;
    uint8_t  cdb_info_misc;
    uint16_t length;
    uint8_t  control;
} __attribute__((packed)) scsi_cdb10_t;


/**
 * SCSI Low level drivers.
 */

typedef struct{
    uint8_t        cont;
    uint8_t        lun;
    uint8_t       *cmd;
    uint8_t       *data;
    uint32_t       datamax;
    uint32_t      *datalen;
    scsi_status_t *status;
}scsi_params_t;

typedef enum {
    USB_MSC_NONE,
    USB_MSC_SD, 
    USB_MSC_MMAPFS
} scsi_lun_type_t;

typedef void          (*lun_init)          (uint8_t lun);
typedef void          (*lun_read_capacity) (uint8_t lun, uint32_t *bcount, uint16_t *bsize);
typedef scsi_cmdret_t (*lun_read10)        (scsi_params_t scsi_params);	//scsi_params modif
typedef scsi_cmdret_t (*lun_write10)       (scsi_params_t scsi_params);

typedef struct {
    scsi_lun_type_t     type;
    const void         *info;
    lun_init            init;
    lun_read_capacity   read_capacity;
    lun_read10          read10;
    lun_write10         write10;
} scsi_lun_t;


/**
 * These variables must be defined by a library or by the
 * application
 */

extern const uint8_t    scsi_vendor_string  [ 8];
extern const uint8_t    scsi_product_string [16];
extern const uint8_t    scsi_rev_string     [ 4];

extern const uint8_t    scsi_max_lun;
extern const scsi_lun_t scsi_lun[];

/**
 * Process a SCSI command on a given Logical Unit (Lun)
 * Command parameter cmd is modified if needed for next request
 * See scsi.c for initial arguments
 **/
scsi_cmdret_t scsi_process_command(scsi_params_t scsi_params);

/**
 * SCSI layer init
 */

void scsi_init();
void scsi_dump_blk(uint32_t lba, uint8_t *d, uint16_t len);

#endif
