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

/** FAT32 file utilities
 * \file file.h
 * \date Aug 17, 2011
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

/** \addtogroup lib
 * @{
 */

/** \addtogroup FAT32
 * @{
 */

#ifndef __FILE_H
#define __FILE_H

#include <stdint.h>
#include "fat32.h"

/** Definition of the file type */
typedef void *file_t;

/** Create a new file
 * This function creates a new file named by the parameter filename. When creating the file a buffer of
 * 512 bytes for read and write buffering must be given. A file descriptor if created and stored in f.
 * \note The file will be created at the root of the file system as the FAT32 FS implementation does not
 * support directories.
 * \warning If the filename is not 8.3 (limitation of the FAT32 FS implementation), the name is truncated,
 * e.g. NOTVERYLONGFILE.NAME will be truncated to NOTVERYL.NAM . Therefore, be careful when naming files
 * as two truncated names could correspond to the same file.
 * \warning ALWAYS use upper case filenames otherwise Windows and MacOS won't be able to read the files back.
 * 
 * \param filename The name of the file to create. This must be a 8.3 filename.
 * \param page_buffer The address of a 512-byte buffer
 * \param f The file descriptor to be created if creation was successful
 * \return FAT32_OK if file creation was successfull or FAT32_FILE_EXISTS if there already is a file with
 * the given filename. This function can return FAT32_FS_ERROR in case of a low level error (e.g. page write)
 */
fat32_error_t file_create(uint8_t *filename, uint8_t *page_buffer, file_t *f);

/** Create a new file
 * This function creates a new file with a given basename, a sequence number and an extension. The sequence
 * number starts with 00 if there is no file with the specified basename and is incremented so that a new file
 * can be created. For example, if we want to create a file with basename BASE and extension TXT the function will
 * try to create a file named BASE00.TXT, suppose files BASE00.TXT, BASE01.TXT and BASE03.TXT exist, the 
 * function will create a file named BASE02.TXT. Another call to the function with the same parameters will
 * create a file named BASE004.TXT.
 * \note The file will be created at the root of the file system as the FAT32 FS implementation does not
 * support directories.
 * \warning If the basename is more than 6 character long, the basename is truncated,
 * e.g. FILENAME will be truncated to FILEN.
 * \warning ALWAYS use upper case base names and extensions otherwise Windows and MacOS won't be able to
 * read the files back.
 * 
 * \param basename The basename of the file to create. This must be less than 6 character long
 * \param ext The extension of the file to create. This must be less thant 3 character long
 * \param page_buffer The address of a 512-byte buffer
 * \param f The file descriptor to be created if creation was successful
 * \return FAT32_OK if file creation was successfull or FAT32_FILE_EXISTS if there are already 256
 * files with the same basename. This function can return FAT32_FS_ERROR in case of a low level error (e.g. page write)
 */
fat32_error_t file_create_next(uint8_t *basename, uint8_t *ext, uint8_t *buffer, file_t *f);

/** Open an existing file
 * This function opens an existing file. When opening the file, a buffer of 512 bytes must be given for
 * read and write buffering. A file descriptor is created and stored in f.
 * \note Only file on the root of the file system can be opened as the FAT32 FS implementation does not
 * support directories.
 * \warning Long filenames are not supported, the filename must be in the 8.3 format.
 *
 * \param filename The name of the file to open. This must be a 8.3 filename
 * \param page_buffer The address of a 512-byte buffer
 * \param f The file descriptor to be created if the file is opened successfuly
 * \return FAT32_OK if the file was opened successfuly of FAT32_NOT_FOUND if the file does not exists.
 * This function can return FAT32_FS_ERROR in case of a low level error (e.g. page write)
 */
fat32_error_t file_open(uint8_t *filename, uint8_t *page_buffer, file_t *f);

/** Close a file
 * This function closes properly a file that has been created of opened previously. This function ensures the
 * file system is consistent and that all buffers are written back on the device.
 *
 * \param f The file descriptor of the file to close
 * \return FAT32_OK in case of success or FAT32_FS_ERROR in case of a low level error (e.g. page write)
 */
fat32_error_t file_close(file_t f);

/** Sequencial write
 * This function writes a buffer of data on the device. All write operations are sequencial.
 * \warning This function MUST NOT be used on an existing file (i.e. a file descriptor created with the
 * file_open function). It would result in file system inconsistence.
 *
 * \param f The file descriptor of the file to write to
 * \param buf The buffer of data to write to the device
 * \param size The size of the buffer to write
 */
fat32_error_t file_write(file_t f, uint8_t *buf, uint16_t size);

/** Sequencial read 
 * This function reads a buffer of data from the device. All read operations are sequencial.
 *
 * \param f The file descriptor of the file to read from 
 * \param buf The buffer of data to read from the device
 * \param size The size of the buffer to read. This parameter is updated with the actual amount of read data
 */
fat32_error_t file_read(file_t f, uint8_t *buf, uint16_t *size);

/** Update the timestamp of the file
 */
fat32_error_t file_update(file_t f, uint32_t time);

/** Rename the file
 */
fat32_error_t file_rename(file_t f, uint8_t *filename);

/** @} */

/** @} */

#endif
