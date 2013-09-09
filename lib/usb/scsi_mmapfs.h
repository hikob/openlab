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
 * scsi_mmapfs.h
 *
 *  Created on: Jun 19, 2012
 *      Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */


#ifndef __USB_SCSI_MMAPFS_H
#define __USB_SCSI_MMAPFS_H

/**
 * SCSI Memory mapped device
 *
 * This device provides a virtual disk which contains a number
 * of entries reachable throughout their associated FAT descriptor.
 */

void          scsi_mmapfs_init          ( uint8_t lun );
void          scsi_mmapfs_read_capacity ( uint8_t lun, uint32_t *bcount, uint16_t *bsize );
scsi_cmdret_t scsi_mmapfs_read10        ( scsi_params_t scsi_params);
scsi_cmdret_t scsi_mmapfs_write10       ( scsi_params_t scsi_params);


/**
 * MMap device descriptor that can be registered in
 * the SCSI MMap device
 */

typedef void (*mmap_info)   (void* arg, int *present, uint32_t *size);
typedef void (*mmap_read)   (void* arg, uint16_t lba, uint8_t *b);
typedef void (*mmap_write)  (void* arg, uint16_t lba, uint8_t *b);
typedef void (*mmap_unlink) (void* arg);

typedef struct {
    void*       arg;
    char        name[8];
    char        ext[3];
    uint32_t    maxsize;    // bytes
    mmap_info   info;
    mmap_read   read;
    mmap_write  write;
    mmap_unlink unlink;
} mmap_entry_t;

typedef struct {
    uint16_t      size;
    const mmap_entry_t *files;
} mmapfs_t;
 
#endif
