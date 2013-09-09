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

#ifndef __STORAGE_H
#define __STORAGE_H

/* ************************************************** */
/* ************************************************** */

void storage_eeprom_read     (void* arg, uint16_t lba, uint8_t *b);
void storage_eeprom_write    (void* arg, uint16_t lba, uint8_t *b);

void storage_spiflash_info   (void* arg, int *present, uint32_t *size);
void storage_spiflash_read   (void* arg, uint16_t lba, uint8_t *b);
void storage_spiflash_write  (void* arg, uint16_t lba, uint8_t *b);
void storage_spiflash_delete (void* arg);

/* ************************************************** */
/* ************************************************** */

#endif
