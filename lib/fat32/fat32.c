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
 * Copyright (C) 2012 HiKoB. All rights reserved.
 */

/*
 * fat32.c
 *
 *  Created on: Aug 17, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "fat32.h"
#include "buf_util.h"
#include "fs.h"
#include "debug.h"

fat32_t fat;
static uint32_t last_free_cluster = 0;

fat32_error_t fat32_init()
{
    if (fs_init() != FS_OK)
    {
        return FAT32_FS_ERROR;
    }

    return FAT32_OK;
}

static uint8_t check_fs()
{
    uint8_t buf[8];

    /** Check if the page pointed by fat.boot_sect is indeed a boot sector **/
    // Read the last two bytes of the boot sector
    if (fs_read(fat.boot_sect, 0x1FE, buf, 2) != 2)
    {
        return FAT32_FS_ERROR;
    }

    // The last two bytes should be 0x55 0xAA
    if (!cmp(buf, (uint8_t *)"\125\252", 2))
    {
        return FAT32_NOT_BOOT_RECORD;
    }

    /** Check FAT version **/
    // Read the FAT version string (at offset 0x52 in the boot sector)
    if (fs_read(fat.boot_sect, 0x52, buf, 8) != 8)
    {
        return FAT32_FS_ERROR;
    }

    // The FAT version string should be "FAT32   "
    if (!cmp(buf, (uint8_t *)"FAT32   ", 8))
    {
        return FAT32_NOT_FAT32;
    }

    return FAT32_OK;
}

fat32_error_t fat32_mount()
{
    uint8_t buf[4];
    fat32_error_t ret;

    /** Check if the file system is a FAT32 or not **/
    // First see if the first page is the boot sector
    fat.boot_sect = 0;

    if ((ret = check_fs()) == FAT32_NOT_FAT32)
    {
        // If not it means that the disk is partitioned, thus we must check if the patition is FAT32

        // Read the MBR partition type
        if (fs_read(fat.boot_sect, 0x1C2, buf, 1) != 1)
        {
            return FAT32_FS_ERROR;
        }

        // The MBR partition type must be 0x0B or 0x0C
        if ((buf[0] == 0x0B) || (buf[0] == 0x0C))
        {
            // In that case read the address of the boot sector
            if (fs_read(fat.boot_sect, 0x1C6, buf, 4) != 4)
            {
                return FAT32_FS_ERROR;
            }

            fat.boot_sect = read32(buf);

            // Check that the boot sector is consistent
            ret = check_fs();
        }
    }

    // If there is still an error, stop mounting operation and return error
    if (ret != FAT32_OK)
    {
        return ret;
    }

    // Read the number of sectors per cluster
    if (fs_read(fat.boot_sect, 0x0d, &(fat.sect_per_clust), 1) != 1)
    {
        return FAT32_FS_ERROR;
    }

    // Read the number of reserved sectors, the start address of the FS is updated in consequence
    if (fs_read(fat.boot_sect, 0x0e, buf, 2) != 2)
    {
        return FAT32_FS_ERROR;
    }

    fat.start = fat.boot_sect + read16(buf);

    // Read the number of file allocation table
    if (fs_read(fat.boot_sect, 0x10, &(fat.num_fat), 1) != 1)
    {
        return FAT32_FS_ERROR;
    }

    // Read the total number of sectors
    if (fs_read(fat.boot_sect, 0x20, buf, 4) != 4)
    {
        return FAT32_FS_ERROR;
    }

    fat.num_sect = read32(buf);

    // Read the number of sectors per file allocation table
    if (fs_read(fat.boot_sect, 0x24, buf, 4) != 4)
    {
        return FAT32_FS_ERROR;
    }

    fat.sect_per_fat = read32(buf);
    // Compute the data start sector
    fat.data_start = fat.start + fat.sect_per_fat * fat.num_fat;

    // Read the cluster number of root directory and compute the actual sector address
    if (fs_read(fat.boot_sect, 0x2c, buf, 4) != 4)
    {
        return FAT32_FS_ERROR;
    }

    fat.root_start = fat.data_start + read32(buf) - 2;
    // Compute the maximum cluster index
    fat.max_clust = (fat.num_sect - fat.data_start + fat.boot_sect) / fat.sect_per_clust + 2;

    return FAT32_OK;
}

uint32_t fat32_get_next_cluster(uint32_t cluster)
{
    uint8_t buf[4];

    // If the requested cluster is not allowed, return an error value
    if ((cluster < 2) || (cluster > fat.max_clust))
    {
        return 1;
    }

    // Read the address of the next cluster
    // A cluster with id n is located in (n / 128) th sector of the FAT
    // In this sector it locate at index (n % 128) * 4
    if (fs_read(fat.start + (cluster >> 7), (cluster & 0x7F) << 2, buf, 4) != 4)
    {
        return 1;
    }

    return read32(buf);
}

fat32_error_t fat32_set_next_cluster(uint32_t cluster, uint32_t next_cluster)
{
    uint8_t buf[4];

    // Check if both parameters are correct
    if (((cluster != 0) && ((cluster < 2) || (cluster > fat.max_clust))) || (next_cluster == 1) || (next_cluster > fat.max_clust))
    {
        return FAT32_WRONG_CLUSTER;
    }

    // Write the address of the next cluster
    // A cluster with id n is located in (n / 128) th sector of the FAT
    // In this sector it locate at index (n % 128) * 4
    write32(buf, next_cluster);

    if (fs_write(fat.start + (cluster >> 7), (cluster & 0x7F) << 2, buf, 4) != 4)
    {
        return FAT32_FS_ERROR;
    }

    return FAT32_OK;
}

inline fat32_error_t fat32_free_cluster(uint32_t cluster)
{
    // Freeing a cluster is equivalent to set its successor to 0
    return fat32_set_next_cluster(cluster, 0);
}

uint32_t fat32_find_empty_cluster()
{
    uint32_t current_sector = fat.start;
    uint16_t i, idx = 0;
    uint8_t buf[4];

    // If we know the last free cluster, there is no need to scan the whole FAT,
    // we will scan from this point to improve the global search time.
    // To ensure the best performance, we must ensure that the disk is not fragmented,
    // in that case the search time is constant
    if (last_free_cluster != 0)
    {
        current_sector = fat.start + (last_free_cluster >> 7);
        idx = (last_free_cluster & 0x7F) << 2;
    }

    // Scan the file allocation table to find an empty cluster
    // Look in every sector of the file allocation table
    while (current_sector < ((uint32_t)fat.start) + fat.sect_per_fat)
    {
        // In each sector scan successors for every cluster
        for (i = idx; i < 512; i += 4)
        {
            // Read the cluster successor, if there is an error while reading return a forbidden value
            if (fs_read(current_sector, i, buf, 4) != 4)
            {
                last_free_cluster = 0;
                return 1;
            }

            // If the successor is 0, then it means that the cluster is free
            if (read32(buf) == 0)
            {
                // Reserve it and indicate that it has no successor
                write32(buf, 0x0FFFFFF8);

                // Write the new successor value, if there is an error while writing return a forbidden value
                if (fs_write(current_sector, i, buf, 4) != 4)
                {
                    last_free_cluster = 0;
                    return 1;
                }

                // Return the cluster index
                return last_free_cluster;
            }

            last_free_cluster++;
        }

        idx = 0;
        current_sector++;
    }

    // If we got out of the loop it means that the file system is full
    // Indicates the error by returning an error value
    last_free_cluster = 0;
    return 1;
}

static fat32_error_t fat32_find(uint8_t *name, uint8_t *ext, uint32_t *sector, uint16_t *index, dir_entry_t *dir)
{
    uint32_t current_cluster = 2, s;
    uint16_t idx;
    fat32_error_t ret = FAT32_FILE_NOT_FOUND, i;
    dir_entry_t d;

    if (sector == NULL)
    {
        sector = &s;
    }

    if (index == NULL)
    {
        index = &idx;
    }

    if (dir == NULL)
    {
        dir = &d;
    }

    do
    {
        *sector = fat.root_start + fat.sect_per_clust * (current_cluster - 2);

        for (i = 0; i < fat.sect_per_clust; i++)
        {
            for (*index = 0; *index < 512; *index += 32)
            {
                ret = fat32_read_dir_entry(*sector, *index, dir);

                if (ret != FAT32_OK)
                {
                    return ret;
                }

                if (dir->name[0] == 0)
                {
                    return FAT32_FILE_NOT_FOUND;
                }

                if (((dir->attrib & 0x0F) != 0x0F) && (name[0] != 0xE5) && cmp(name, dir->name, 8) && cmp(ext, dir->ext, 3))
                {
                    return FAT32_OK;
                }
            }

            (*sector)++;
        }

        current_cluster = fat32_get_next_cluster(current_cluster);
    }
    while ((*sector & 0x0FFFFFF8) != 0x0FFFFFF8);

    return FAT32_FILE_NOT_FOUND;
}

fat32_error_t fat32_get_volume_name(uint8_t *name)
{
    uint32_t current_sector, current_cluster = 2;
    uint16_t current_index;
    dir_entry_t current_dir;
    uint8_t i;
    fat32_error_t ret;

    do
    {
        current_sector = fat.root_start + fat.sect_per_clust * (current_cluster - 2);

        for (i = 0; i < fat.sect_per_clust; i++)
        {
            for (current_index = 0; current_index < 512; current_index += 32)
            {
                ret = fat32_read_dir_entry(current_sector, current_index, &current_dir);

                if (ret != FAT32_OK)
                {
                    return ret;
                }

                if (current_dir.name[0] == 0)
                {
                    return FAT32_FILE_NOT_FOUND;
                }

                if (current_dir.attrib == 0x08)
                {
                    cpy(current_dir.name, name, 9);
                    return FAT32_OK;
                }
            }

            current_sector++;
        }

        current_cluster = fat32_get_next_cluster(current_cluster);
    }
    while ((current_sector & 0x0FFFFFF8) != 0x0FFFFFF8);

    return FAT32_FILE_NOT_FOUND;
}

fat32_error_t fat32_open(uint8_t *filename, uint32_t *sector, uint16_t *index, dir_entry_t *dir)
{
    uint8_t ext[4] = {0x20, 0x20, 0x20, 0};
    uint8_t name[9] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0};
    fat32_error_t ret;

    split(filename, name, ext);

    if ((ret = fat32_find(name, ext, sector, index, dir)) != FAT32_OK)
    {
        return ret;
    }

    return FAT32_OK;
}

bool fat32_file_exists(uint8_t *filename)
{
    uint8_t ext[4] = {0x20, 0x20, 0x20, 0};
    uint8_t name[9] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0};

    split(filename, name, ext);

    if (fat32_find(name, ext, NULL, NULL, NULL) == FAT32_OK)
    {
        return true;
    }

    return false;
}


fat32_error_t fat32_set_file_size(uint8_t *filename, uint32_t size)
{
    uint8_t ext[4] = {0x20, 0x20, 0x20, 0};
    uint8_t name[9] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0};
    uint32_t sector;
    uint16_t index;
    fat32_error_t ret;

    split(filename, name, ext);

    if ((ret = fat32_find(name, ext, &sector, &index, NULL)) != FAT32_OK)
    {
        return ret;
    }

    write32(ext, size);

    if (fs_write(sector, index + 28, ext, 4) != 4)
    {
        return FAT32_FS_ERROR;
    }

    return FAT32_OK;
}

fat32_error_t fat32_get_file_size(uint8_t *filename, uint32_t *size)
{
    uint8_t ext[4] = {0x20, 0x20, 0x20, 0};
    uint8_t name[9] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0};
    uint32_t sector;
    uint16_t index;
    fat32_error_t ret;

    split(filename, name, ext);

    if ((ret = fat32_find(name, ext, &sector, &index, NULL)) != FAT32_OK)
    {
        return ret;
    }

    if (fs_read(sector, index + 28, ext, 4) != 4)
    {
        return FAT32_FS_ERROR;
    }
    
    *size = read32(ext);
    return FAT32_OK;
}

fat32_error_t fat32_read_dir_entry(uint32_t sect, uint16_t index, dir_entry_t *dir)
{
    uint8_t buf[5];

    // Read the file name
    if (fs_read(sect, index, dir->name, 8) != 8)
    {
        return FAT32_FS_ERROR;
    }

    dir->name[8] = '\0';

    // Read the extension
    if (fs_read(sect, index + 8, dir->ext, 3) != 3)
    {
        return FAT32_FS_ERROR;
    }

    dir->ext[3] = '\0';

    // Read the file attributes
    if (fs_read(sect, index + 11, &(dir->attrib), 1) != 1)
    {
        return FAT32_FS_ERROR;
    }

    // Read the date
    if (fs_read(sect, index + 13, buf, 5) != 5)
    {
        return FAT32_FS_ERROR;
    }

    dir->ms = buf[0] % 100;
    dir->seconds = buf[0] / 100 + (buf[1] & 0x1F) * 2;
    dir->minutes = buf[1] >> 5 | ((buf[2] & 0x7) << 3);
    dir->hour = buf[2] >> 3;
    dir->day = buf[3] & 0x1F;
    dir->month = (buf[3] >> 5) | ((buf[4] & 0x1) << 3);
    dir->year = buf[4] >> 1;

    // Read the start cluster
    if (fs_read(sect, index + 20, buf, 2) != 2)
    {
        return FAT32_FS_ERROR;
    }

    dir->start_cluster = (uint32_t)read16(buf) << 16;

    if (fs_read(sect, index + 26, buf, 2) != 2)
    {
        return FAT32_FS_ERROR;
    }

    dir->start_cluster |= (uint32_t)read16(buf);

    // Read the file size
    if (fs_read(sect, index + 28, buf, 4) != 4)
    {
        return FAT32_FS_ERROR;
    }

    dir->size = read32(buf);

    return FAT32_OK;
}

fat32_error_t fat32_delete(uint8_t *filename)
{
    uint8_t ext[4] = {0x20, 0x20, 0x20, 0};
    uint8_t name[9] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0};
    uint16_t index;
    uint32_t next_cluster, current_cluster, sector;
    dir_entry_t dir;
    fat32_error_t ret;

    split(filename, name, ext);

    if ((ret = fat32_find(name, ext, &sector, &index, &dir)) != FAT32_OK)
    {
        return ret;
    }

    ext[0] = 0xE5;

    if (fs_write(sector, index, ext, 1) != 1)
    {
        return FAT32_FS_ERROR;
    }

    current_cluster = dir.start_cluster;

    do
    {
        next_cluster = fat32_get_next_cluster(current_cluster);

        if ((ret = fat32_free_cluster(current_cluster)) != FAT32_OK)
        {
            return ret;
        }

        current_cluster = next_cluster;
    }
    while ((current_cluster & 0x0FFFFFF8) != 0x0FFFFFF8);

    return FAT32_OK;
}

fat32_error_t fat32_create(uint8_t *filename, uint32_t *sector, uint16_t *index, dir_entry_t *dir)
{
    uint8_t ext[4] = {0x20, 0x20, 0x20, 0};
    uint8_t name[9] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0};
    uint8_t buf[32];
    uint16_t i;
    uint32_t current_cluster = 2, start;
    bool found = false;

    // Check if file already exists
    if (fat32_file_exists(filename))
    {
        return FAT32_FILE_EXISTS;
    }

    // Split the filename
    split(filename, name, ext);

    // Find a free file descriptor
    do
    {
        *sector = fat.root_start + fat.sect_per_clust * (current_cluster - 2);

        for (i = 0; i < fat.sect_per_clust; i++)
        {
            for (*index = 0; *index < 512; *index += 32)
            {
                if (fs_read(*sector, *index, buf, 1) != 1)
                {
                    return FAT32_FS_ERROR;
                }

                if ((buf[0] == 0) || (buf[0] == 0xE5))
                {
                    break;
                }
            }

            if (*index != 512)
            {
                found = true;
                break;
            }

            (*sector)++;
        }

        if (found)
        {
            break;
        }

        current_cluster = fat32_get_next_cluster(current_cluster);
    }
    while ((*sector & 0x0FFFFFF8) != 0x0FFFFFF8);

    start = fat32_find_empty_cluster();

    if (start == 1)
    {
        return FAT32_NO_FREE_CLUSTER;
    }

    // A free entry has been found in sector "*sector" at slot "*index"
    zero(buf, 32);          // By default, everything is set to 0
    cpy(name, buf, 8);      // Name
    cpy(ext, buf + 8, 3);   // Extension
    buf[11] = 0x20;         // Attributes
    buf[20] = start >> 16;  // Set higher two bytes of start cluster
    buf[21] = start >> 24;
    buf[26] = start;        // Set lower two bytes of start clunter
    buf[27] = start >> 8;

    if (fs_write(*sector, *index, buf, 32) != 32)
    {
        return FAT32_FS_ERROR;
    }

    fat32_read_dir_entry(*sector, *index, dir);

    return FAT32_OK;
}

uint32_t fat32_first_sector(uint32_t cluster)
{
    return fat.data_start + (cluster - 2) * fat.sect_per_clust;
}
