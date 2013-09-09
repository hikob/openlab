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
 * pga308.c
 *
 *  Created on: Nov 21, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include <stdint.h>

#include "pga308.h"
#include "pga308_.h"

#include "printf.h"
#include "debug.h"

enum
{
    PGA308_SYNC = 0x55,
    PGA308_ACCESS_READ = 0x80,
    PGA308_ACCESS_WRITE = 0x00,
    PGA308_ACCESS_OTP = 0x40,
    PGA308_ACCESS_RAM = 0x00,
};

enum
{
    PGA308_REG__ZDAC = 0x00,
    PGA308_REG__GDAC = 0x01,
    PGA308_REG__CFG0 = 0x02,
    PGA308_REG__CFG1 = 0x03,
    PGA308_REG__CFG2 = 0x04,
    PGA308_REG__CHKS = 0x05,
    PGA308_REG__CHSR = 0x06,
    PGA308_REG__SFTC = 0x07,
    PGA308_REG__OENC = 0x08,
    PGA308_REG__ALRM = 0x09,
    PGA308_REG__OTPS = 0x0A,
};

enum
{
    PGA308_SFTC__SWL_SOFTLOCK = 0x0050
};

static void pga308_char_rx(handler_arg_t arg, uint8_t c);
static uint16_t pga308_read_reg(uint8_t addr);
static void pga308_write_reg(uint8_t addr, uint16_t value);

static inline void wait_char();

static struct
{
    uart_t uart;
    gpio_t gpio;
    gpio_pin_t enable_pin;
    adc_t adc;
    uint8_t adc_channel;

    // Configured registers
    uint16_t zdac;
    uint16_t gdac;
    uint16_t cfg0;

} pga308_config;

static volatile uint8_t rx_c, flag;

void pga308_init(uart_t uart, gpio_t gpio, gpio_pin_t enable_pin,
                 adc_t adc, uint8_t adc_channel)
{
    // Store the uart
    pga308_config.uart = uart;

    // Store the enable pin
    pga308_config.gpio = gpio;
    pga308_config.enable_pin = enable_pin;

    // Store the ADC channel
    pga308_config.adc = adc;
    pga308_config.adc_channel = adc_channel;

    // Configure it as output
    gpio_set_output(gpio, enable_pin);
    gpio_config_output_type(gpio, enable_pin, GPIO_TYPE_OPEN_DRAIN);

    // Disable for now
    pga308_disable();

    // Set the handler
    uart_set_rx_handler(pga308_config.uart, pga308_char_rx, NULL);
}

void pga308_enable()
{
    // Enable the power
    gpio_pin_clear(pga308_config.gpio, pga308_config.enable_pin);

    // Enable the UART
    uart_enable(pga308_config.uart, 114000);

    // Enter Software Lock Mode
    pga308_write_reg(PGA308_REG__SFTC, PGA308_SFTC__SWL_SOFTLOCK);

    // Set DOUT/VCLAMP as VCLAMP
    pga308_write_reg(PGA308_REG__CFG2, 0x0100);

    // Write stored registers
    pga308_write_reg(PGA308_REG__ZDAC, pga308_config.zdac);
    pga308_write_reg(PGA308_REG__GDAC, pga308_config.gdac);
    pga308_write_reg(PGA308_REG__CFG0, pga308_config.cfg0);

    // Enable alarms
    pga308_write_reg(PGA308_REG__CFG1, 0x1780);

    // Enable output
    pga308_write_reg(PGA308_REG__CFG2, 0x0100);
}

void pga308_configure_gain(pga308_input_gain_t input_gain, uint16_t zero_dac,
                           uint16_t gain_dac, pga308_output_gain_t output_gain)
{
    // Store the registers values
    pga308_config.zdac = zero_dac;
    pga308_config.gdac = gain_dac;
    pga308_config.cfg0 = (input_gain << 8) | (output_gain << 13);

    // Call enable to update the registers
    pga308_enable();
}

void pga308_disable()
{
    gpio_pin_set(pga308_config.gpio, pga308_config.enable_pin);
    uart_disable(pga308_config.uart);
}

adc_t pga308_get_adc()
{
    return pga308_config.adc;
}
uint8_t pga308_get_adc_channel()
{
    return pga308_config.adc_channel;
}

static uint16_t pga308_read_reg(uint8_t addr)
{
    uint8_t buf[2];
    uint32_t i;

    // Transfer sync and addr
    buf[0] = PGA308_SYNC;
    buf[1] = PGA308_ACCESS_RAM | PGA308_ACCESS_READ | addr;

    // Write first
    flag = 0;
    uart_transfer(pga308_config.uart, buf, 1);
    wait_char();

    // Write second
    flag = 0;
    uart_transfer(pga308_config.uart, buf + 1, 1);
    wait_char();

    flag = 0;

    // Read value
    for (i = 0; i < 2; i++)
    {
        wait_char();
        flag = 0;
        // Store the char
        buf[i] = rx_c;
    }

    return (((uint16_t) buf[1]) << 8) + buf[0];
}
static void pga308_write_reg(uint8_t addr, uint16_t value)
{
    uint8_t buf[4];

    // Transfer sync, addr and data
    buf[0] = PGA308_SYNC;
    buf[1] = PGA308_ACCESS_RAM | PGA308_ACCESS_WRITE | addr;
    buf[2] = value & 0xFF;
    buf[3] = value >> 8, uart_transfer(pga308_config.uart, buf, 4);
}

void pga308_print_regs()
{
    (void) pga308_read_reg;
    log_printf("PGA308 registers:\n");
    log_printf("\tZDAC:\t%04x\n", pga308_read_reg(PGA308_REG__ZDAC));
    log_printf("\tGDAC:\t%04x\n", pga308_read_reg(PGA308_REG__GDAC));
    log_printf("\tCFG0:\t%04x\n", pga308_read_reg(PGA308_REG__CFG0));
    log_printf("\tCFG1:\t%04x\n", pga308_read_reg(PGA308_REG__CFG1));
    log_printf("\tCFG2:\t%04x\n", pga308_read_reg(PGA308_REG__CFG2));
    log_printf("\tCHKS:\t%04x\n", pga308_read_reg(PGA308_REG__CHKS));
    log_printf("\tCHSR:\t%04x\n", pga308_read_reg(PGA308_REG__CHSR));
    log_printf("\tSFTC:\t%04x\n", pga308_read_reg(PGA308_REG__SFTC));
    log_printf("\tOENC:\t%04x\n", pga308_read_reg(PGA308_REG__OENC));
    log_printf("\tALRM:\t%04x\n", pga308_read_reg(PGA308_REG__ALRM));
    log_printf("\tOTPS:\t%04x\n", pga308_read_reg(PGA308_REG__OTPS));
}
static void pga308_char_rx(handler_arg_t arg, uint8_t c)
{
    // Store char
    rx_c = c;
    // Set flag
    flag = 1;
}
static inline void wait_char()
{
    // Wait for a received char
    while (flag == 0)
    {
        asm volatile("nop");
    }
}
