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

/** N25xxx flash driver
 * \file n25xxx.h
 * \date May 18, 2012
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

/** \addtogroup periph
 * @{
 */

/** \defgroup n25xxx N25xxx flash
 *
 * @{
 */

#ifndef N25XXX_H_
#define N25XXX_H_

#include <stdint.h>

/** Read the flash chip ID
 * Reads the flash chip ID which contains the manufacturer ID, the device ID and an unique ID
 * \param[out] id The content of the ID. The ID is 20-byte long, but the length parameter allows
 * partial reading. The id array MUST be at least len-byte long.
 * \param[in] len The length to be read. If len exceeds 20, only 20 bytes are read.
 */
void n25xxx_read_id(uint8_t *id, uint16_t len);

/** Read from flash
 * Reads a given amount of data from the flash starting anywhere in the flash
 * \param address The address where to start reading. This address must be inside the chip
 * addressable range. Any reading outside this area will give unspecified results
 * \param buf The buffer where the read content will be stored. This buffer must have enough
 * allocated space to avoid memory access fault.
 * \param len The number of bytes to read.
 */
void n25xxx_read(uint32_t address, uint8_t *buf, uint16_t len);

/** Enable writing on the flash
 * This function must be called before each write operation (i.e. call to \ref n25xxx_write_page, \ref n25xxx_erase_subsector,
 * \ref n25xxx_erase_sector and \ref n25xxx_bulk_erase)
 */
void n25xxx_write_enable();

/** Disable writing on the flash
 * This function must be called to prevent any write operation (i.e. any call to \ref n25xxx_write_page,
 * \ref n25xxx_erase_subsector, \ref n25xxx_erase_sector and \ref n25xxx_bulk_erase)
 * functions to alter the flash content
 */
void n25xxx_write_disable();

/** Write a complete flash page
 * This function writes the content of a buffer to a given flash page.
 * \note The write must be enabled before calling this function (\see \ref n25xxx_write_enable). If writting is
 * not enabled the function does not do anything and return no error.
 * \warning The address MUST be aligned to the beginning of a flash page (i.e. the last 8
 * bits of the address must be 0). Otherwise the alignment is forced to the beginning of the
 * page.
 * \warning The buffer must be 256-byte long
 * \param address The address of the page where to write the content of the buffer. The last 8 bits of the address must be 0
 * \param buf[in] The buffer to write to the flash page. This buffer MUST be 256-byte long 
 */
void n25xxx_write_page(uint32_t address, uint8_t *buf);

/** Erase a sub-sector of the flash
 * This functions erases a given sub-sector of the flash, a sub-sector is composed of 16 pages (i.e. 4096 bytes)
 * \note The write must be enabled before calling this function (\see \ref n25xxx_write_enable). If writting is
 * not enabled the function does not do anything and return no error.
 * \param address The address of the sub-sector to erase. The last 12 bits of the address must be 0 (aligned
 * with the beginning of the sub-sector), if not, the alignment is forced to the beginning of the sub-sector
 */
void n25xxx_erase_subsector(uint32_t address);

/** Erase a sector of the flash
 * This functions erases a given sector of the flash, a sector is composed of 16 sub-sectors (i.e 256 page or 65536 bytes)
 * \note The write must be enabled before calling this function (\see \ref n25xxx_write_enable). If writting is
 * not enabled the function does not do anything and return no error.
 * \param address The address of the sector to erase. The last 16 bits of the address must be 0 (aligned
 * with the beginning of the sector), if not, the alignment is forced to the beginning of the sector
 */
void n25xxx_erase_sector(uint32_t address);

/** Erase the entire flash
 * This functions erases the entire flash.
 * \note The write must be enabled before calling this function (\see \ref n25xxx_write_enable). If writting is
 * not enabled the function does not do anything and return no error.
 */
void n25xxx_bulk_erase();

/** @} */

/** @} */

#endif /* N25XXX_H_ */
