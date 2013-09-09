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
 * eeprom.h
 *
 *  Created on: Jul 5, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>
#include "packer.h"

/**
 * Base interface for EEPROM access.
 *
 * All data is stored in system endian
 */
typedef struct
{
    /** Get the size of the EEPROM, in bytes */
    uint32_t (*get_size)();

    /** Read a uint8_t, offset can be any value */
    uint8_t (*read_uint8)(uint32_t offset);
    /** Read a uint16_t, offset can be any value */
    uint16_t (*read_uint16)(uint32_t offset);
    /** Read a uint32_t, offset can be any value */
    uint32_t (*read_uint32)(uint32_t offset);

    /** Write a 8bit word, offset can be any value */
    void (*write_uint8)(uint32_t offset, uint8_t value);
    /** Write a 16bit word, offset can be any value */
    void (*write_uint16)(uint32_t offset, uint16_t value);
    /** Write a 32bit word, offset can be any value */
    void (*write_uint32)(uint32_t offset, uint32_t value);

} eeprom_t;

/**
 *  Get the internal EEPROM structure.
 *
 *  \return a pointer to the eeprom, or NULL if none exists
 */
const eeprom_t* eeprom_get_internal();

/**
 *  Get the external EEPROM structure.
 *
 *  \return a pointer to the eeprom, or NULL if none exists
 */
const eeprom_t* eeprom_get_external();

/**
 * Read a uint16 written in network endian
 */
static inline uint16_t eeprom_read_uint16_ntoh(const eeprom_t* eeprom,
        uint32_t offset)
{
    return packer_uint16_hton(eeprom->read_uint16(offset));
}

/**
 * Read a uint32 written in network endian
 */
static inline uint32_t eeprom_read_uint32_ntoh(const eeprom_t* eeprom,
        uint32_t offset)
{
    return packer_uint32_hton(eeprom->read_uint32(offset));
}

/**
 * Read a float written in network endian
 */
static inline float eeprom_read_float_ntoh(const eeprom_t* eeprom,
        uint32_t offset)
{
    uint32_t value = packer_uint32_hton(eeprom->read_uint32(offset));
    float *f = (void*) &value;
    return *f;
}

/**
 * Read a float written in host endian
 */
static inline float eeprom_read_float(const eeprom_t* eeprom, uint32_t offset)
{
    uint32_t value = eeprom->read_uint32(offset);
    float *f = (void*) &value;
    return *f;
}

#endif /* EEPROM_H_ */
