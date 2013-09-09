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
 * Copyright (C) 2011-2013 HiKoB.
 */

/**
 * \file file.c
 * \date Aug 17, 2011
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "FreeRTOS.h"
#include "fat32.h"
#include "buf_util.h"
#include "fs.h"
#include "file.h"

typedef struct
{
    uint32_t current_cluster;
    uint32_t current_sector;
    uint8_t sector_index;

    uint8_t *pbuf;
    uint16_t buffer_index;

    uint32_t descriptor_sector;
    uint16_t descriptor_index;

    dir_entry_t dir;
} _file_t;

fat32_error_t file_create_next(uint8_t *basename, uint8_t *ext, uint8_t *buffer, file_t *f)
{
    uint8_t cpt = 0;
    static uint8_t filename[13] = "            ";
    fat32_error_t ret = FAT32_FILE_EXISTS;
    uint16_t len = length(basename, 6), len_ext = length(ext, 3);

    while (true)
    {
        cpy(basename, filename, len);
        filename[len] = (cpt < 0xA0) ? '0' + (cpt >> 4) : 'A' - 10 + (cpt >> 4);
        filename[len + 1] = ((cpt & 0xF) < 0xA) ? '0' + (cpt & 0xF) : 'A' - 10 + (cpt & 0xF);
        filename[len + 2] = '.';
        cpy(ext, filename + len + 3, len_ext);
        filename[len + len_ext + 3] = 0;

        if (!fat32_file_exists(filename))
        {
            ret = file_create(filename, buffer, f);
            break;
        }

        if (cpt == 0xFF)
        {
            ret = FAT32_FILE_EXISTS;
            break;
        }

        cpt++;
    }

    return ret;
}

fat32_error_t file_create(uint8_t *filename, uint8_t *page_buffer, file_t *f)
{
    fat32_error_t ret;
    _file_t *file = (_file_t*)pvPortMalloc(sizeof(_file_t));

    if ((ret = fat32_create(filename, &(file->descriptor_sector), &(file->descriptor_index), &(file->dir))) != FAT32_OK)
    {
        vPortFree(file);
        return ret;
    }

    file->current_cluster = file->dir.start_cluster;
    file->current_sector = fat32_first_sector(file->current_cluster);
    file->sector_index = 0;
    file->pbuf = page_buffer;
    file->buffer_index = 0;

    *f = file;

    return FAT32_OK;
}

fat32_error_t file_open(uint8_t *filename, uint8_t *page_buffer, file_t *f)
{
    fat32_error_t ret;
    _file_t *file = (_file_t*)pvPortMalloc(sizeof(_file_t));

    if ((ret = fat32_open(filename, &(file->descriptor_sector), &(file->descriptor_index), &(file->dir))) != FAT32_OK)
    {
        vPortFree(file);
        return ret;
    }

    file->current_cluster = file->dir.start_cluster;
    file->current_sector = fat32_first_sector(file->current_cluster);
    file->sector_index = 0;
    file->pbuf = page_buffer;
    file->buffer_index = 0;

    *f = file;

    return FAT32_OK;
}

fat32_error_t file_write(file_t f, uint8_t *buf, uint16_t size)
{
    uint32_t tmp;
    uint16_t max, l;
    uint8_t t[4];
    fat32_error_t ret;
    _file_t *file = (_file_t*)f;
    uint8_t *b = file->pbuf + file->buffer_index;

    max = 512 - file->buffer_index;
    l = max > size ? size : max;

    cpy(buf, b, l);

    file->dir.size += l;
    file->buffer_index += l;

    if (l < size)
    {
        if (fs_write(file->current_sector, 0, file->pbuf, 512) != 512)
        {
            return FAT32_FS_ERROR;
        }

        // This write updates the file size
        write32(t, file->dir.size);

        if (fs_write(file->descriptor_sector, file->descriptor_index + 28, t, 4) != 4)
        {
            return FAT32_FS_ERROR;
        }

        file->sector_index++;

        if (file->sector_index == fat.sect_per_clust)
        {
            tmp = fat32_find_empty_cluster();

            if ((ret = fat32_set_next_cluster(file->current_cluster, tmp)) != FAT32_OK)
            {
                return ret;
            }

            file->current_cluster = tmp;
            file->current_sector = fat32_first_sector(tmp);
            file->sector_index = 0;
        }
        else
        {
            file->current_sector++;
        }

        file->buffer_index = 0;

        return file_write(f, buf + l, size - l);
    }

    return FAT32_OK;
}

fat32_error_t file_read(file_t f, uint8_t *buf, uint16_t *size)
{
    uint16_t max, l, s;
    fat32_error_t ret;
    _file_t *file = (_file_t*)f;
    uint8_t *b = file->pbuf + file->buffer_index;

    max = 512 - file->buffer_index;
    l = max > *size ? *size : max;

    if (file->buffer_index + l > file->dir.size)
    {
        l = file->dir.size - file->buffer_index;
        *size = l;
    }

    if ((l != 0) && (file->buffer_index == 0))
    {
        if (fs_read(file->current_sector, 0, file->pbuf, 512) != 512)
        {
            return FAT32_FS_ERROR;
        }
    }

    cpy(b, buf, l);

    file->buffer_index += l;

    if (l < *size)
    {
        file->sector_index++;

        if (file->sector_index == fat.sect_per_clust)
        {
            file->current_cluster = fat32_get_next_cluster(file->current_cluster);
            file->current_sector = fat32_first_sector(file->current_cluster);
            file->sector_index = 0;
        }
        else
        {
            file->current_sector++;
        }

        file->buffer_index = 0;

        s = *size - l;
        ret = file_read(f, buf + l, &s);
        *size = s + l;

        return ret;
    }

    return FAT32_OK;
}

fat32_error_t file_close(file_t f)
{
    uint8_t buf[4];
    _file_t *file = (_file_t*)f;

    if (fs_write(file->current_sector, 0, file->pbuf, 512) != 512)
    {
        return FAT32_FS_ERROR;
    }

    write32(buf, file->dir.size);

    if (fs_write(file->descriptor_sector, file->descriptor_index + 28, buf, 4) != 4)
    {
        return FAT32_FS_ERROR;
    }

    return FAT32_OK;
}

fat32_error_t file_rename(file_t f, uint8_t *filename)
{
    _file_t *file = (_file_t*)f;
    uint8_t ext[4] = {0x20, 0x20, 0x20, 0};
    uint8_t name[9] = {0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0};

    if (fat32_file_exists(filename))
    {
        return FAT32_FILE_EXISTS;
    }
    split(filename, name, ext);
    cpy(name, file->dir.name, 9);
    cpy(ext, file->dir.ext, 4);
    if (fs_write(file->descriptor_sector, file->descriptor_index, name, 8) != 8)
    {
        return FAT32_FS_ERROR;
    }
    if (fs_write(file->descriptor_sector, file->descriptor_index + 8, ext, 3) != 3)
    {
        return FAT32_FS_ERROR;
    }
    return FAT32_OK;
}


fat32_error_t file_update(file_t f, uint32_t time)
{
    _file_t *file = (_file_t*)f;
    //taking posix time in input
    unsigned leap;
    unsigned year;
    uint8_t buf[5];
    year = 1970;
    while (time > (366 + 365 *3) * 24 * 3600)
    {
        year += 4;
        time -= (366 + 365 * 3) * 24 * 3600;
    }
    while (1)
    {
        leap = (year & 0x3) ? 0 : 1;
        unsigned len = (365 + leap) * 24 * 3600;
        if (time >= len)
        {
            year += 1;
            time -= len;
            continue;
        }
        break;
    }
    file->dir.year = year - 1980;
    {
        int tmp1, tmp2;
        if ((tmp1 = time - 31 * 24 * 3600) < 0)
        {
            file->dir.month = 1;
        }
        else if ((tmp2 = tmp1 - (28 + leap) * 24 * 3600) < 0)
        {
            time = tmp1;
            file->dir.month = 2;
        }
        else if ((tmp1 = tmp2 - 31 * 24 * 3600) < 0)
        {
            time = tmp2;
            file->dir.month = 3;
        }
        else if ((tmp2 = tmp1 - 30 * 24 * 3600) < 0)
        {
            time = tmp1;
            file->dir.month = 4;
        }
        else if ((tmp1 = tmp2 - 31 * 24 * 3600) < 0)
        {
            time = tmp2;
            file->dir.month = 5;
        }
        else if ((tmp2 = tmp1 - 30 * 24 * 3600) < 0)
        {
            time = tmp1;
            file->dir.month = 6;
        }
        else if ((tmp1 = tmp2 - 31 * 24 * 3600) < 0)
        {
            time = tmp2;
            file->dir.month = 7;
        }
        else if ((tmp2 = tmp1 - 31 * 24 * 3600) < 0)
        {
            time = tmp1;
            file->dir.month = 8;
        }
        else if ((tmp1 = tmp2 - 30 * 24 * 3600) < 0)
        {
            time = tmp2;
            file->dir.month = 9;
        }
        else if ((tmp2 = tmp1 - 31 * 24 * 3600) < 0)
        {
            time = tmp1;
            file->dir.month = 10;
        }
        else if ((tmp1 = tmp2 - 30 * 24 * 3600) < 0)
        {
            time = tmp2;
            file->dir.month = 11;
        }
        else
        {
            time = tmp1;
            file->dir.month = 12;
        }
    }
    file->dir.day = 1 + time / (24 * 3600);
    time %= 24 * 3600;
    file->dir.hour = time / 3600;
    time %= 3600;
    file->dir.minutes = time / 60;
    time %= 60;
    file->dir.seconds = time;

    buf[0] = 100 * (file->dir.seconds % 2);
    buf[1] = (file->dir.seconds >> 1) | (file->dir.minutes << 5);
    buf[2] = (file->dir.minutes >> 3) | (file->dir.hour << 3);
    buf[3] = (file->dir.day) | (file->dir.month << 5);
    buf[4] = (file->dir.month >> 3) | (file->dir.year << 1);

    if (fs_write(file->descriptor_sector, file->descriptor_index + 0x16, &buf[1], 4) != 4)
    {
        return FAT32_FS_ERROR;
    }
    return FAT32_OK;
}

