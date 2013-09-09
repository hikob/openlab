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
 * cwave.c
 *
 *  Created on: Aug 24, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include <stdint.h>
#include <stdlib.h>
#include "platform.h"
#include "printf.h"
#include "uart.h"

#include "event.h"

#include "rf2xx.h"

extern rf2xx_t rf231;

static void char_rx(handler_arg_t arg, uint8_t c);
static void emit(handler_arg_t arg);
static void show_help(handler_arg_t arg);

static uint8_t tx_channel = 11;
static enum
{
    MODE_SINE_min05,
    MODE_SINE_plus05,
    MODE_PSEUDORANDOM,
} mode = MODE_SINE_min05;

static void uart_putc(char c, void* arg)
{
    uart_transfer(uart_external, (const uint8_t*) &c, 1);
}
static struct fprintf_stream uart_out =
{ .putc = uart_putc };

const struct
{
    char* txt;
    uint8_t value;
} powers[] =
{
{ .txt = "PMax", .value = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__3dBm },
{ .txt = "PMax - 3dBm", .value = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__0dBm },
{ .txt = "PMax - 5dBm", .value = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m2dBm },
{ .txt = "PMax - 10dBm", .value = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m7dBm },
{ .txt = "PMax - 15dBm", .value = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m12dBm },
{ .txt = "PMax - 20dBm", .value = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m17dBm }, };

static int tx_pow_ix;

int main()
{
    // Initialize the platform
    platform_init();

    if (uart_external == NULL )
    {
        uart_external = uart_print;
    }
    else
    {
        uart_enable(uart_external, 500000);
    }

    // Debug
    fprintf(&uart_out, "Continuous Wave Test\r\n");

    uart_set_rx_handler(uart_external, char_rx, NULL );
    show_help(NULL );

    // Start RX
    event_post(EVENT_QUEUE_APPLI, emit, NULL );

    // Run
    platform_run();
    return 0;
}

static void emit(handler_arg_t arg)
{
    // This procedure follows the Appendix A of the datasheet

    // Step 1: RESET
    rf2xx_reset(rf231);
    rf2xx_wakeup(rf231);

    // Enable the PA
    rf2xx_pa_enable(rf231);

    // Step 2: Set IRQ mask PLL ON
    rf2xx_reg_write(rf231, RF2XX_REG__IRQ_MASK,
            RF2XX_IRQ_STATUS_MASK__PLL_LOCK);
    rf2xx_reg_read(rf231, RF2XX_REG__IRQ_STATUS);

    // Step 3: disable TX_AUTO_CRC_ON
    rf2xx_reg_write(rf231, RF2XX_REG__TRX_CTRL_1, 0);

    // Step 4: Set State TRX_OFF
    rf2xx_reg_write(rf231, RF2XX_REG__TRX_STATE,
            RF2XX_TRX_STATE__FORCE_TRX_OFF);

    // Step 5: Set clock at pin 17
    rf2xx_reg_write(rf231, RF2XX_REG__TRX_CTRL_0, 0x01);

    // Step 6: Set tx_channel
    rf2xx_reg_write(rf231, RF2XX_REG__PHY_CC_CCA,
            RF2XX_PHY_CC_CCA_DEFAULT__CCA_MODE | tx_channel);

    // Step 7: Set output power as specified
    rf2xx_reg_write(rf231, RF2XX_REG__PHY_TX_PWR,
            powers[tx_pow_ix].value);

    // Step 8: Verify TRX_OFF status
    if (rf2xx_reg_read(rf231, RF2XX_REG__TRX_STATUS)
            != RF2XX_TRX_STATUS__TRX_OFF)
    {
        fprintf(&uart_out, "Step 8 error!\r\n");
    }

    // If radio has external PA, enable DIG3/4
    if (rf2xx_has_pa(rf231))
    {
        // Enable the PA
        rf2xx_pa_enable(rf231);

        // Activate DIG3/4 pins
        uint8_t reg = rf2xx_reg_read(rf231, RF2XX_REG__TRX_CTRL_1);
        reg |= RF2XX_TRX_CTRL_1_MASK__PA_EXT_EN;
        rf2xx_reg_write(rf231, RF2XX_REG__TRX_CTRL_1, reg);
    }

    // Step 9: Enable continuous transmission test mode step #1
    rf2xx_reg_write(rf231, RF2XX_REG__TST_CTRL_DIGI, 0x0F);

    // Step 10: not used for PRBS
    rf2xx_reg_write(rf231, 0x0C, 0x03);

    // Step 11: idem
    rf2xx_reg_write(rf231, 0x0A, 0xA7);

    // Step 12: Write a complete frame buffer with random data
    uint8_t frame_buf[128];
    uint16_t i;
    // Set length
    frame_buf[0] = 127;

    // Set data to send
    for (i = 1; i < 128; i++)
    {
        /*
         * If data is all zero or all one, (0x00 or 0xFF), then a sine wave
         * will be continuously transmitted (fc-0.5 MHz or fc+0.5MHz resp.)
         *
         * If normal modulated data is desired, random data should fill the
         * buffer (with rand() for example)
         */

        if (mode == MODE_PSEUDORANDOM)
        {
            frame_buf[i] = rand(); /* Modulated data */
        }
        else if (mode == MODE_SINE_min05)
        {
            frame_buf[i] = 0x00; /* Sine Wave */
        }
        else
        {
            frame_buf[i] = 0xFF; /* Sine Wave */
        }
    }

    // Write data
    rf2xx_fifo_write(rf231, frame_buf, 128);

    // Step 13: Enable continuous transmission test mode step #2
    rf2xx_reg_write(rf231, RF2XX_REG__PART_NUM, 0x54);

    // Step 14: Enable continuous transmission test mode step #3
    rf2xx_reg_write(rf231, RF2XX_REG__PART_NUM, 0x46);

    // Step 15: Enable PLL_ON state
    rf2xx_reg_write(rf231, RF2XX_REG__TRX_STATE, RF2XX_TRX_STATE__PLL_ON);

    // Step 16: Wait for PLL_LOCK
    while (!rf2xx_reg_read(rf231, RF2XX_REG__IRQ_STATUS)
            & RF2XX_IRQ_STATUS_MASK__PLL_LOCK)
    {
        asm volatile("nop");
    }

    // Step 17: Enter BUSY TX
    rf2xx_reg_write(rf231, RF2XX_REG__TRX_STATE, RF2XX_TRX_STATE__TX_START);

    fprintf(&uart_out, "TX running on channel %u (%u MHz) %s mode, %s output power...\r\n",
            tx_channel, 2405 + 5 * (tx_channel - 11),
            mode == MODE_PSEUDORANDOM ? "pseudo-random" :
                    (mode == MODE_SINE_min05 ? "sine fc-0.5MHz" :
                            "sine fc+0.5MHz"), powers[tx_pow_ix].txt);
}

static void show_help(handler_arg_t arg)
{
    fprintf(&uart_out,
            "\n\nSelect the channel to use with the following values:\r\n");
    int i;
    for (i = 0; i < 16; i++)
    {
        fprintf(&uart_out, "\t'%c': \tchannel %u\r\n", 'a' + i, 11 + i);
    }
    for (i = 0; i < 6; i++)
    {
        fprintf(&uart_out, "\t'%c': \t%s\r\n", '1' + i, powers[i].txt);
    }

    fprintf(&uart_out, "\nSelect the output type with the following values:\r\n");
    fprintf(&uart_out, "\t'*': \tmodulated pseudo random data\r\n");
    fprintf(&uart_out, "\t'-': \tsine wave at fc-0.5MHz\r\n");
    fprintf(&uart_out, "\t'+': \tsine wave at fc+0.5MHz\r\n");
}

static void char_rx(handler_arg_t arg, uint8_t c)
{
    switch (c)
    {
        case 'a' ... 'p':
            tx_channel = 11 + (c - 'a');
            event_post_from_isr(EVENT_QUEUE_APPLI, emit, NULL );
            break;

        case '1' ... '6':
            tx_pow_ix = c - '1';
            event_post_from_isr(EVENT_QUEUE_APPLI, emit, NULL );
            break;

        case '-':
            mode = MODE_SINE_min05;
            event_post_from_isr(EVENT_QUEUE_APPLI, emit, NULL );
            break;
        case '+':
            mode = MODE_SINE_plus05;
            event_post_from_isr(EVENT_QUEUE_APPLI, emit, NULL );
            break;
        case '*':
            mode = MODE_PSEUDORANDOM;
            event_post_from_isr(EVENT_QUEUE_APPLI, emit, NULL );
            break;

        case '?':
            event_post_from_isr(EVENT_QUEUE_APPLI, show_help, NULL );
            break;
        default:
            // Unknown code
            break;
    }
}
