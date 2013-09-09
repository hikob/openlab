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
 * \file fs.h
 * \date Aug 17, 2011
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef FS_H_
#define FS_H_

#include <stdbool.h>
#include "FreeRTOS.h"
#include "semphr.h"

typedef struct
{
    // Buffer itself
    // WARNING!!!! The buffer must be declared first to ensure that content is
    // aligned on 4-bytes boundaries. Otherwise the DMA will give crappy results
    uint8_t content[512];

    // Mutex for mutal exclusion on buffer access
    xSemaphoreHandle vMutex;

    // Mutex for content access lock
    xSemaphoreHandle vContentMutex;

    // Buffer status
    bool dirty;

    // Page location on the SD card
    uint32_t page;

    // Used only for read buffer
    uint8_t use;
} buffer_t __attribute__((aligned(8)));

typedef enum {FS_OK, FS_MEDIUM_INIT_ERROR, FS_READ_FAILED} fs_error_t;

fs_error_t fs_init();
uint16_t fs_write(uint32_t page, uint16_t offset, uint8_t *buf, uint16_t size);
uint16_t fs_read(uint32_t page, uint16_t offset, uint8_t *buf, uint16_t size);

#endif
