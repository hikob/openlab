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
 * sdio.c
 *
 *  Created on: Jul 26, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include <stdbool.h>
#include "platform.h"
#include "printf.h"

#include "sdio.h"
extern sdio_t sdio;

bool transfer_ended = false;
sd_error_t err = SD_NO_ERROR;

void error();

void transfer_handler(handler_arg_t arg)
{
    transfer_ended = true;
    err = (sd_error_t)arg;
}

int main()
{
    uint32_t i, j;
    uint8_t buf[512] __attribute__((aligned(8)));

    // Initialize the platform
    platform_init();

    for (i = 0; i < 0x10000; i++)
    {
        asm("nop");
    }

    printf("SD Card test\r\n");
    printf("============\r\n");

    printf("SD Initialization\r\n");

    if ((err = sd_init(sdio)) != SD_NO_ERROR)
    {
        error();
    }

    sd_set_transfer_handler(sdio, transfer_handler);

    printf("Card type is: ");

    switch (sd_get_type(sdio))
    {
        case SDSC_1_1:
            printf("SD v1.1 Standard capacity\r\n");
            break;
        case SDSC_2_0:
            printf("SD v2.0 Standard capacity\r\n");
            break;
        case SDHC:
            printf("SD v2.0 High capacity\r\n");
            break;
        default:
            printf("Unknown\r\n");
    }

    printf("Card has %d 512-byte block\r\n", sd_get_size(sdio));


    for (j = 0; j < sd_get_size(sdio); j++)
    {
        for (i = 0; i < sizeof(buf) / sizeof(buf[0]); i++)
        {
            buf[i] = j;
        }

        printf("Writing block #%d\r\n", j);
        transfer_ended = false;

        if (sd_get_type(sdio) == SDHC)
        {
            err = sd_write_single_block(sdio, j, buf);
        }
        else
        {
            err = sd_write_single_block(sdio, j * 512, buf);
        }

        if (err != SD_NO_ERROR)
        {
            error();
        }

        while (!transfer_ended)
        {
            ;
        }

        if (err != SD_NO_ERROR)
        {
            error();
        }

        for (i = 0; i < sizeof(buf) / sizeof(buf[0]); i++)
        {
            buf[i] = j + 1;
        }

        printf("Reading the written block: ");
        transfer_ended = false;

        if (sd_get_type(sdio) == SDHC)
        {
            err = sd_read_single_block(sdio, j, buf);
        }
        else
        {
            err = sd_read_single_block(sdio, j * 512, buf);
        }

        if (err != SD_NO_ERROR)
        {
            error();
        }

        while (!transfer_ended)
        {
            ;
        }

        if (err != SD_NO_ERROR)
        {
            error();
        }

        for (i = 0; i < sizeof(buf) / sizeof(buf[0]); i++)
        {
            if (buf[i] != (j & 0xFF))
            {
                printf("wrong buffer content\r\n");
                error();
            }
        }

        printf("OK\r\n");
    }

    printf("END\r\n");

    while (1)
    {
        ;
    }

    return 0;
}

void error()
{
    switch (err)
    {
        case SD_NO_ERROR:
            printf("SD_NO_ERROR");
            break;
        case SD_TIMEOUT:
            printf("SD_TIMEOUT");
            break;
        case SD_WRONG_CRC:
            printf("SD_WRONG_CRC");
            break;
        case SD_WRONG_PATTERN:
            printf("SD_WRONG_PATTERN");
            break;
        case SD_NOT_SD:
            printf("SD_NOT_SD");
            break;
        case SD_NOT_POWEREDUP:
            printf("SD_NOT_POWEREDUP");
            break;
        case SD_WRONG_CSD_STRUCTURE:
            printf("SD_WRONG_CSD_STRUCTURE");
            break;
        case SD_RX_OVERRUN:
            printf("SD_RX_OVERRUN");
            break;
        case SD_TX_UNDERRUN:
            printf("SD_TX_UNDERRUN");
            break;
        case SD_CARD_LOCKED:
            printf("SD_CARD_LOCKED");
            break;
        case SD_UNKNOWN_ERROR:
            printf("SD_UNKNOWN_ERROR");
            break;
        case SD_AUTH_SEQ_ERROR:
            printf("SD_AUTH_SEQ_ERROR");
            break;
        case SD_WP_ERASE_SKIP:
            printf("SD_WP_ERASE_SKIP");
            break;
        case SD_CSD_OVERWRITE:
            printf("SD_CSD_OVERWRITE");
            break;
        case SD_GENERAL_ERROR:
            printf("SD_GENERAL_ERROR");
            break;
        case SD_CC_ERROR:
            printf("SD_CC_ERROR");
            break;
        case SD_CARD_ECC_FAILED:
            printf("SD_CARD_ECC_FAILED");
            break;
        case SD_ILLEGAL_COMMAND:
            printf("SD_ILLEGAL_COMMAND");
            break;
        case SD_COM_CRC_ERROR:
            printf("SD_COM_CRC_ERROR");
            break;
        case SD_LOCK_UNLOCK_FAILED:
            printf("SD_LOCK_UNLOCK_FAILED");
            break;
        case SD_WP_VIOLATION:
            printf("SD_WP_VIOLATION");
            break;
        case SD_ERASE_PARAM:
            printf("SD_ERASE_PARAM");
            break;
        case SD_ERASE_SEQ_ERROR:
            printf("SD_ERASE_SEQ_ERROR");
            break;
        case SD_BLOCK_LEN_ERROR:
            printf("SD_BLOCK_LEN_ERROR");
            break;
        case SD_ADDRESS_ERROR:
            printf("SD_ADDRESS_ERROR");
            break;
        case SD_OUT_OF_RANGE:
            printf("SD_OUT_OF_RANGE");
            break;
        case SD_DATA_TIMEOUT:
            printf("SD_DATA_TIMEOUT");
            break;
        case SD_DATA_WRONG_CRC:
            printf("SD_DATA_WRONG_CRC");
            break;
        case SD_TRAN_TIMEOUT:
            printf("SD_TRAN_TIMEOUT");
            break;
        default:
            printf("Not sd_error_t");
            break;
    }

    while (1)
    {
        ;
    }
}
