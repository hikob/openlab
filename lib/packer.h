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
 * packer.h
 *
 *  Created on: Jan 17, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef PACKER_H_
#define PACKER_H_

#include <stdint.h>
#include <string.h>

static inline uint8_t *packer_uint16_pack(uint8_t *buffer, uint16_t value)
{
    *buffer++ = (value >> 8) & 0xFF;
    *buffer++ = value & 0xFF;
    return buffer;
}
static inline const uint8_t *packer_uint16_unpack(const uint8_t *buffer,
        uint16_t *value)
{
    *value = ((uint16_t) buffer[0] << 8) + buffer[1];
    return buffer + 2;
}
static inline uint8_t *packer_uint32_pack(uint8_t *buffer, uint32_t value)
{
    *buffer++ = (value >> 24) & 0xFF;
    *buffer++ = (value >> 16) & 0xFF;
    *buffer++ = (value >> 8) & 0xFF;
    *buffer++ = value & 0xFF;
    return buffer;
}
static inline uint8_t *packer_float_pack(uint8_t *buffer, float value)
{
    uint32_t v;
    memcpy(&v, &value, 4);
    return packer_uint32_pack(buffer, v);
}
static inline const uint8_t *packer_uint32_unpack(const uint8_t *buffer,
        uint32_t *value)
{
    *value = ((uint32_t) buffer[0] << 24) + ((uint32_t) buffer[1] << 16)
             + ((uint32_t) buffer[2] << 8) + buffer[3];
    return buffer + 4;
}
static inline const uint8_t *packer_float_unpack(const uint8_t *buffer, float *value)
{
    return packer_uint32_unpack(buffer, (uint32_t *) value);
}
static inline uint16_t packer_uint16_hton(uint16_t value)
{
    return ((value >> 8) & 0x00FF) | ((value << 8) & 0xFF00);
}
static inline uint32_t packer_uint32_hton(uint32_t value)
{
    return ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00)
           | ((value << 8) & 0x00FF0000) | ((value << 24) & 0xFF000000);
}
static inline uint16_t packer_uint16_ntoh(uint16_t value)
{
    return packer_uint16_hton(value);
}
static inline uint32_t packer_uint32_ntoh(uint32_t value)
{
    return packer_uint32_hton(value);
}

#endif /* PACKER_H_ */
