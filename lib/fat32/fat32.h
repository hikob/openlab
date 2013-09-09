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

/**
 * \file fat32.h
 * \date Aug 17, 2011
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef FAT32__H_
#define FAT32__H_

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint32_t boot_sect;      // Boot sector
    uint8_t num_fat;         // Number of FAT
    uint32_t sect_per_fat;   // Number of sectors per FAT
    uint8_t sect_per_clust;  // Number of sectors per cluster
    uint32_t num_sect;       // Total number of sectors
    uint32_t max_clust;      // Maximum cluster index

    uint16_t start;          // FAT start sector
    uint32_t root_start;     // Start sector of root directory
    uint32_t data_start;     // Start sector of data
} fat32_t;

typedef struct
{
    uint8_t name[9];
    uint8_t ext[4];
    uint8_t attrib;
    uint8_t hour;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t ms;
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint32_t start_cluster;
    uint32_t size;
} dir_entry_t;

typedef enum
{
    FAT32_OK              = 0,
    FAT32_NOT_BOOT_RECORD = 1,
    FAT32_NOT_FAT32       = 2,
    FAT32_FILE_NOT_FOUND  = 3,
    FAT32_FILE_EXISTS     = 4,
    FAT32_NO_FREE_CLUSTER = 5,
    FAT32_WRONG_CLUSTER   = 6,
    FAT32_FS_ERROR        = 7
} fat32_error_t;

extern fat32_t fat;

fat32_error_t fat32_init();

fat32_error_t fat32_mount();

fat32_error_t fat32_get_volume_name(uint8_t *name);

fat32_error_t fat32_open(uint8_t *filename, uint32_t *sector, uint16_t *index, dir_entry_t *dir);
fat32_error_t fat32_create(uint8_t *filename, uint32_t *sector, uint16_t *index, dir_entry_t *dir);
fat32_error_t fat32_delete(uint8_t *filename);
bool fat32_file_exists(uint8_t *filename);

fat32_error_t fat32_set_file_size(uint8_t *filename, uint32_t size);
fat32_error_t fat32_get_file_size(uint8_t *filename, uint32_t *size);

fat32_error_t fat32_read_dir_entry(uint32_t sect, uint16_t index, dir_entry_t *dir);

uint32_t fat32_get_next_cluster(uint32_t cluster);
fat32_error_t fat32_set_next_cluster(uint32_t cluster, uint32_t next_cluster);
uint32_t fat32_find_empty_cluster();
fat32_error_t fat32_free_cluster(uint32_t cluster);

uint32_t fat32_first_sector(uint32_t cluster);

#endif
