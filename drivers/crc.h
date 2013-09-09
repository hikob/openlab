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

/**
 * \file crc.h
 *
 * \date Jan 17, 2012
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef CRC_H_
#define CRC_H_

/**
 * \addtogroup drivers
 * @{
 */

/**
 * \defgroup CRC CRC Computing driver
 *
 * This driver provides hardware CRC computation methods, compatible with
 * standard CRC algorithm such as Ethernet.
 *
 * The CRC is computed over a given number of words.
 *
 *@{
 */

#include <stdint.h>

/**
 * Enable the CRC module.
 *
 * \note This must be called before any of the \ref crc_reset, \ref crc_compute
 * and \ref crc_terminate functions.
 */
void crc_enable();

/**
 * Disable the CRC module.
 *
 * This saves power when not needed.
 */
void crc_disable();

/**
 * Reset the CRC module.
 *
 * And prepare the driver to compute a new CRC over a given memory range.
 */
void crc_reset();

/**
 * Compute a CRC32 over a range of memory.
 *
 * \param addr a pointer to the internal memory to start computing
 * \param length the number of words to compute the CRC over
 */
void crc_compute(const uint32_t *addr, uint32_t length);

/**
 * Terminate the CRC32 computation.
 *
 * \return the computed CRC value
 */
uint32_t crc_terminate();

/**
 * @}
 * @}
 */

#endif /* CRC_H_ */
