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
 * sdio.h
 *
 *  Created on: Jul 26, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef SDIO_H_
#define SDIO_H_

#include <stdint.h>
#include "handler.h"

typedef void *sdio_t;

typedef enum
{
    // Command errors
    SD_NO_ERROR,
    SD_TIMEOUT,
    SD_WRONG_CRC,
    SD_WRONG_PATTERN,
    SD_NOT_SD,
    SD_NOT_POWEREDUP,
    SD_WRONG_CSD_STRUCTURE,
    SD_RX_OVERRUN,
    SD_TX_UNDERRUN,
    SD_CARD_LOCKED,

    // Data errors
    SD_DATA_TIMEOUT,
    SD_DATA_WRONG_CRC,

    // Card status errors
    SD_AUTH_SEQ_ERROR,
    SD_WP_ERASE_SKIP,
    SD_CSD_OVERWRITE,
    SD_GENERAL_ERROR,
    SD_CC_ERROR,
    SD_CARD_ECC_FAILED,
    SD_ILLEGAL_COMMAND,
    SD_COM_CRC_ERROR,
    SD_LOCK_UNLOCK_FAILED,
    SD_WP_VIOLATION,
    SD_ERASE_PARAM,
    SD_ERASE_SEQ_ERROR,
    SD_BLOCK_LEN_ERROR,
    SD_ADDRESS_ERROR,
    SD_OUT_OF_RANGE,

    // Other errors
    SD_TRAN_TIMEOUT,
    SD_UNKNOWN_ERROR
} sd_error_t;

typedef enum
{
    SDSC_1_1,
    SDSC_2_0,
    SDHC,
} sd_card_type_t;

/**
 * SDIO controller initialization and SD card identification
 */
sd_error_t sd_init(sdio_t sdio);

/**
 * Get the type of card we are dealing with.
 * This method must be called before reading or writing to the SD card in order to determine the meaning of the addr parameter
 */
sd_card_type_t sd_get_type(sdio_t sdio);

/**
 * Get last SDIO status
 */
sd_error_t sd_get_status(sdio_t sdio);

/**
 * Get the card size in 512-byte blocks
 */
uint32_t sd_get_size(sdio_t sdio);

/**
 * Method to set the transfer handler.
 * This handler is called every time a transfer is ended
 */
void sd_set_transfer_handler(sdio_t sdio, handler_t handler);

/**
 * Method for reading from SD card
 * /param sdio The SDIO port to read from
 * /param addr The address to read. Is the card is SDHC, addr is a page address, otherwise, it is a byte address
 * /param buf The buffer to fill. It must be 32-bit aligned
 */
sd_error_t sd_read_single_block(sdio_t sdio, uint32_t addr, uint8_t *buf);

/**
 * Method for reading multiple blocks from SD card
 * /param sdio The SDIO port to read from
 * /param addr The address to read. Is the card is SDHC, addr is a page address, otherwise, it is a byte address
 * /param buf The buffers to fill. Each buffer must be 32-bit aligned
 */
sd_error_t sd_read_multiple_blocks(sdio_t sdio, uint32_t addr, uint8_t **buf, uint32_t nb_blocks);

/**
 * Method for writing to SD card
 * /param sdio The SDIO port to write to
 * /param addr The address to write. Is the card is SDHC, addr is a page address, otherwise, it is a byte address
 * /param buf The buffer to write. It must be 32-bit aligned
 */
sd_error_t sd_write_single_block(sdio_t sdio, uint32_t addr, uint8_t *buf);

/**
 * Method for writing multiple blocks to SD card
 * /param sdio The SDIO port to write to
 * /param addr The address to write. Is the card is SDHC, addr is a page address, otherwise, it is a byte address
 * /param buf The buffers to write. Each buffer must be 32-bit aligned
 */
sd_error_t sd_write_multiple_blocks(sdio_t sdio, uint32_t addr, uint8_t **buf, uint32_t nb_blocks);

/**
 * SDIO interrupt handler
 */
void sdio_handle_interrupt(sdio_t sdio);

#endif
