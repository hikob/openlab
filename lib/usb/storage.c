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
 * storage.c
 *
 *  Created on: May 28, 2012
 *      Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "platform.h"
#include "storage.h"
#include "scsi.h"
#include "scsi_mmapfs.h"

#define NO_DEBUG_HEADER
#define LOG_LEVEL LOG_LEVEL_DEBUG
#include "printf.h"
#include "debug.h"

#define BLOCKSIZE 512

/* ************************************************** */
/* ************************************************** */
 
void storage_eeprom_read(void* arg, uint16_t lba, uint8_t *b)
{
    log_debug("mmapfs eeprom_read() lba %d",lba);
    memset(b,0,BLOCKSIZE);
    sprintf((char*)b,"eeprom_read() lba %d\n",lba);
}

void storage_eeprom_write(void* arg, uint16_t lba, uint8_t *b)
{
    log_debug("mmapfs eeprom_write() lba %d",lba);
    memset(b,0,BLOCKSIZE);
}

/* ************************************************** */
/* ************************************************** */

void storage_spiflash_info(void* arg, int *present, uint32_t *size)
{
    int *ptr = arg;
    *present = *ptr;
    *size    = 4096;
    //    log_debug("mmapfs spiflash_info() : %s %d bytes",(*present != 0) ? "present":"deleted",*size);
}
 
void storage_spiflash_read(void* arg, uint16_t lba, uint8_t *b)
{
    log_debug("mmapfs spiflash_read() lba %d",lba);
    memset(b,0,BLOCKSIZE);
    sprintf((char*)b,"spiflash_read() lba %d\n",lba);
}

void storage_spiflash_write(void* arg, uint16_t lba, uint8_t *b)
{
    log_debug("mmapfs spiflash_write() lba %d",lba);
    memset(b,0,BLOCKSIZE);
}

void storage_spiflash_delete(void* arg)
{
    int *ptr = arg;
    if (*ptr == 1)
    {
	log_debug("mmapfs spiflash_delete()");
	log_warning("mmapfs spiflash_delete() :: start erasing internal flash");
	*ptr = 0;
    }
    else
    {
	log_debug("mmapfs spiflash_delete() :: flash is already deleted.");
    }
}

/* ************************************************** */
/* ************************************************** */
/* ************************************************** */
