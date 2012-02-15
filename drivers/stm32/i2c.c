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
 * Copyright (C) 2011 HiKoB.
 */

/*
 * i2c.c
 *
 *  Created on: Jul 31, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "rcc.h"
#include "gpio.h"
#include "i2c.h"
#include "i2c_.h"
#include "i2c_registers.h"

#include "printf.h"

typedef enum
{
    I2C_IDLE = 0,
    I2C_SENDING_START = 1,
    I2C_SENDING_ADDRESS = 2,
    I2C_SENDING_DATA = 3,
    I2C_RECEIVING_DATA = 4,
    I2C_SENDING_RESTART = 5
} i2c_state_t;

static void test_ready(_i2c_t *_i2c);

static volatile i2c_state_t state;
// Address to send or receive
static uint8_t address;
// Send buffer
static uint32_t len_send;
static uint32_t cpt_send;
static const uint8_t *buf_send;
// Receive buffer
static uint32_t len_recv;
static uint32_t cpt_recv;
static uint8_t *buf_recv;

void i2c_enable(i2c_t i2c, i2c_clock_mode_t mode)
{
    uint32_t pclk, freq;
    _i2c_t *_i2c = i2c;

    // Disable the I2C EV and ERR interrupt lines in the NVIC
    nvic_disable_interrupt_line(_i2c->irq_line_ev);
    nvic_disable_interrupt_line(_i2c->irq_line_er);

    // Enable the clock for this peripheral
    rcc_apb_enable(_i2c->apb_bus, _i2c->apb_bit);

    // Enable the GPIO port
    gpio_enable(_i2c->gpio);

    // Configure SDA pin
    gpio_set_i2c_sda(_i2c->gpio, _i2c->pin_sda);

    // Configure SCL pin
    gpio_set_i2c_scl(_i2c->gpio, _i2c->pin_scl);

    // Reset the peripheral
    *i2c_get_CR1(_i2c) = I2C_CR1__SWRST;

    // Clear all registers
    *i2c_get_CR1(_i2c) = 0;
    *i2c_get_CR2(_i2c) = 0;
    *i2c_get_CCR(_i2c) = 0;
    *i2c_get_TRISE(_i2c) = 0;
    *i2c_get_SR1(_i2c) = 0;
    *i2c_get_SR2(_i2c) = 0;

    /* Step 1: program the peripheral input clock in CR2 */

    // Compute clock frequency
    pclk = rcc_sysclk_get_clock_frequency(_i2c->apb_bus == 1
                                          ? RCC_SYSCLK_CLOCK_PCLK1 : RCC_SYSCLK_CLOCK_PCLK2);
    freq = pclk / 1000000;

    *i2c_get_CR2(_i2c) = freq & I2C_CR2__FREQ_MASK;

    /* Step 2: configure the clock control registers */

    if(mode == I2C_CLOCK_MODE_FAST)
    {
        // Set Fast Speed mode and bitrate: 400kHz with duty t_low/t_high = 16/9
        *i2c_get_CCR(_i2c) = I2C_CCR__FS | I2C_CCR__DUTY | (freq / 10 * 25);
    }
    else
    {
        // Set Standard Speed mode and bitrate: 100kHz
        *i2c_get_CCR(_i2c) = (freq * 5) & I2C_CCR__CCR_MASK;
    }

    /* Step 3: configure the rise time registers */

    if(mode == I2C_CLOCK_MODE_FAST)
    {
        // Configure maximum rise time
        *i2c_get_TRISE(_i2c) = (freq * 3 / 10 + 1) & I2C_TRISE__TRISE_MASK;
    }
    else
    {
        // Configure maximum rise time
        *i2c_get_TRISE(_i2c) = (freq + 1) & I2C_TRISE__TRISE_MASK;
    }

    /* Step 4: program the CR1 register to enable the peripheral */

    // Set I2C mode, enable ACK and enable I2C
    *i2c_get_CR1(_i2c) = I2C_CR1__ACK | I2C_CR1__PE;

    // Enable EV and BUF interrupt
    *i2c_get_CR2(_i2c) |= I2C_CR2__ITEVTEN | I2C_CR2__ITBUFEN
                          | I2C_CR2__ITERREN;

    // Initialize I2C state
    state = I2C_IDLE;

    // Enable I2C EV and ERR interrupt line in the NVIC
    nvic_enable_interrupt_line(_i2c->irq_line_ev);
    nvic_enable_interrupt_line(_i2c->irq_line_er);
}

void i2c_disable(i2c_t i2c)
{
    _i2c_t *_i2c = i2c;

    // Diable the clock for this peripheral
    rcc_apb_disable(_i2c->apb_bus, _i2c->apb_bit);

    // Disable the I2C
    *i2c_get_CR1(_i2c) = 0;

    // Disable all interrupts
    *i2c_get_CR2(_i2c) = 0;
}

uint32_t i2c_tx(i2c_t i2c, uint8_t addr, const uint8_t *tx_buffer,
                uint16_t length)
{
    _i2c_t *_i2c = i2c;

    // Copy the data to send, data to read. Set the current state for the
    // I2C state machine and the issue the start condition.
    address = addr;

    len_send = length;
    cpt_send = 0;
    buf_send = tx_buffer;

    len_recv = 0;
    cpt_recv = 0;
    buf_recv = NULL;

    state = I2C_SENDING_START;

    // Set ACK bit before starting anything
    *i2c_get_CR1(_i2c) |= I2C_CR1__ACK;

    // Generate START condition to initiate the transfer
    *i2c_get_CR1(_i2c) |= I2C_CR1__START;

    while(state != I2C_IDLE)
        ;

    // Wait until busy is cleared
    while(*i2c_get_SR2(_i2c) & I2C_SR2__BUSY)
    {
    }

    // TODO: return a real error code
    return 0;
}

uint32_t i2c_rx(i2c_t i2c, uint8_t addr, uint8_t *rx_buffer, uint16_t length)
{
    _i2c_t *_i2c = i2c;

    // Copy the data to send, data to read. Set the current state for the
    // I2C state machine and the issue the start condition.
    address = addr;

    len_send = 0;
    cpt_send = 0;
    buf_send = NULL;

    len_recv = length;
    cpt_recv = 0;
    buf_recv = rx_buffer;

    state = I2C_SENDING_START;

    // Set ACK bit before starting anything
    *i2c_get_CR1(_i2c) |= I2C_CR1__ACK;

    // Generate START condition to initiate the transfer
    *i2c_get_CR1(_i2c) |= I2C_CR1__START;

    while(state != I2C_IDLE)
        ;

    // Wait until busy is cleared
    while(*i2c_get_SR2(_i2c) & I2C_SR2__BUSY)
    {
    }

    // TODO: return a real error code
    return 0;
}

uint32_t i2c_tx_rx(i2c_t i2c, uint8_t addr, const uint8_t *tx_buffer,
                   uint16_t tx_length, uint8_t *rx_buffer, uint16_t rx_length)
{
    _i2c_t *_i2c = i2c;

    // Do the before-transfer test
    test_ready(_i2c);

    // Copy the data to send, data to read. Set the current state for the
    // I2C state machine and the issue the start condition.
    address = addr;

    len_send = tx_length;
    cpt_send = 0;
    buf_send = tx_buffer;

    len_recv = rx_length;
    cpt_recv = 0;
    buf_recv = rx_buffer;

    state = I2C_SENDING_START;

    // Set ACK bit before starting anything
    *i2c_get_CR1(_i2c) |= I2C_CR1__ACK;

    // Generate START condition to initiate the transfer
    *i2c_get_CR1(_i2c) |= I2C_CR1__START;

    while(state != I2C_IDLE)
        ;

    // Wait until busy is cleared
    while(*i2c_get_SR2(_i2c) & I2C_SR2__BUSY)
    {
    }

    // TODO: return a real error code
    return 0;
}

void i2c_handle_ev_interrupt(_i2c_t *_i2c)
{
    uint32_t sr1 = *i2c_get_SR1(_i2c);
    uint32_t sr2;

    if(sr1 & I2C_SR1__SB)
    {
        // Start bit has been issued.
        // The flag is cleared after reading SR1 (already done at the
        // beginning of the function) and writing DR (done right now)

        // If there there is nothing to send, set the lower bit
        // of address to 1 as we are in receiving mode
        if(len_send == 0)
        {
            // Send address in order to receive data
            *i2c_get_DR(_i2c) = address | 1;

            // In the case of a 2-byte reception we need to set POS bit
            if(len_recv == 2)
            {
                *i2c_get_CR1(_i2c) |= I2C_CR1__POS;
            }
        }
        else
        {
            // Send address in order to send data
            *i2c_get_DR(_i2c) = address;
        }

        *i2c_get_CR1(_i2c) |= I2C_CR1__ACK;

        state = I2C_SENDING_ADDRESS;
    }

    if(sr1 & I2C_SR1__ADDR)
    {
        // Address byte has been sent.
        // The flag is cleared after reading both SR1 and SR2

        // Send the first byte if any
        if(len_send > 0)
        {
            // Clear the flag by reading SR2. This must be done before sendin data
            sr2 = *i2c_get_SR2(_i2c);

            // Send data
            *i2c_get_DR(_i2c) = buf_send[cpt_send];
            cpt_send++;

            if(cpt_send == len_send)
            {
                // The only byte to send has already been sent so we need to issue
                // a STOP condition if there is nothing to receive or
                // a RESTART condition otherwise
                if(len_recv == 0)
                {
                    // There is nothing to receive
                    *i2c_get_CR1(_i2c) |= I2C_CR1__STOP;

                    // The transfer is complete
                    state = I2C_IDLE;
                }
                else
                {
                    // We need to read something
                    *i2c_get_CR1(_i2c) |= I2C_CR1__START;

                    // Wait for the new start condition to be issued
                    state = I2C_SENDING_RESTART;
                }

                // Indicate that there is nothing else to send
                len_send = 0;
                cpt_send = 0;
            }
            else
            {
                // There is something else to send
                // Wait for the first byte to be sent
                state = I2C_SENDING_DATA;
            }
        }
        else
        {
            switch(len_recv)
            {
                case 1:
                    // In this case we need to clear ACK bit right now
                    *i2c_get_CR1(_i2c) &= ~I2C_CR1__ACK;

                    // Clear the flag by reading SR2
                    sr2 = *i2c_get_SR2(_i2c);

                    // We also need to program the STOP condition
                    *i2c_get_CR1(_i2c) |= I2C_CR1__STOP;
                    break;

                case 2:
                    // Clear the flag by reading SR2
                    sr2 = *i2c_get_SR2(_i2c);

                    // Clear ACK bit
                    *i2c_get_CR1(_i2c) &= ~I2C_CR1__ACK;
                    break;

                default:
                    // In the default case we do not need to do anything
                    // except clearing the flag by reading SR2
                    sr2 = *i2c_get_SR2(_i2c);
            }

            // Wait for the first byte to read
            state = I2C_RECEIVING_DATA;
        }
    }

    if(sr1 & I2C_SR1__BTF)
    {
        // This is the only case where we need BTF flag
        // We have waited until the last to bytes to read are available
        if((state == I2C_RECEIVING_DATA) && (len_recv == 2))
        {
            // Program a STOP condition before reading the second last data
            *i2c_get_CR1(_i2c) |= I2C_CR1__STOP;

            // Read Data[N-1]
            buf_recv[cpt_recv] = *i2c_get_DR(_i2c);
            cpt_recv++;

            // Read Data[N]
            buf_recv[cpt_recv] = *i2c_get_DR(_i2c);
            cpt_recv++;

            // Unset POS bit in order not disturb next transfers
            *i2c_get_CR1(_i2c) &= ~I2C_CR1__POS;

            // Goto idle state
            state = I2C_IDLE;
        }
    }

    if(sr1 & I2C_SR1__ADD10)
    {
        // This should not happen as we do not handle 10-bits addresses
        log_error("ADD10 interrupt!");
    }

    if(sr1 & I2C_SR1__STOPF)
    {
        // This should not happen as we are in master mode
        log_error("STOPF interrupt!");
    }

    // The following two cases should not happen as ITBUFEN is not set

    if(sr1 & I2C_SR1__RXNE)
    {
        if(state == I2C_RECEIVING_DATA)
        {
            // We are receiving data
            if(len_recv == 1)
            {
                // This is the only byte to read
                buf_recv[cpt_recv] = *i2c_get_DR(_i2c);

                // Indicate that there is nothing else to receive
                len_recv = 0;
                cpt_recv = 0;

                // The transfer is complete
                state = I2C_IDLE;
            }
            else if(len_recv == 2)
            {
                // Do not read yet, wait for BTF to be issued

                // Stay in the I2C_RECEIVING_DATA state
            }
            else
            {
                // Read data
                buf_recv[cpt_recv] = *i2c_get_DR(_i2c);
                cpt_recv++;

                if(len_recv - cpt_recv == 1)
                {
                    // We need to clear ACK bit to be sure NACK is sent when the last data is received
                    *i2c_get_CR1(_i2c) &= ~I2C_CR1__ACK;

                    // Program a STOP condition before reading the second last data
                    *i2c_get_CR1(_i2c) |= I2C_CR1__STOP;
                }

                if(len_recv == cpt_recv)
                {
                    state = I2C_IDLE;
                }

                // Else stay in the I2C_RECEIVING_DATA state
            }

            // In other cases there is nothing to do
        }

        // This flag should not be set when we are not in receiving mode
    }

    if(sr1 & I2C_SR1__TXE)
    {
        // The flag is cleared after reading SR1 (already done) and reading or writing DR
        if(state == I2C_SENDING_DATA)
        {
            if(len_send == cpt_send)
            {
                // The last byte has been sent so we need to issue
                // a STOP condition if there is nothing to receive or
                // a RESTART condition otherwise
                if(len_recv == 0)
                {
                    // There is nothing to receive
                    *i2c_get_CR1(_i2c) |= I2C_CR1__STOP;

                    // The transfer is complete
                    state = I2C_IDLE;
                }
                else
                {
                    // We need to read something
                    *i2c_get_CR1(_i2c) |= I2C_CR1__START;

                    state = I2C_SENDING_RESTART;
                }

                // Indicate that there is nothing else to send
                len_send = 0;
                cpt_send = 0;
            }
            else
            {
                // Send the data
                *i2c_get_DR(_i2c) = buf_send[cpt_send];
                cpt_send++;

                // Stay in the I2C_SENDING_DATA state
            }
        }

        // If we are not sending data, do nothing
    }
}

void i2c_handle_er_interrupt(_i2c_t *_i2c)
{
    uint16_t sr1 = *i2c_get_SR1(_i2c);
    uint16_t sr2 = *i2c_get_SR2(_i2c);

    log_error("I2C ERROR, SR1: %x, SR2: %x, state: %x", sr1, sr2, state);

    if(sr1 & I2C_SR1__BERR)
    {
        log_error("Bus Error");
    }

    if(sr1 & I2C_SR1__ARLO)
    {
        log_error("Arbitration Lost");
    }

    if(sr1 & I2C_SR1__AF)
    {
        log_error("Acknowledge Failure");
    }

    if(sr1 & I2C_SR1__OVR)
    {
        log_error("Overrun");
    }

    if(sr1 & I2C_SR1__PECERR)
    {
        log_error("PEC error");
    }

    if(sr1 & I2C_SR1__TIMEOUT)
    {
        log_error("Timeout");
    }

    if(sr1 & I2C_SR1__SMBALERT)
    {
        log_error("SMBAlert");
    }

    HALT();
}

static void test_ready(_i2c_t *_i2c)
{
    uint16_t reg;

    // Check State
    if(state != I2C_IDLE)
    {
        log_error("I2C State not Idle");
    }
    else if(((reg = *i2c_get_CR1(_i2c)) & I2C_CR1__PE) == 0)
    {
        log_error("I2C CR1 not PE: %x", reg);
    }
    else if((reg = (*i2c_get_CR2(_i2c) & (I2C_CR2__ITERREN | I2C_CR2__ITEVTEN
                                          | I2C_CR2__ITEVTEN))) != (I2C_CR2__ITERREN | I2C_CR2__ITEVTEN
                                                  | I2C_CR2__ITEVTEN))
    {
        log_error("I2C CR2 interrupts not enabled: %x", reg);
    }

    while((reg = *i2c_get_SR2(_i2c)) & I2C_SR2__BUSY)
    {
        log_error("I2C SR2 Busy not cleared: %x", reg);
    }
}
