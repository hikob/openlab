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
 * crc.h
 *
 *  Created on: Jan 17, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef CRC_H_
#define CRC_H_

#include <stdint.h>

/** Enable the CRC module */
void crc_enable();

/** Disable the CRC module */
void crc_disable();

/** Reset the CRC module */
void crc_reset();

/** Compute a CRC32 over a range of memory */
uint32_t crc_compute(const uint32_t *addr, uint32_t length);

#endif /* CRC_H_ */
