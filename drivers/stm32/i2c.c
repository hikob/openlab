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
 * i2c.c
 *
 *  Created on: Jul 31, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 *      Author: Damien Hedde        <damien.hedde.at.hikob.com>
 */

#include "rcc.h"
#include "gpio.h"
#include "i2c.h"
#include "i2c_.h"
#include "i2c_registers.h"
#include "nvic_.h"

#ifdef I2C__SLAVE_SUPPORT
#include "i2c_slave.h"
#endif

#define LOG_LEVEL LOG_LEVEL_DEBUG
#include "printf.h"
#include "debug.h"

#ifdef I2C_DEBUG
#include "drivers.h"
#ifndef I2C_DEBUG_BUFSIZE
#error I2C_DEBUG_BUFSIZE is undefined
#endif
struct i2c_dbg_info {
    uint16_t time;
    uint16_t sr1;
    uint8_t  state;
    uint8_t  count;
} __attribute__((packed));
struct i2c_dbg_struct {
    unsigned idx;
    struct i2c_dbg_info buffer[I2C_DEBUG_BUFSIZE];
} __attribute__((packed));
extern struct i2c_dbg_struct * volatile i2c_dbg_struct;
struct i2c_dbg_struct * volatile i2c_dbg_struct = NULL;
#ifdef I2C_DEBUG_TIMER
#define I2C_DEBUG_TIME timer_time(I2C_DEBUG_TIMER)
#else
#define I2C_DEBUG_TIME 0
#endif
#define I2C_DEBUG_LOG(sr1_, state_, count_) do { \
    struct i2c_dbg_struct *s_ = i2c_dbg_struct; \
    if (!s_) \
        break; \
    struct i2c_dbg_info *i_ = &s_->buffer[s_->idx++ % I2C_DEBUG_BUFSIZE]; \
    i_->time = I2C_DEBUG_TIME; \
    i_->sr1 = (sr1_); \
    i_->state = (state_); \
    i_->count = (count_); \
} while (0)
#else
#define I2C_DEBUG_LOG(a,b,c)
#endif

static void tx_rx_start(const _i2c_t *_i2c, uint8_t addr, const uint8_t *tx_buffer,
                   uint16_t tx_length, uint8_t *rx_buffer, uint16_t rx_length,
                   result_handler_t handler, handler_arg_t arg);
static void tx_rx_end(const _i2c_t *_i2c, i2c_state_t state);
static uint32_t tx_rx_wait(const _i2c_t *_i2c);
static void test_ready(const _i2c_t *_i2c);

void i2c_enable(i2c_t i2c, i2c_clock_mode_t mode)
{
    uint32_t pclk, freq;
    const _i2c_t *_i2c = i2c;

    // Disable the I2C EV and ERR interrupt lines in the NVIC
    nvic_disable_interrupt_line(_i2c->irq_line_ev);
    nvic_disable_interrupt_line(_i2c->irq_line_er);

    // Enable the clock for this peripheral
    rcc_apb_enable(_i2c->apb_bus, _i2c->apb_bit);

    // Reset the peripheral
    *i2c_get_CR1(_i2c) = I2C_CR1__SWRST;

    // Clear all registers
    *i2c_get_CR2(_i2c) = 0;
    *i2c_get_CCR(_i2c) = 0;
    *i2c_get_TRISE(_i2c) = 0;
    *i2c_get_SR1(_i2c) = 0;
    *i2c_get_SR2(_i2c) = 0;
    *i2c_get_OAR1(_i2c) = 0;
    *i2c_get_OAR2(_i2c) = 0;

    // Release SWRST
    *i2c_get_CR1(_i2c) = 0;

    /* Step 1: program the peripheral input clock in CR2 */

    // Compute clock frequency
    pclk = rcc_sysclk_get_clock_frequency(
               _i2c->apb_bus == 1 ? RCC_SYSCLK_CLOCK_PCLK1
               : RCC_SYSCLK_CLOCK_PCLK2);
    freq = pclk / 1000000;

    *i2c_get_CR2(_i2c) = freq & I2C_CR2__FREQ_MASK;

    /* Step 2: configure the clock control registers */

    if (mode == I2C_CLOCK_MODE_FAST)
    {
        // Set Fast Speed mode and bitrate: ~400kHz
        uint32_t ccr = freq * 10;
        ccr = ccr / 12 + (ccr % 12 ? 1 : 0);
        *i2c_get_CCR(_i2c) = I2C_CCR__FS | ccr;
    }
    else
    {
        // Set Standard Speed mode and bitrate: 100kHz
        *i2c_get_CCR(_i2c) = (freq * 5) & I2C_CCR__CCR_MASK;
    }

    /* Step 3: configure the rise time registers */

    if (mode == I2C_CLOCK_MODE_FAST)
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

    // Enable I2C
    *i2c_get_CR1(_i2c) = I2C_CR1__PE;

    // Enable EVT and ERR interrupt
    // Note: do not enable BUF to avoid stm32f problem
    *i2c_get_CR2(_i2c) |= I2C_CR2__ITEVTEN | I2C_CR2__ITERREN;

    // Initialize I2C state
    _i2c->data->state = I2C_IDLE;
    _i2c->data->len_send = 0;
    _i2c->data->len_recv = 0;
    _i2c->data->cpt_send = 0;
    _i2c->data->cpt_recv = 0;
    _i2c->data->transfer_handler = NULL;

    // Enable I2C EV and ERR interrupt line in the NVIC
    nvic_enable_interrupt_line(_i2c->irq_line_ev);
    nvic_enable_interrupt_line(_i2c->irq_line_er);
}

void i2c_disable(i2c_t i2c)
{
    const _i2c_t *_i2c = i2c;

    // Disable the clock for this peripheral
    rcc_apb_disable(_i2c->apb_bus, _i2c->apb_bit);

    // Disable the I2C
    *i2c_get_CR1(_i2c) = 0;

    // Disable all interrupts
    *i2c_get_CR2(_i2c) = 0;
}

unsigned i2c_tx_rx_async(i2c_t i2c, uint8_t addr, const uint8_t *tx_buffer,
                   uint16_t tx_length, uint8_t *rx_buffer, uint16_t rx_length,
                   result_handler_t handler, handler_arg_t arg)
{
    const _i2c_t *_i2c = i2c;
    uint32_t res = 0;

    // Start the transfer
    tx_rx_start(_i2c, addr, tx_buffer, tx_length, rx_buffer, rx_length, handler, arg);

    // Eventually wait for the transfer to be completed
    if (handler == NULL)
    {
        res = tx_rx_wait(_i2c);
    }

    return res;
}

#ifdef I2C__SLAVE_SUPPORT
void i2c_slave_configure(i2c_t i2c, i2c_slave_handler_t handler)
{
    const _i2c_t *_i2c = i2c;

    _i2c->data->slave_handler = handler;
    if (handler)
    {
        // Enable address ACK
        *i2c_get_CR1(_i2c) |= I2C_CR1__ACK;
    }
    else
    {
        // Disable address ACK
        *i2c_get_CR1(_i2c) &= ~I2C_CR1__ACK;
    }
}

void i2c_slave_set_address(i2c_t i2c, uint8_t addr)
{
    const _i2c_t *_i2c = i2c;

    *i2c_get_OAR1(_i2c) = addr & 0xfe;
}
#endif

void i2c_handle_ev_interrupt(const _i2c_t *_i2c)
{
    _i2c_data_t *const data = _i2c->data;
    uint32_t sr1 = *i2c_get_SR1(_i2c);
    uint32_t sr2 = 0;
    (void) sr2; // clear defined but not used warning warning

    I2C_DEBUG_LOG(sr1, data->state, data->state == I2C_RECEIVING_DATA ? data->len_recv - data->cpt_recv : data->len_send - data->cpt_send);

    /*
     * EVxxx refer to the STM32 I2C datasheet's events
     *
     * STM32F1xx errata tells EV6_1 EV6_3 EV7 EV7_1 EV8 (and slave ones EV2 EV3)
     * should be handled before the current byte is transferred.
     * + EV6_1, EV6_3 must be un-interrupted
     * + EV8 must be done early enough
     * In consequence we mask TXBUFEN (RXNE and TXNE interrupt)
     */

    if (sr1 & I2C_SR1__SB)
    {
        // ** EV5 **
        // Start bit has been issued.
        // The flag is cleared after reading SR1 (already done at the
        // beginning of the function) and writing DR (done right now)

        // If there there is nothing to send, set the lower bit
        // of address to 1 as we are in receiving mode
        if (data->len_send == 0)
        {
            // Send address in order to receive data
            *i2c_get_DR(_i2c) = data->address | 1;

            // In the case of a 2-byte reception we need to set POS bit
            if (data->len_recv == 2)
            {
                *i2c_get_CR1(_i2c) |= I2C_CR1__POS;
            }
        }
        else
        {
            // Send address in order to send data
            *i2c_get_DR(_i2c) = data->address;
        }

        // Disable Buffer Interrupt
        *i2c_get_CR2(_i2c) &= ~I2C_CR2__ITBUFEN;

        // Enable ACK
        *i2c_get_CR1(_i2c) |= I2C_CR1__ACK;

        data->state = I2C_SENDING_ADDRESS;
    }

    if (sr1 & I2C_SR1__ADDR && data->state == I2C_SENDING_ADDRESS)
    {
        // ** EV6 **
        // Master mode: Address byte has been sent.
        // The flag is cleared after reading both SR1 and SR2

        // Send the first byte if any
        if (data->len_send > 0)
        {
            // Clear the flag by reading SR2. This must be done before sending data
            sr2 = *i2c_get_SR2(_i2c);

            // ** EV8_1 **
            // Send data
            *i2c_get_DR(_i2c) = data->buf_send[data->cpt_send];
            data->cpt_send++;

            if (data->cpt_send == data->len_send)
            {
                // ** EV8_2 **
                // The only byte to send has already been sent so we need to issue
                // a STOP condition if there is nothing to receive or
                // a RESTART condition otherwise
                if (data->len_recv == 0)
                {
                    // There is nothing to receive
                    *i2c_get_CR1(_i2c) |= I2C_CR1__STOP;

                    // The transfer is complete
                    tx_rx_end(_i2c, I2C_IDLE);
                }
                else
                {
                    // We need to read something
                    *i2c_get_CR1(_i2c) |= I2C_CR1__START;

                    // Wait for the new start condition to be issued
                    data->state = I2C_SENDING_RESTART;
                }

                // Indicate that there is nothing else to send
                data->len_send = 0;
                data->cpt_send = 0;
            }
            else
            {
                // There is something else to send
                // Wait for the first byte to be sent
                data->state = I2C_SENDING_DATA;

                // STMF: do not enable ITBUFEN since we can not ensure
                // to handle EV8 before current byte transfer after TXNE
                // instead, we'll wait for BTF (which lower the transfer speed)
            }
        }
        else
        {
            switch (data->len_recv)
            {
                case 1:
                    // ** EV6_3 **
                    // STM32F_errata:
                    // + the sequence read_SR2 STOP should be fast enough
                    // + or SCL must be tied low in during it

                    // In this case we need to clear ACK bit right now
                    *i2c_get_CR1(_i2c) &= ~I2C_CR1__ACK;

                    // Clear the flag by reading SR2
                    sr2 = *i2c_get_SR2(_i2c);

                    // We also need to program the STOP condition
                    *i2c_get_CR1(_i2c) |= I2C_CR1__STOP;

                    //activate ITBUFEN since we'll only receive a single byte
                    *i2c_get_CR2(_i2c) |= I2C_CR2__ITBUFEN;
                    break;

                case 2:
                    // ** EV6_1 **
                    // STM32F_errata:
                    // + the sequence read_SR2 NACK should be fast enough
                    // + or SCL must be tied low in during it

                    // Clear the flag by reading SR2
                    sr2 = *i2c_get_SR2(_i2c);

                    // Clear ACK bit
                    *i2c_get_CR1(_i2c) &= ~I2C_CR1__ACK;
                    break;

                default:
                    // In the default case we do not need to do anything
                    // except clearing the flag by reading SR2
                    sr2 = *i2c_get_SR2(_i2c);
                    break;
            }

            // Wait for the first byte to read
            data->state = I2C_RECEIVING_DATA;
        }

        // return there
        // in particular, we must not execute TXNE part because
        // we may have written into DR
        return;
    }
#ifdef I2C__SLAVE_SUPPORT
    else if (sr1 & I2C_SR1__ADDR && (data->state == I2C_IDLE ||
                (data->state == I2C_SL_RX && !(sr1 & I2C_SR1__RXNE))))
    {
        // ** EV1 **
        // Slave mode: Address byte has been acknowledged.

        // Clear the flag by reading SR2.
        sr2 = *i2c_get_SR2(_i2c);

        // Disable Buffer Interrupt
        *i2c_get_CR2(_i2c) &= ~I2C_CR2__ITBUFEN;

        if (sr2 & I2C_SR2__TRA)
        {
            uint8_t byte = 0;
            // we will send data
            data->state = I2C_SL_TX;
            if (data->slave_handler)
            {
                data->slave_handler(I2C_SLAVE_EV_TX_START, NULL);
                data->slave_handler(I2C_SLAVE_EV_TX_BYTE, &byte);
            }

            //send the first byte
            *i2c_get_DR(_i2c) = byte;
        }
        else
        {
            // we will receive data
            data->state = I2C_SL_RX;
            if (data->slave_handler)
            {
                data->slave_handler(I2C_SLAVE_EV_RX_START, NULL);
            }
        }

        return;
    }
#endif

    if (sr1 & I2C_SR1__ADD10)
    {
        // This should not happen as we do not handle 10-bits addresses
        log_error("ADD10 interrupt!");

        // Set stop bit to stop transfer
        *i2c_get_CR1(_i2c) |= I2C_CR1__STOP;

        // Update State
        tx_rx_end(_i2c, I2C_ERROR);
        return;
    }

    if (sr1 & I2C_SR1__RXNE)
    {
        /* Note:
         *  when RXNE:
         *     + 1 byte is in I2C controller buffers
         *     + bus is not stalled (one byte may be in transfer)
         *  when BTF & RXNE:
         *     + 2 bytes are in I2C controller buffers
         *     + bus is stalled (controller wait we read a byte)
         */
        if (data->state == I2C_RECEIVING_DATA)
        {
            /* * * * * * * WARNING STM32F1xx HARDWARE ERRATA * * * * * * *
             * RXNE-only events should not be handled since it sometimes *
             * fails. Only BTF & RXNE events should be handled (with the *
             * consequence of slowing down the transfer).                *
             *                                                           *
             * It seems that when a RXNE interrupt is handled 'around'   *
             * the end of the next byte reception, the DR register read  *
             * is ignored by the i2c controller: it does not flush the   *
             * DR with next byte                                         *
             * Thus we read twice the same byte and we read effectively  *
             * read one byte less than expected from the i2c slave point *
             * of view.                                                  *
             * Example:                                                  *
             *  + we want to receive 6 bytes (B1 to B6)                  *
             *  + the problem appear when reading B3                     *
             *  -> we read B1 B2 B3 B3 B4 B5 (B3 twice)                  *
             *  -> the i2c transfer was B1 B2 B3 B4 B5 (B6 is not sent)  *
             * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
            int remaining = data->len_recv - data->cpt_recv;
            if (remaining <= 0)
            {
                //should not happen
                log_error("Bad I2C receiving state");
                *i2c_get_CR1(_i2c) |= I2C_CR1__STOP;
                *i2c_get_DR(_i2c);
                *i2c_get_DR(_i2c);
                tx_rx_end(_i2c, I2C_ERROR);
            }
            else
            {
                switch (remaining)
                {
                    default:// >=4
                        if (sr1 & I2C_SR1__BTF)
                        {
                            // ** EV7 **
                            // Read the byte that is in buffer
                            data->buf_recv[data->cpt_recv++] = *i2c_get_DR(_i2c);
                        }
                        break;

                    case 3:
                        // Bytes 1..N-3 have been read
                        if (sr1 & I2C_SR1__BTF)
                        {
                            // ** EV7 **
                            // Bytes N-2 and N-1 have been received

                            // Program NACK
                            *i2c_get_CR1(_i2c) &= ~I2C_CR1__ACK;
                            // Read N-3 to ensure BTF after N (before STOP)
                            data->buf_recv[data->cpt_recv++] = *i2c_get_DR(_i2c);
                        }
                        break;

                    case 2:
                        // Bytes 1..N-2 have been read
                        if (sr1 & I2C_SR1__BTF)
                        {
                            // ** (EV7_1 and EV7) or EV7_3 **
                            // STM32F_errata:
                            // + the sequence STOP read_DR read_DR should be fast enough
                            // + or SCL must be tied low in during it

                            // Program STOP
                            *i2c_get_CR1(_i2c) |= I2C_CR1__STOP;
                            // Read N-2
                            data->buf_recv[data->cpt_recv++] = *i2c_get_DR(_i2c);
                            // Read N-1
                            data->buf_recv[data->cpt_recv++] = *i2c_get_DR(_i2c);

                            // Indicate that there is nothing else to receive
                            data->len_recv = 0;
                            data->cpt_recv = 0;
                            // Signal the transfer is complete
                            tx_rx_end(_i2c, I2C_IDLE);
                        }
                        break;

                    case 1:
                        // ** EV7 **
                        // Happen only on single byte transfers
                        // The byte has been received but not read
                        // Note: BTF should not be set
                        data->buf_recv[data->cpt_recv++] = *i2c_get_DR(_i2c);

                        // Indicate that there is nothing else to receive
                        data->len_recv = 0;
                        data->cpt_recv = 0;

                        // The transfer is complete
                        tx_rx_end(_i2c, I2C_IDLE);
                        break;
                }
            }
        }
#ifdef I2C__SLAVE_SUPPORT
        else if (data->state == I2C_SL_RX)
        {
            if (sr1 & I2C_SR1__BTF || ((sr1 & I2C_SR1__ADDR) && (sr1 & I2C_SR1__TXE)))
            {
                // ** EV2 **
                // read the byte into the buffer
                uint8_t byte = *i2c_get_DR(_i2c);
                data->slave_handler(I2C_SLAVE_EV_RX_BYTE, &byte);
            }
        }
#endif
    }

    if (sr1 & I2C_SR1__TXE)
    {
        // The flag is cleared after reading SR1 (already done) and reading or writing DR
        if (data->state == I2C_SENDING_DATA)
        {
            if (data->len_send == data->cpt_send)
            {
                // ** EV8_2 **
                // The last byte has been sent so we need to issue
                // a STOP condition if there is nothing to receive or
                // a RESTART condition otherwise

                if (data->len_recv == 0)
                {
                    // There is nothing to receive
                    *i2c_get_CR1(_i2c) |= I2C_CR1__STOP;

                    // The transfer is complete
                    tx_rx_end(_i2c, I2C_IDLE);
                }
                else
                {
                    // We need to read something
                    *i2c_get_CR1(_i2c) |= I2C_CR1__START;

                    data->state = I2C_SENDING_RESTART;
                }

                // Indicate that there is nothing else to send
                data->len_send = 0;
                data->cpt_send = 0;
            }
            else
            {
                // ** EV8 **
                // Send the data
                *i2c_get_DR(_i2c) = data->buf_send[data->cpt_send];
                data->cpt_send++;

                // Stay in the I2C_SENDING_DATA state
            }
        }
#ifdef I2C__SLAVE_SUPPORT
        else if (data->state == I2C_SL_TX)
        {
            // ** EV3 **
            uint8_t byte;
            // request the byte to send
            data->slave_handler(I2C_SLAVE_EV_TX_BYTE, &byte);
            *i2c_get_DR(_i2c) = byte;
        }
#endif
    }


    if (sr1 & I2C_SR1__STOPF)
    {
        switch (data->state)
        {
#ifdef I2C__SLAVE_SUPPORT
            case I2C_SL_RX:
                if (sr1 & I2C_SR1__RXNE)
                {
                    // it remains exactly 1 byte to read
                    // since we have already read the buffer one (if any)
                    uint8_t byte = *i2c_get_DR(_i2c);
                    if (data->slave_handler)
                    {
                        data->slave_handler(I2C_SLAVE_EV_RX_BYTE, &byte);
                    }
                }
                sr1 = *i2c_get_SR1(_i2c);
                *i2c_get_CR1(_i2c) &= ~I2C_CR1__STOP;
                data->state = I2C_IDLE;
                if (data->slave_handler)
                {
                    data->slave_handler(I2C_SLAVE_EV_STOP, NULL);
                }
                break;
            case I2C_SL_TX:
                log_error("STOPF interrupt!");
                data->state = I2C_IDLE;
                break;
#endif
            default:
                // This should not happen as we are in master mode
                log_error("STOPF interrupt!");
                uint8_t x = *i2c_get_CR1(_i2c);
                *i2c_get_CR1(_i2c) = x;

                // Set stop bit to stop transfer
                *i2c_get_CR1(_i2c) |= I2C_CR1__STOP;

                // Update State
                tx_rx_end(_i2c, I2C_ERROR);
                break;
        }
    }
}

void i2c_handle_er_interrupt(const _i2c_t *_i2c)
{
    uint16_t sr1 = *i2c_get_SR1(_i2c);
    uint16_t sr2 = *i2c_get_SR2(_i2c);
    (void) sr2; // clear defined but not used warning warning

    if (sr1 & I2C_SR1__BERR)
    {
        log_error("Bus Error");
        *i2c_get_SR1(_i2c) &= ~I2C_SR1__BERR;
    }

    if (sr1 & I2C_SR1__ARLO)
    {
        log_error("Arbitration Lost");
        *i2c_get_SR1(_i2c) &= ~I2C_SR1__ARLO;
    }

    if (sr1 & I2C_SR1__AF)
    {
#ifdef I2C__SLAVE_SUPPORT
        if (_i2c->data->state == I2C_SL_TX)
        {
            *i2c_get_SR1(_i2c) &= ~I2C_SR1__AF;
            _i2c->data->state = I2C_IDLE;
            if (_i2c->data->slave_handler)
            {
                _i2c->data->slave_handler(I2C_SLAVE_EV_STOP, NULL);
            }
            return;
        }
#endif
        log_error("Acknowledge Failure");
        *i2c_get_SR1(_i2c) &= ~I2C_SR1__AF;
    }

    if (sr1 & I2C_SR1__OVR)
    {
        log_error("Overrun");
        *i2c_get_SR1(_i2c) &= ~I2C_SR1__OVR;
    }

    if (sr1 & I2C_SR1__PECERR)
    {
        log_error("PEC error");
        *i2c_get_SR1(_i2c) &= ~I2C_SR1__PECERR;
    }

    if (sr1 & I2C_SR1__TIMEOUT)
    {
        log_error("Timeout");
        *i2c_get_SR1(_i2c) &= ~I2C_SR1__TIMEOUT;
    }

    if (sr1 & I2C_SR1__SMBALERT)
    {
        log_error("SMBAlert");
        *i2c_get_SR1(_i2c) &= ~I2C_SR1__SMBALERT;
    }

    log_error("I2C ERROR, CR1: %x, CR2: %x, SR1: %x, SR2: %x, state: %x",
            *i2c_get_CR1(_i2c), *i2c_get_CR1(_i2c), sr1, sr2, _i2c->data->state);

    // Set stop bit to stop transfer
    *i2c_get_CR1(_i2c) |= I2C_CR1__STOP;

    // Update State
    tx_rx_end(_i2c, I2C_ERROR);
}

/*
 * Start a transfer
 */
static void tx_rx_start(const _i2c_t *_i2c, uint8_t addr, const uint8_t *tx_buffer,
                   uint16_t tx_length, uint8_t *rx_buffer, uint16_t rx_length,
                   result_handler_t handler, handler_arg_t arg)
{
    _i2c_data_t *const data = _i2c->data;

    // Do the before-transfer test
    test_ready(_i2c);

    // Save the handler
    data->transfer_handler = handler;
    data->transfer_handler_arg = arg;

    // Copy the data to send, data to read. Set the current state for the
    // I2C state machine and the issue the start condition.
    data->address = addr;

    data->len_send = tx_length;
    data->cpt_send = 0;
    data->buf_send = tx_buffer;

    data->len_recv = rx_length;
    data->cpt_recv = 0;
    data->buf_recv = rx_buffer;

    // Ensure POS bit is cleared
    *i2c_get_CR1(_i2c) &= ~I2C_CR1__POS;

    data->state = I2C_SENDING_START;

    // Set ACK bit before starting anything
    *i2c_get_CR1(_i2c) |= I2C_CR1__ACK;

    // Generate START condition to initiate the transfer
    *i2c_get_CR1(_i2c) |= I2C_CR1__START;
}

/*
 * Wait for the end of a transfer
 */
static uint32_t tx_rx_wait(const _i2c_t *_i2c)
{
    _i2c_data_t *const data = _i2c->data;
    uint32_t result;

    while ((data->state != I2C_IDLE) && (data->state != I2C_ERROR))
    {
    }

    // Wait until busy is cleared
    while (*i2c_get_SR2(_i2c) & I2C_SR2__BUSY)
    {
    }

    // Get result
    result = (data->state != I2C_IDLE);

    data->state = I2C_IDLE;
    return result;
}

/*
 * Signal the transfer is completed
 */
static void tx_rx_end(const _i2c_t *_i2c, i2c_state_t state)
{
    _i2c_data_t *const data = _i2c->data;

    if (data->transfer_handler)
    {
        result_handler_t hdl = data->transfer_handler;
        data->transfer_handler = NULL;
        data->state = I2C_IDLE;
        hdl(data->transfer_handler_arg, (state != I2C_IDLE));
    }
    else
    {
        data->state = state;
    }
}

static void test_ready(const _i2c_t *_i2c)
{
    uint16_t reg;

    // Check State
    if (_i2c->data->state != I2C_IDLE)
    {
        log_error("I2C State not Idle");
    }
    else if (((reg = *i2c_get_CR1(_i2c)) & I2C_CR1__PE) == 0)
    {
        log_error("I2C CR1 not PE: %x", reg);
    }
    else if ((reg = (*i2c_get_CR2(_i2c) & (I2C_CR2__ITERREN | I2C_CR2__ITEVTEN
                                           | I2C_CR2__ITEVTEN))) != (I2C_CR2__ITERREN | I2C_CR2__ITEVTEN
                                                   | I2C_CR2__ITEVTEN))
    {
        log_error("I2C CR2 interrupts not enabled: %x", reg);
    }

    if ((reg = *i2c_get_SR2(_i2c)) & I2C_SR2__BUSY)
    {
        log_error("I2C SR2 Busy not cleared: %x", reg);
    }
}
