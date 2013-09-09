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
 * scsi_sd.c
 *
 *  Created on: May 23, 2012
 *      Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "platform.h"
#include "sdio.h"

#include "scsi.h"
#include "scsi_sd.h"

#define NO_DEBUG_HEADER
#define LOG_LEVEL LOG_LEVEL_WARNING
#include "printf.h"
#include "debug.h"

#if (LOG_LEVEL < LOG_LEVEL_DEBUG)
#define DBG(x...)   printf(x)
#else
#define DBG(x...)   do { } while(0)
#endif // LOG_LEVEL


#define BLOCKSIZE                    512ul

/* used in sdio transfers */
static volatile bool       sd_transfer_ended = false;

/* ********************************************************************** */
/* Init                                                                   */
/* ********************************************************************** */

void scsi_sd_init(uint8_t lun)
{
    const sdio_t* sdio = (const sdio_t*)scsi_lun[lun].info;

    if (sd_init(*sdio) != SD_NO_ERROR)
    {
	log_error("MSC sd_init() error");
    }
}

/* ********************************************************************** */
/* Async I/O Callback                                                     */
/* ********************************************************************** */

void sd_transfer_handler(handler_arg_t arg)
{
    sd_transfer_ended = true;
}

/* ********************************************************************** */
/* READ CAPACITY(10), MMC5 page 435                                       */
/* ********************************************************************** */

void scsi_sd_read_capacity(uint8_t lun, uint32_t *bcount, uint16_t *bsize) 
{
    const sdio_t *sdio = (const sdio_t*)scsi_lun[lun].info;

    *bcount = sd_get_size(*sdio);
    *bsize  = 512u;
}

/* ********************************************************************** */
/* READ10, MMC5 page 425                                                  */
/* ********************************************************************** */

scsi_cmdret_t scsi_sd_read10( scsi_params_t scsi_params )
{
    const sdio_t   *sdio     = (const sdio_t*)scsi_lun[scsi_params.lun].info;
    scsi_cdb10_t   *cdb10    = (scsi_cdb10_t *)scsi_params.cmd;
    //uint8_t       DPO      = (cdb10->cdb_info >> 4) & 0x1;
    //uint8_t       FUA      = (cdb10->cdb_info >> 3) & 0x1;
    uint32_t        lba      = msbtohost32(cdb10->lba);
    uint32_t        nblocks  = msbtohost16(cdb10->length);
    uint32_t        sdsize   = sd_get_size(*sdio);
    sd_card_type_t  sdtype   = sd_get_type(*sdio);
    sd_error_t      sdret;
    scsi_cmdret_t   ret;
    uint32_t        la       = (sdtype != SDHC) ? lba * BLOCKSIZE : lba;

    sdret             = SD_NO_ERROR;
    sd_transfer_ended = false;
    sd_set_transfer_handler(*sdio, sd_transfer_handler);

    if ((lba + nblocks - 1) >= sdsize)
    {
	log_error("SCSI Read(10), read beyond limit (limit 0x%08x, request 0x%08x / +%d)",sdsize,lba,nblocks);
	*scsi_params.status = SCSI_CHECK_CONDITION;
	return SCSI_CMD_DONE;
    }

    if (scsi_params.datamax < BLOCKSIZE)
    {
	log_error("SCSI Read(10), read buffer is smaller than a single sector (%d/%d bytes)",scsi_params.datalen,BLOCKSIZE);
	*scsi_params.status = SCSI_CHECK_CONDITION;
	return SCSI_CMD_ERROR;
    }

    sdret = sd_read_single_block(*sdio, la, scsi_params.data);

    if (scsi_params.cont == 0)
    {
	log_info("SCSI Read(10) type %d, lba 0x%08x, length %d",sdtype,lba,nblocks);
    }
    else
    {
	DBG("-%d-\n",scsi_params.cont);
    }

    if (sdret != SD_NO_ERROR)
    {
	log_error("SCSI Read Command Error on SD %d",sdret);
	*scsi_params.status = SCSI_CHECK_CONDITION;
	return SCSI_CMD_DONE;
    }

    while (!sd_transfer_ended) 
    {
	;
    }

    sdret = sd_get_status(*sdio);

    if (sdret != SD_NO_ERROR)
    {
	log_error("SCSI Read Error on SD (lba 0x%08x) ret %d",lba,sdret);
	*scsi_params.status = SCSI_CHECK_CONDITION;
	return SCSI_CMD_DONE;
    }

    switch (nblocks)
    {
    case 0:
	*scsi_params.status        = SCSI_CHECK_CONDITION;
    ret          		       = SCSI_CMD_ERROR;
	break;
    case 1:
	*scsi_params.datalen       = BLOCKSIZE;
	*scsi_params.status        = SCSI_GOOD;
	ret          			   = SCSI_CMD_DONE;
	break;
    default:
	cdb10->lba    			   = msbtohost32( lba     + 1 );
	cdb10->length			   = msbtohost16( nblocks - 1 );
	*scsi_params.datalen       = BLOCKSIZE;
	*scsi_params.status        = SCSI_GOOD;
	ret           			   = SCSI_CMD_PARTIAL;
	break;
    }
    return ret;
}

/* ********************************************************************** */
/* WRITE10, MMC5 page 425                                                  */
/* ********************************************************************** */

scsi_cmdret_t scsi_sd_write10( scsi_params_t scsi_params )
{
    const sdio_t *sdio = (const sdio_t*)scsi_lun[scsi_params.lun].info;

    scsi_cdb10_t   *cdb10    = (scsi_cdb10_t *)scsi_params.cmd;
    //uint8_t       DPO      = (cdb10->cdb_info >> 4) & 0x1;
    //uint8_t       FUA      = (cdb10->cdb_info >> 3) & 0x1;
    uint32_t        lba      = msbtohost32(cdb10->lba);
    uint32_t        nblocks  = msbtohost16(cdb10->length);
    uint32_t        sdsize   = sd_get_size(*sdio);
    sd_card_type_t  sdtype   = sd_get_type(*sdio);
    sd_error_t      sdret;
    scsi_cmdret_t   ret;
    uint32_t        la       = (sdtype != SDHC) ? lba * BLOCKSIZE : lba;

    sdret             = SD_NO_ERROR;
    sd_transfer_ended = false;
    sd_set_transfer_handler(*sdio, sd_transfer_handler);

    if ((lba + nblocks - 1) >= sdsize)
    {
	log_error("SCSI Write(10), write beyond limit (limit 0x%08x, request 0x%08x / +%d)",sdsize,lba,nblocks);
	*scsi_params.status = SCSI_CHECK_CONDITION;
	return SCSI_CMD_DONE;
    }

    if (scsi_params.datamax < BLOCKSIZE)
    {
	log_error("SCSI Write(10), write buffer is smaller than a single sector (%d/%d bytes)",scsi_params.datamax,BLOCKSIZE);
	*scsi_params.status = SCSI_CHECK_CONDITION;
	return SCSI_CMD_ERROR;
    }

    if (scsi_params.cont == 0)
    {
	log_info("SCSI Write(10) type %d, lba 0x%08x, length %d",sdtype,lba,nblocks);
	*scsi_params.status = SCSI_GOOD;
	return SCSI_CMD_PARTIAL;
    }
    else
    {
	DBG("-%d-\n",cont);

#define TEST_WRITE 0
#if TEST_WRITE == 1
	scsi_dump_blk(lba, data, datamax);
	sdret             = SD_NO_ERROR;
	sd_transfer_ended = true;
#else
        sdret = sd_write_single_block(*sdio, la, scsi_params.data);
#endif
    }

    if (sdret != SD_NO_ERROR)
    {
	log_error("SCSI Write Command Error on SD %d",sdret);
	*scsi_params.status = SCSI_CHECK_CONDITION;
	return SCSI_CMD_DONE;
    }

    while (!sd_transfer_ended)
    {
	;
    }

    sdret = sd_get_status(*sdio);

    if (sdret != SD_NO_ERROR)
    {
	log_error("SCSI Write Error on SD (lba 0x%08x) ret %d",lba,sdret);
	*scsi_params.status = SCSI_CHECK_CONDITION;
	return SCSI_CMD_DONE;
    }

    switch (nblocks)
    {
    case 0:
	*scsi_params.status        = SCSI_CHECK_CONDITION;
	ret            = SCSI_CMD_ERROR;
	break;
    case 1:
	*scsi_params.datalen       = BLOCKSIZE;
	*scsi_params.status        = SCSI_GOOD;
	ret            = SCSI_CMD_DONE;
	break;
    default:
	cdb10->lba     = msbtohost32( lba     + 1 );
	cdb10->length  = msbtohost16( nblocks - 1 );
	*scsi_params.datalen       = BLOCKSIZE;
	*scsi_params.status        = SCSI_GOOD;
	ret            = SCSI_CMD_PARTIAL;
	break;
    }
    return ret;
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
