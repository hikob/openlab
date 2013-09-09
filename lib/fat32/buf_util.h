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
 * \file buf_util.h
 * \date Aug 17, 2011
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef BUF_UTIL_H_
#define BUF_UTIL_H_

#include <stdbool.h>
#include <stdint.h>

inline static uint16_t read16(uint8_t *buf)
{
    return (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
}

inline static uint32_t read32(uint8_t *buf)
{
    return (uint32_t)buf[0] | ((uint32_t)buf[1] << 8) | ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24);
}

inline static void write16(uint8_t *buf, uint16_t val)
{
    buf[0] = val;
    buf[1] = val >> 8;
}

inline static void write32(uint8_t *buf, uint32_t val)
{
    buf[0] = val;
    buf[1] = val >> 8;
    buf[2] = val >> 16;
    buf[3] = val >> 24;
}

bool cmp(uint8_t *a, uint8_t *b, uint16_t len);

void cpy(uint8_t *src, uint8_t *dst, uint16_t len);

void zero(uint8_t *buf, uint16_t len);

void split(uint8_t *filename, uint8_t *name, uint8_t *ext);

uint16_t length(uint8_t *buf, uint16_t max);

#endif
