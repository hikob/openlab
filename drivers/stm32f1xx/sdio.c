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
 * sdio.c
 *
 *  Created on: Aug 30, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */


#include <stdbool.h>
#include "platform.h"
#include "sdio.h"
#include "sdio_.h"
#include "sdio_registers.h"
#include "rcc.h"
#include "gpio.h"
#include "gpio_.h"
#include "dma.h"
#include "nvic.h"
#include "nvic_.h"

#define MAX_VOLT_TRY   10000
#define MAX_CMD_TRY    10
#define MAX_WRITE_WAIT 10000
//#define DEBUG

typedef enum
{
    // CMD
    SD_GO_IDLE_STATE        =  0,  // CMD0
    SD_ALL_SEND_CID         =  2,  // CMD2
    SD_SET_REL_ADDR         =  3,  // CMD3
    SD_SELECT_DESELECT_CARD =  7,  // CMD7
    SD_SEND_IF_COND         =  8,  // CMD8
    SD_SEND_CSD             =  9,  // CMD9
    SD_SEND_CID             =  10, // CMD10
    SD_SEND_STATUS          =  13, // CMD13
    SD_SET_BLOCKLEN         =  16, // CMD16
    SD_READ_SINGLE_BLOCK    =  17, // CMD17
    SD_WRITE_SINGLE_BLOCK   =  24, // CMD24
    SD_APP_CMD              =  55, // CMD55

    // ACMD
    SD_SET_BUS_WIDTH        = 106, // ACMD6
    SD_SEND_OP_COND         = 141  // ACMD41
} sd_command_t;

typedef enum
{
    NO_RESPONSE,
    SHORT_RESPONSE,
    LONG_RESPONSE
} sd_response_type_t;

static sd_error_t parse_card_status(_sdio_t *_sdio, uint32_t resp)
{
    _sdio->sd_state = (resp >> 9) & 0xF;
    _sdio->sd_card_locked = (resp >> 25) & 0x1;
    _sdio->sd_app_cmd = (resp >> 5) & 0x1;

    if (resp & (1 << 3))
    {
        return SD_AUTH_SEQ_ERROR;
    }
    else if (resp & (1 << 15))
    {
        return SD_WP_ERASE_SKIP;
    }
    else if (resp & (1 << 16))
    {
        return SD_CSD_OVERWRITE;
    }
    else if (resp & (1 << 19))
    {
        return SD_GENERAL_ERROR;
    }
    else if (resp & (1 << 20))
    {
        return SD_CC_ERROR;
    }
    else if (resp & (1 << 21))
    {
        return SD_CARD_ECC_FAILED;
    }
    else if (resp & (1 << 22))
    {
        return SD_ILLEGAL_COMMAND;
    }
    else if (resp & (1 << 23))
    {
        return SD_COM_CRC_ERROR;
    }
    else if (resp & (1 << 24))
    {
        return SD_LOCK_UNLOCK_FAILED;
    }
    else if (resp & (1 << 26))
    {
        return SD_WP_VIOLATION;
    }
    else if (resp & (1 << 27))
    {
        return SD_ERASE_PARAM;
    }
    else if (resp & (1 << 28))
    {
        return SD_ERASE_SEQ_ERROR;
    }
    else if (resp & (1 << 29))
    {
        return SD_BLOCK_LEN_ERROR;
    }
    else if (resp & (1 << 30))
    {
        return SD_ADDRESS_ERROR;
    }
    else if (resp & (1 << 31))
    {
        return SD_OUT_OF_RANGE;
    }

    return SD_NO_ERROR;
}

static inline sd_error_t sd_get_R1(_sdio_t *_sdio)
{
    return parse_card_status(_sdio, *sdio_get_RESP1());
}

static inline void sd_get_R2(uint32_t *resp)
{
    resp[0] = *sdio_get_RESP1();
    resp[1] = *sdio_get_RESP2();
    resp[2] = *sdio_get_RESP3();
    resp[3] = *sdio_get_RESP4();
}

static inline void sd_get_R3(uint32_t *resp)
{
    *resp = *sdio_get_RESP1();
}

static inline sd_error_t sd_get_R6(_sdio_t *_sdio)
{
    uint32_t resp, t;

    t = *sdio_get_RESP1();

    _sdio->rca = t & 0xFFFF0000;

    // Reorder bits to convert R6 response into R1 response
    resp = t & 0x1FFF;
    resp |= (t & (1 << 13)) << 6;
    resp |= (t & ((1 << 14) | (1 << 15))) << 8;

    return parse_card_status(_sdio, resp);
}

static inline sd_error_t sd_get_R7(uint8_t pattern, uint8_t *voltage)
{
    uint32_t resp = *sdio_get_RESP1();

    if ((resp & 0xFF) != pattern)
    {
        return SD_WRONG_PATTERN;
    }

    *voltage = (resp & 0xF00) >> 8;

    return SD_NO_ERROR;
}

static sd_error_t sd_send_command(_sdio_t *_sdio, sd_command_t cmd, uint32_t data, sd_response_type_t resp)
{
    sd_error_t err = SD_NO_ERROR;
    uint32_t i, tmp;

#ifdef DEBUG
    printf("---\r\nCommand: %s%u\tData: 0x%X\r\n", cmd > 100 ? "ACMD" : "CMD", cmd % 100, data);
#endif

    for (i = 0; i < MAX_CMD_TRY; i++)
    {
#ifdef DEBUG
        printf("Try #%u\r\n", i);
#endif

        // If the command is a ACMD, we need to send APP_CMD (CMD55) before sending it
        if (cmd > 100)
        {
            err = sd_send_command(_sdio, SD_APP_CMD, _sdio->rca, SHORT_RESPONSE);

            if (err != SD_NO_ERROR)
            {
                return err;
            }

            if ((err = sd_get_R1(_sdio)) != SD_NO_ERROR)
            {
                return err;
            }
        }

        *sdio_get_ARG() = data;
        tmp = (cmd % 100) | SDIO_CMD__CPSMEN;

        if (resp != NO_RESPONSE)
        {
            tmp |= 1 << 6;    //! \todo name these two bits
        }

        if (resp == LONG_RESPONSE)
        {
            tmp |= 1 << 7;
        }

        *sdio_get_CMD() = tmp;

        tmp = 0;

        if (resp == NO_RESPONSE)
        {
            // Wait command to be sent (CMDSENT bit)
            while (!(tmp & SDIO_STA__CMDSENT))
            {
                tmp = *sdio_get_STA();
            }

            break;
        }
        else
        {
            // Wait for the response to be received
            // Check if there is neither CRC error nor timeout
            while (!(tmp & (SDIO_STA__CCRCFAIL | SDIO_STA__CTIMEOUT | SDIO_STA__CMDREND)))
            {
                tmp = *sdio_get_STA();
            }

            // Return CRC error only if command was not expecting R3 response
            // Otherwise, CCRCFAIL is set as R3 response has no CRC, in that case it means that the response has been received
            if ((tmp & SDIO_STA__CCRCFAIL) && (cmd != SD_SEND_OP_COND))
            {
                err = SD_WRONG_CRC;
            }
            else if (tmp & SDIO_STA__CTIMEOUT)
            {
                err = SD_TIMEOUT;
            }
            else
            {
                err = SD_NO_ERROR;
                break;
            }
        }

#ifdef DEBUG
        printf("Err: %u\r\n", err);
        parse_card_status(*sdio_get_RESP1());
        printf("Card state: %u\t ACMD: %u\tLocked: %u\tRESP: 0x%X\r\n", sd_state, sd_app_cmd, sd_card_locked, *sdio_get_RESP1());
#endif

        *sdio_get_ICR() = SDIO_ICR__ALL;
    }

    *sdio_get_ICR() = SDIO_ICR__ALL;

#ifdef DEBUG
    parse_card_status(*sdio_get_RESP1());
    printf("Card state: %u\t ACMD: %u\tLocked: %u\tRESP: 0x%X\r\n", sd_state, sd_app_cmd, sd_card_locked, *sdio_get_RESP1());
#endif

    return err;
}

static sd_error_t sd_powerup(_sdio_t *_sdio)
{
    uint32_t initialized = 0, param = 0, cpt = 0, resp;
    uint8_t voltage;
    sd_error_t ret;

    // SDIO_POWER = 0x03 card is powered on
    *sdio_get_POWER() = SDIO_POWER__PWRCTRL_MASK;

    // Wait for at least 74 clock cycles to be sure the card has boot.
    for (cpt = 0; cpt < 0x1000000; cpt++)
    {
        asm("nop");    // FIXME: do a real cycle count
    }

    cpt = 0;

    /***** CMD0 (GO_IDLE_STATE) *****/
    ret = sd_send_command(_sdio, SD_GO_IDLE_STATE, 0, NO_RESPONSE);

    if (ret != SD_NO_ERROR)
    {
        return ret;
    }

    /***** CMD8 (SEND_IF_COND) *****/
    // Verify supported voltage and check pattern
    // Argument:
    //  [31:12]: 0
    //  [11:8] : Supply voltage, range 2.7 to 3.6V (1)
    //  [7:0]  : Pattern: 0xAA
#define PATTERN (0xAA)
    ret = sd_send_command(_sdio, SD_SEND_IF_COND, 0x100 | PATTERN, SHORT_RESPONSE);

    if (ret == SD_NO_ERROR)
    {
        // Card follows the v2.0 specification
        _sdio->sd_type = SDSC_2_0;

        if ((ret = sd_get_R7(PATTERN, &voltage)) != SD_NO_ERROR)
        {
            return ret;
        }
    }
    else if (ret == SD_TIMEOUT)
    {
        // Card follows the v1.1 specification at best
        _sdio->sd_type = SDSC_1_1;
    }
    else
    {
        return ret;
    }

#undef PATTERN

    // Send a dummy CMD55 to flush the "illegal command" flag in case of a v1.01 card
    ret = sd_send_command(_sdio, SD_APP_CMD, _sdio->rca, SHORT_RESPONSE);

    if (ret != SD_NO_ERROR)
    {
        return ret;
    }

    ret = sd_get_R1(_sdio);

    if ((ret != SD_NO_ERROR) && (ret != SD_ILLEGAL_COMMAND))
    {
        return ret;
    }

    while ((!initialized) && (cpt < MAX_VOLT_TRY))
    {
        /***** ACMD41 (SEND_OP_COND) *****/
        // Set HCS = 1 to tell v2.0 cards that we support SDHC cards
        // Otherwise HCS = 0 as the card is at best v1.1
        if (_sdio->sd_type == SDSC_2_0)
        {
            param = 0x40000000;
        }

        param |= 0x80100000;
        ret = sd_send_command(_sdio, SD_SEND_OP_COND, param, SHORT_RESPONSE);

        if (ret != SD_NO_ERROR)
        {
            return ret;
        }

        sd_get_R3(&resp);

        // Read card power up status bit
        initialized = resp & (1 << 31);

        cpt++;

        if (initialized)
        {
            // If the card is initialized, the Card Capacity Status (CCS) is valid
            if (resp & (1 << 30))
            {
                _sdio->sd_type = SDHC;
            }
        }
    }

    if (!initialized)
    {
        return SD_NOT_POWEREDUP;
    }

    return SD_NO_ERROR;
}

static sd_error_t sd_identification(_sdio_t *_sdio)
{
    sd_error_t ret;

    /***** CMD2 (ALL_SEND_CID) *****/
    ret = sd_send_command(_sdio, SD_ALL_SEND_CID, 0, LONG_RESPONSE);

    if (ret != SD_NO_ERROR)
    {
        return ret;
    }

    sd_get_R2(_sdio->CID);

    /***** CMD3 (SET_REL_ADDR) *****/
    ret = sd_send_command(_sdio, SD_SET_REL_ADDR, 0, SHORT_RESPONSE);

    if (ret != SD_NO_ERROR)
    {
        return ret;
    }

    if ((ret = sd_get_R6(_sdio)) != SD_NO_ERROR)
    {
        return ret;
    }

    /***** CMD9 (SEND_CSD) *****/
    ret = sd_send_command(_sdio, SD_SEND_CSD, _sdio->rca, LONG_RESPONSE);

    if (ret != SD_NO_ERROR)
    {
        return ret;
    }

    sd_get_R2(_sdio->CSD);

    // Parse CSD fields
    switch (_sdio->CSD[0] & 0xC0000000)
    {
        case 0x00000000:

            // This is CSD version 1.0
            if (_sdio->sd_type == SDHC)
            {
                return SD_WRONG_CSD_STRUCTURE;
            }

            // bits [83:80] matters: READ_BL_LEN
            // bits [73:62] are C_SIZE
            // bits [49:47] are C_SIZE_MULT
            _sdio->sd_desc.read_bl_len = (_sdio->CSD[1] & 0x000F0000) >> 16;
            _sdio->sd_desc.c_size = ((_sdio->CSD[1] & 0x000003FF) << 2) | (_sdio->CSD[2] >> 30);
            _sdio->sd_desc.c_size_mult = (_sdio->CSD[2] & 0x00038000) >> 15;
            break;

        case 0x40000000:

            // This is CSD version 2.0
            if (_sdio->sd_type != SDHC)
            {
                return SD_WRONG_CSD_STRUCTURE;
            }

            // bits [83:80] are fixed: READ_BL_LEN = 9
            // bits [69:48] are C_SIZE (6 first MSB are 0)
            // there is no C_SIZE_MULT field
            _sdio->sd_desc.read_bl_len = (_sdio->CSD[1] & 0x000F0000) >> 16;
            _sdio->sd_desc.c_size = ((_sdio->CSD[1] & 0x3F) << 16) | (_sdio->CSD[2] >> 16);
            _sdio->sd_desc.c_size_mult = 0xFF;
            break;

        default:
            return SD_WRONG_CSD_STRUCTURE;
    }

    /***** CMD7 (SELECT_DESELECT_CARD) *****/
    ret = sd_send_command(_sdio, SD_SELECT_DESELECT_CARD, _sdio->rca, SHORT_RESPONSE);

    if (ret != SD_NO_ERROR)
    {
        return ret;
    }

    if ((ret = sd_get_R1(_sdio)) != SD_NO_ERROR)
    {
        return ret;
    }

    return SD_NO_ERROR;
}

static int32_t sd_get_divider(int32_t freq)
{
    int32_t hclk, div;

    // Get HCLK frequency to compute dividers
    hclk = rcc_sysclk_get_clock_frequency(RCC_SYSCLK_CLOCK_HCLK);

    // Compute the divider so that the SDIO clock is 'freq'
    div = hclk / freq - 2;

    // If the is a rounding problem, increment the divider
    if (hclk % freq)
    {
        div++;
    }

    // In case HCLK is too low
    if (div < 0)
    {
        div = 0;
    }

    return div;
}

sd_error_t sd_init(sdio_t sdio_)
{
    int32_t div;
    uint8_t i;
    sd_error_t ret;
    _sdio_t *_sdio = sdio_;

    // Shutdown SD card
    *sdio_get_POWER() = 0;

    // Enable clocking of SDIO, DMA2, GPIOC and GPIOD
    rcc_apb_enable(RCC_APB2, RCC_APB_BIT_AFIO);
    rcc_ahb_enable(RCC_AHB_BIT_SDIO);

    // Configure DMA
    dma_enable(_sdio->dma_channel);

    // Configure GPIO pin alternate functions
    for (i = 0; i < 6; i++)
    {
        gpio_enable(_sdio->gpio[i]);
        gpio_set_output(_sdio->gpio[i], _sdio->pin[i]);
        gpio_set_alternate_function(_sdio->gpio[i], _sdio->pin[i]);
    }

    if (_sdio->regu_gpio)
    {
        // Set regulator to 500mA
        gpio_set_output(_sdio->regu_gpio, _sdio->regu_pin);
        gpio_pin_set(_sdio->regu_gpio, _sdio->regu_pin);
    }

    // Disable SDIO interrupt line
    nvic_disable_interrupt_line(NVIC_IRQ_LINE_SDIO);

    // Set SDIO clock frequency to 400KHz
    // This is the maximum allowed frequency during identification process
    div = sd_get_divider(400000);
    // Enable clock and set clock divider
    // Bus width is 1 during identification
    *sdio_get_CLKCR() = SDIO_CLKCR__CLKEN | (div & SDIO_CLKCR__CLKDIV_MASK);

    // Initialize SDIO peripheral
    if ((ret = sd_powerup(_sdio)) != SD_NO_ERROR)
    {
        return ret;
    }

    if ((ret = sd_identification(_sdio)) != SD_NO_ERROR)
    {
        return ret;
    }

    // Set SDIO clock frequency to 25MHz
    // This is the maximum allowed frequency
    div = sd_get_divider(25000000);
    // Enable clock and set clock divider
    // WARNING: never set hardware flow control on as it will corrupt data
    *sdio_get_CLKCR() = SDIO_CLKCR__CLKEN | (div & SDIO_CLKCR__CLKDIV_MASK);

    // Set bus width to 4
    /***** ACMD6 (SET_BUS_WIDTH) *****/
    ret = sd_send_command(_sdio, SD_SET_BUS_WIDTH, 0x2, SHORT_RESPONSE);

    if (ret != SD_NO_ERROR)
    {
        return ret;
    }

    if ((ret = sd_get_R1(_sdio)) != SD_NO_ERROR)
    {
        return ret;
    }

    *sdio_get_CLKCR() |= SDIO_CLKCR__WIDBUS_4;

    // Set block length once for all
    /***** CMD16 (SET_BLOCKLEN) *****/
    ret = sd_send_command(_sdio, SD_SET_BLOCKLEN, 512, SHORT_RESPONSE);

    if (ret != SD_NO_ERROR)
    {
        return ret;
    }

    if ((ret = sd_get_R1(_sdio)) != SD_NO_ERROR)
    {
        return ret;
    }

    return SD_NO_ERROR;
}

sd_card_type_t sd_get_type(sdio_t sdio_)
{
    _sdio_t *_sdio = sdio_;

    return _sdio->sd_type;
}

void sd_set_transfer_handler(sdio_t sdio_, handler_t handler)
{
    _sdio_t *_sdio = sdio_;

    _sdio->transfer_handler = handler;
}

sd_error_t sd_get_status(sdio_t sdio_)
{
    sd_error_t ret;
    _sdio_t *_sdio = sdio_;

    ret = sd_send_command(_sdio, SD_SEND_STATUS, _sdio->rca, SHORT_RESPONSE);
    
    if (ret != SD_NO_ERROR)
    {
	return ret;
    }

    return sd_get_R1(_sdio);
}

sd_error_t sd_read_single_block(sdio_t sdio_, uint32_t addr, uint8_t *buf)
{
    sd_error_t ret;
    _sdio_t *_sdio = sdio_;

    // Reset DPSM configuration
    *sdio_get_DCTRL()  = 0;
    *sdio_get_DLEN()   = 0;
    *sdio_get_DTIMER() = 0xFFFF; //! \todo compute the best data timeout value

    *sdio_get_DLEN() = 512;

    // Wait for card to be transfer state
    while (true)
    {
        /***** CMD13 (SEND_STATUS) *****/
        ret = sd_send_command(_sdio, SD_SEND_STATUS, _sdio->rca, SHORT_RESPONSE);

        if (ret != SD_NO_ERROR)
        {
            return ret;
        }

        if ((ret = sd_get_R1(_sdio)) != SD_NO_ERROR)
        {
            return ret;
        }

        if (_sdio->sd_state == SD_STATE_TRAN)
        {
            break;
        }
        else
        {
            ret = SD_TRAN_TIMEOUT;
        }
    }

    /***** CMD17 (READ_SINGLE_BLOCK) *****/
    ret = sd_send_command(_sdio, SD_READ_SINGLE_BLOCK, addr, SHORT_RESPONSE);

    if (ret != SD_NO_ERROR)
    {
        return ret;
    }

    if ((ret = sd_get_R1(_sdio)) != SD_NO_ERROR)
    {
        return ret;
    }

    // Configure DPSM (Data Path State Machine)
    // 512-bytes blocks, from card to controller and data transfert enabled
    *sdio_get_DCTRL() = SDIO_DCTRL__DBLOCKSIZE_512 | SDIO_DCTRL__DTDIR | SDIO_DCTRL__DTEN;

    // Clear and enable interrupts
    *sdio_get_ICR() = SDIO_ICR__ALL;
    *sdio_get_MASK() = SDIO_MASK__DCRCFAILIE | SDIO_MASK__DTIMEOUTIE | SDIO_MASK__DBCKENDIE | SDIO_MASK__RXOVERRIE | SDIO_MASK__STBITERRIE;

    // Configure DMA
    dma_config(_sdio->dma_channel, (uint32_t)sdio_get_FIFO(), (uint32_t)buf, 512 / 4, DMA_SIZE_32bit, DMA_DIRECTION_FROM_PERIPHERAL, DMA_INCREMENT_ON);

    // Enable SDIO interrupt line
    *sdio_get_ICR() = SDIO_ICR__ALL;
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_SDIO);

    // Enable and start DMA
    dma_start(_sdio->dma_channel, NULL, NULL);
    *sdio_get_DCTRL() |= SDIO_DCTRL__DMAEN;

    return SD_NO_ERROR;
}

sd_error_t sd_read_multiple_blocks(sdio_t sdio_, uint32_t addr, uint8_t **buf, uint32_t nb_blocks)
{
    //! \todo implement multiple blocks reading

    return SD_NO_ERROR;
}

sd_error_t sd_write_single_block(sdio_t sdio_, uint32_t addr, uint8_t *buf)
{
    sd_error_t ret;
    _sdio_t *_sdio = sdio_;

    // Reset DPSM configuration
    *sdio_get_DCTRL()  = 0;
    *sdio_get_DLEN()   = 0;
    *sdio_get_DTIMER() = 0xFFFF; //! \todo compute the best data timeout

    *sdio_get_DLEN() = 512;

    // Wait for card to be transfer state
    while (true)
    {
        /***** CMD13 (SEND_STATUS) *****/
        ret = sd_send_command(_sdio, SD_SEND_STATUS, _sdio->rca, SHORT_RESPONSE);

        if (ret != SD_NO_ERROR)
        {
            return ret;
        }

        if ((ret = sd_get_R1(_sdio)) != SD_NO_ERROR)
        {
            return ret;
        }

        if (_sdio->sd_state == SD_STATE_TRAN)
        {
            break;
        }
        else
        {
            ret = SD_TRAN_TIMEOUT;
        }
    }

    /***** CMD24 (WRITE_SINGLE_BLOCK) *****/
    ret = sd_send_command(_sdio, SD_WRITE_SINGLE_BLOCK, addr, SHORT_RESPONSE);

    if (ret != SD_NO_ERROR)
    {
        return ret;
    }

    if ((ret = sd_get_R1(_sdio)) != SD_NO_ERROR)
    {
        return ret;
    }

    // Configure DPSM (Data Path State Machine)
    // 512-bytes blocks, data transfert enabled
    *sdio_get_DCTRL() = SDIO_DCTRL__DBLOCKSIZE_512 | SDIO_DCTRL__DTEN;

    // Enable interrupts
    *sdio_get_ICR() = SDIO_ICR__ALL;
    *sdio_get_MASK() = SDIO_MASK__DCRCFAILIE | SDIO_MASK__DTIMEOUTIE | SDIO_MASK__DBCKENDIE | SDIO_MASK__TXUNDERRIE | SDIO_MASK__STBITERRIE;

    // Configure DMA
    dma_config(_sdio->dma_channel, (uint32_t)sdio_get_FIFO(), (uint32_t)buf, 512 / 4, DMA_SIZE_32bit, DMA_DIRECTION_TO_PERIPHERAL, DMA_INCREMENT_ON);

    // Enable SDIO interrupt line
    *sdio_get_ICR() = SDIO_ICR__ALL;
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_SDIO);

    // Enable and start DMA
    dma_start(_sdio->dma_channel, NULL, NULL);
    *sdio_get_DCTRL() |= SDIO_DCTRL__DMAEN;

    return SD_NO_ERROR;
}

sd_error_t sd_write_multiple_blocks(sdio_t sdio_, uint32_t addr, uint8_t **buf, uint32_t nb_blocks)
{
    //! \todo implement multiple blocks writing

    return SD_NO_ERROR;
}

void sdio_handle_interrupt(sdio_t sdio_)
{
    sd_error_t transfer_error = SD_NO_ERROR;
    _sdio_t *_sdio = sdio_;

    if (*sdio_get_STA() & SDIO_STA__DCRCFAIL)
    {
        transfer_error = SD_DATA_WRONG_CRC;
    }

    if (*sdio_get_STA() & SDIO_STA__DTIMEOUT)
    {
        transfer_error = SD_DATA_TIMEOUT;
    }

    if (*sdio_get_STA() & SDIO_STA__RXOVERR)
    {
        transfer_error = SD_RX_OVERRUN;
    }

    if (*sdio_get_STA() & SDIO_STA__TXUNDERR)
    {
        transfer_error = SD_TX_UNDERRUN;
    }

    if (*sdio_get_STA() & SDIO_STA__STBITERR)
    {
        transfer_error = SD_UNKNOWN_ERROR;
    }

    *sdio_get_MASK() = 0;
    *sdio_get_ICR() = SDIO_ICR__ALL;
    nvic_disable_interrupt_line(NVIC_IRQ_LINE_SDIO);

    if (_sdio->transfer_handler)
    {
        _sdio->transfer_handler((handler_arg_t)transfer_error);
    }
}

uint32_t sd_get_size(sdio_t sdio_)
{
    _sdio_t *_sdio = sdio_;

    if ((_sdio->sd_type == SDSC_1_1) || (_sdio->sd_type == SDSC_2_0))
    {
        return (_sdio->sd_desc.c_size + 1) * (1 << (_sdio->sd_desc.c_size_mult + 2));
    }
    else
    {
        return (_sdio->sd_desc.c_size + 1) << 10;
    }
}
