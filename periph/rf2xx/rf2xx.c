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
 * rf2xx.c
 *
 *  Created on: Jul 8, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "rf2xx.h"
#include "rf2xx/rf2xx_.h"
#include "spi.h"
#include "gpio.h"
#include "timer.h"
#include "soft_timer.h"
#include "exti.h"

#include "soft_timer.h"
#include "printf.h"
#include "debug.h"

#define RF_MAX_WAIT soft_timer_ms_to_ticks(1)

/* Handy functions */
inline static void csn_set(_rf2xx_t *_radio)
{
    gpio_pin_set(_radio->config->csn_gpio, _radio->config->csn_pin);
}
inline static void csn_clear(_rf2xx_t *_radio)
{
    gpio_pin_clear(_radio->config->csn_gpio, _radio->config->csn_pin);
}
inline static void rstn_set(_rf2xx_t *_radio)
{
    gpio_pin_set(_radio->config->rstn_gpio, _radio->config->rstn_pin);
}
inline static void rstn_clear(_rf2xx_t *_radio)
{
    gpio_pin_clear(_radio->config->rstn_gpio, _radio->config->rstn_pin);
}
inline static void slp_tr_set(_rf2xx_t *_radio)
{
    gpio_pin_set(_radio->config->slp_tr_gpio, _radio->config->slp_tr_pin);
    gpio_set_output(_radio->config->slp_tr_gpio, _radio->config->slp_tr_pin);
}
inline static void slp_tr_clear(_rf2xx_t *_radio)
{
    gpio_pin_clear(_radio->config->slp_tr_gpio, _radio->config->slp_tr_pin);
    gpio_set_output(_radio->config->slp_tr_gpio, _radio->config->slp_tr_pin);
}
inline static void pa_enable_set(_rf2xx_t *_radio)
{
    gpio_pin_set(_radio->config->pa_enable_gpio, _radio->config->pa_enable_pin);
}
inline static void pa_enable_clear(_rf2xx_t *_radio)
{
    gpio_pin_clear(_radio->config->pa_enable_gpio, _radio->config->pa_enable_pin);
}
/* Handler for SPI transfer */
static void transfer_done(handler_arg_t arg);

/* Initialization and reset */
void rf2xx_init(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Initialize the IOs
    // Set output/input
    gpio_set_output(_radio->config->csn_gpio, _radio->config->csn_pin);
    gpio_set_output(_radio->config->rstn_gpio, _radio->config->rstn_pin);
    gpio_set_output(_radio->config->slp_tr_gpio, _radio->config->slp_tr_pin);

    // Set RSTn low, CSn high and SLP_TR low
    rstn_clear(_radio);
    csn_set(_radio);
    slp_tr_clear(_radio);

    // Configure PA if any
    if (_radio->config->pa_enable_gpio != NULL)
    {
        gpio_set_output(_radio->config->pa_enable_gpio, _radio->config->pa_enable_pin);
        pa_enable_clear(_radio);
    }

    // Wait a few micro-seconds
    soft_timer_delay_us(RF2XX_TIMING__VCC_TO_P_ON);

    // Perform a reset
    rf2xx_reset(_radio);

    // Then go to sleep
    rf2xx_sleep(_radio);
}

void rf2xx_reset(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Force reset, default pin values
    rstn_clear(_radio);
    csn_set(_radio);
    slp_tr_clear(_radio);

    // Wait for the required time
    soft_timer_delay_us(RF2XX_TIMING__RESET);

    // Release reset pin
    rstn_set(_radio);

    // Wait until TRX_OFF is entered
    soft_timer_delay_us(RF2XX_TIMING__RESET_TO_TRX_OFF);

    // Send a FORCE TRX OFF command
    rf2xx_reg_write(radio, RF2XX_REG__TRX_STATE, RF2XX_TRX_STATE__FORCE_TRX_OFF);

    // Wait until TRX_OFF state is entered from P_ON
    soft_timer_delay_us(RF2XX_TIMING__SLEEP_TO_TRX_OFF);

    // Loop while not TRX_OFF
    uint8_t status;
    uint32_t t_end = soft_timer_time() + RF_MAX_WAIT;

    do
    {
        status = rf2xx_get_status(radio);

        // Check for block
        if (!soft_timer_a_is_before_b(soft_timer_time(), t_end))
        {
            log_error("Reset not ready");
            break;
        }
    }
    while ((status & RF2XX_TRX_STATUS_MASK__TRX_STATUS)
            != RF2XX_TRX_STATUS__TRX_OFF);
}

rf2xx_type_t rf2xx_get_type(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;
    return _radio->config->type;
}

/* Register, FIFO, SRAM access */
uint8_t rf2xx_reg_read(rf2xx_t radio, uint8_t addr)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;
    uint8_t value;

    // Start SPI transfer
    csn_clear(_radio);

    // Send Register Read access and address
    spi_transfer_single(_radio->config->spi, RF2XX_ACCESS_REG | RF2XX_ACCESS_READ
                        | addr);

    // Receive value
    value = spi_transfer_single(_radio->config->spi, 0x0);

    // End SPI transfer
    csn_set(_radio);

    // Return the register value, in the second case
    return value;
}
void rf2xx_reg_write(rf2xx_t radio, uint8_t addr, uint8_t value)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Start the SPI transfer
    csn_clear(_radio);

    // Send first byte being the command and address
    spi_transfer_single(_radio->config->spi, RF2XX_ACCESS_REG | RF2XX_ACCESS_WRITE
                        | addr);

    // Send value
    spi_transfer_single(_radio->config->spi, value);

    // End the SPI transfer
    csn_set(_radio);
}

void rf2xx_fifo_read(rf2xx_t radio, uint8_t *buffer, uint16_t length)
{
    // If nothing to read, read nothing
    if (!length)
    {
        return;
    }

    // Read first
    buffer[0] = rf2xx_fifo_read_first(radio);

    // Then read the remaining
    rf2xx_fifo_read_remaining(radio, buffer + 1, length - 1);
}

void rf2xx_fifo_read_async(rf2xx_t radio, uint8_t *buffer, uint16_t length,
                           handler_t handler, handler_arg_t arg)
{
    // If nothing to read, read nothing
    if (!length)
    {
        return;
    }

    // Read the first
    buffer[0] = rf2xx_fifo_read_first(radio);

    // Read remaining
    rf2xx_fifo_read_remaining_async(radio, buffer + 1, length - 1, handler, arg);
}

uint8_t rf2xx_fifo_read_first(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Start the SPI transfer
    csn_clear(_radio);

    // Send the command byte
    spi_transfer_single(_radio->config->spi, RF2XX_ACCESS_FRAMEBUFFER
                        | RF2XX_ACCESS_READ);

    // Read the first FIFO byte
    uint8_t first = spi_transfer_single(_radio->config->spi, 0);

    // Do not stop the SPI transfer (but the next function MUST be called after)
    return first;
}
void rf2xx_fifo_read_remaining_async(rf2xx_t radio, uint8_t *buffer,
                                     uint16_t length, handler_t handler, handler_arg_t arg)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Store the handler
    _radio->transfer_handler = handler;
    _radio->transfer_arg = arg;

    // SPI transfer already started

    // If length not null, start the transfer
    if (length)
    {
        // Receive the remaining of the data asynchronously
        spi_transfer_async(_radio->config->spi, 0x0, buffer, length, transfer_done,
                           radio);
    }
    else
    {
        // Make as if the transfer is finished
        transfer_done(radio);
    }
}
void rf2xx_fifo_read_remaining(rf2xx_t radio, uint8_t *buffer, uint16_t length)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // SPI transfer already started

    // Receive the remaining of the data synchronously (no SPI handler)
    spi_transfer(_radio->config->spi, 0x0, buffer, length);

    // End the SPI transfer
    csn_set(_radio);
}

void rf2xx_fifo_write(rf2xx_t radio, const uint8_t *buffer, uint16_t length)
{
    if (!length)
    {
        // If nothing to send, send nothing
        return;
    }

    // Write the first
    rf2xx_fifo_write_first(radio, buffer[0]);

    // Then write the remaining
    rf2xx_fifo_write_remaining(radio, buffer + 1, length - 1);
}

void rf2xx_fifo_write_async(rf2xx_t radio, const uint8_t *buffer,
                            uint16_t length, handler_t handler, handler_arg_t arg)
{
    if (!length)
    {
        // If nothing to send, send nothing, but call handlers
        handler(arg);
        return;
    }

    // Write the first
    rf2xx_fifo_write_first(radio, buffer[0]);

    // Write the remaining
    rf2xx_fifo_write_remaining_async(radio, buffer + 1, length - 1, handler,
                                     arg);
}

void rf2xx_fifo_write_first(rf2xx_t radio, uint8_t first)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Start the SPI transfer
    csn_clear(_radio);

    // Send Frame Buffer Write access
    spi_transfer_single(_radio->config->spi, RF2XX_ACCESS_FRAMEBUFFER
                        | RF2XX_ACCESS_WRITE);

    // Transfer the first byte
    spi_transfer_single(_radio->config->spi, first);

    // Do not end the SPI transfer, as a *_remaining_* call must be made after
}

void rf2xx_fifo_write_remaining(rf2xx_t radio, const uint8_t *buffer,
                                uint16_t length)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Transfer the remaining synchronously
    spi_transfer(_radio->config->spi, buffer, 0x0, length);

    // End the SPI transfer
    csn_set(_radio);
}

void rf2xx_fifo_write_remaining_async(rf2xx_t radio, const uint8_t *buffer,
                                      uint16_t length, handler_t handler, handler_arg_t arg)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Store the handler
    _radio->transfer_handler = handler;
    _radio->transfer_arg = arg;

    // Send the remaining of the data asynchronously
    spi_transfer_async(_radio->config->spi, buffer, 0x0, length, transfer_done, radio);

}

void rf2xx_fifo_access_cancel(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Cancer any asynchronous transfer
    spi_async_cancel(_radio->config->spi);

    // End the SPI transfer
    csn_set(_radio);

    // Clear the handler
    _radio->transfer_handler = NULL;
    _radio->transfer_arg = NULL;
}

void rf2xx_sram_read(rf2xx_t radio, uint8_t addr, uint8_t *buffer,
                     uint16_t length)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Start the SPI transfer
    csn_clear(_radio);

    // Send SRAM Read access
    spi_transfer_single(_radio->config->spi, RF2XX_ACCESS_SRAM | RF2XX_ACCESS_READ);

    // Send SRAM Address
    spi_transfer_single(_radio->config->spi, addr);

    // Receive the remaining of the data synchronously
    spi_transfer(_radio->config->spi, 0x0, buffer, length);

    // End the SPI transfer
    csn_set(_radio);
}
void rf2xx_sram_write(rf2xx_t radio, uint8_t addr, const uint8_t *buffer,
                      uint16_t length)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Start the SPI transfer
    csn_clear(_radio);

    // Send the first byte (command)
    spi_transfer_single(_radio->config->spi, RF2XX_ACCESS_SRAM | RF2XX_ACCESS_WRITE);

    // Send SRAM Address
    spi_transfer_single(_radio->config->spi, addr);

    // Send the remaining of the data synchronously
    spi_transfer(_radio->config->spi, buffer, 0x0, length);

    // End the SPI transfer
    csn_set(_radio);
}

/* Handy Functions */
void rf2xx_sleep(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Clear SLP_TR
    rf2xx_slp_tr_clear(_radio);

    // Force a TRX OFF state
    uint8_t status;
    uint32_t t_end = soft_timer_time() + RF_MAX_WAIT;

    do
    {
        rf2xx_set_state(radio, RF2XX_TRX_STATE__FORCE_TRX_OFF);
        status = rf2xx_get_status(radio);

        // Check for block
        if (!soft_timer_a_is_before_b(soft_timer_time(), t_end))
        {
            log_error("Sleep IDLE not ready");
            break;
        }
    }
    while ((status & RF2XX_TRX_STATUS_MASK__TRX_STATUS)
            != RF2XX_TRX_STATUS__TRX_OFF);

    // Set SLP_TR to enter SLEEP
    rf2xx_slp_tr_set(_radio);
}
void rf2xx_wakeup(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;
    uint8_t status;

    // Clear SLP_TR to enter TRX_OFF
    rf2xx_slp_tr_clear(_radio);

    // Wait until TRX OFF state is reached
    do
    {
        rf2xx_set_state(radio, RF2XX_TRX_STATE__FORCE_TRX_OFF);
        status = rf2xx_get_status(radio) & RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    }
    while (status != RF2XX_TRX_STATUS__TRX_OFF);
}

/* IRQ/DIG2 configuration */
void rf2xx_irq_configure(rf2xx_t radio, handler_t handler, handler_arg_t arg)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Store handler in EXTI
    exti_set_handler(_radio->config->irq_exti_line, handler, arg);
}
void rf2xx_irq_enable(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Enable EXTI interrupt
    exti_enable_interrupt_line(_radio->config->irq_exti_line, EXTI_TRIGGER_RISING);
}
void rf2xx_irq_disable(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Disable EXTI interrupt
    exti_disable_interrupt_line(_radio->config->irq_exti_line);
}

int32_t rf2xx_has_dig2(rf2xx_t radio)
{
    return ((_rf2xx_t*) radio)->config->dig2_timer != NULL;
}

void rf2xx_dig2_configure(rf2xx_t radio, timer_handler_t handler,
                          handler_arg_t arg)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Store handler
    _radio->dig2_handler = handler;
    _radio->dig2_arg = arg;
}
void rf2xx_dig2_enable(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    if (_radio->config->dig2_timer)
    {
        // Start capture in timer
        timer_set_channel_capture(_radio->config->dig2_timer, _radio->config->dig2_channel,
                                  TIMER_CAPTURE_EDGE_RISING, _radio->dig2_handler, _radio->dig2_arg);
    }
}
void rf2xx_dig2_disable(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    if (_radio->config->dig2_timer)
    {
            // Disable capture
        timer_set_channel_capture(_radio->config->dig2_timer, _radio->config->dig2_channel,
                                  TIMER_CAPTURE_EDGE_RISING, NULL, NULL);
    }
}

/* DIG3/4 for external PA */
int32_t rf2xx_has_pa(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;
    return _radio->config->pa_enable_gpio != NULL;

}
void rf2xx_pa_enable(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    if (_radio->config->pa_enable_gpio)
    {
        // Set the enable PIN
        pa_enable_set(_radio);
    }
}
void rf2xx_pa_disable(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    if (_radio->config->pa_enable_gpio)
    {
        // Clear the enable PIN
        pa_enable_clear(_radio);
    }
}

/* SLP_TR command */
void rf2xx_slp_tr_set(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Set SLP_TR
    slp_tr_set(_radio);
}
void rf2xx_slp_tr_clear(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Clear SLP_TR
    slp_tr_clear(_radio);
}

void rf2xx_slp_tr_config_timer(rf2xx_t radio)
{
    // Cast to _rf2xx_t
    _rf2xx_t *_radio = radio;

    // Set timer
    gpio_set_timer_output(_radio->config->slp_tr_gpio, _radio->config->slp_tr_pin, _radio->config->slp_tr_af);
}
/* Private Functions */
static void transfer_done(handler_arg_t caller)
{
    _rf2xx_t *_radio = (_rf2xx_t *) caller;

    // End the SPI transfer
    csn_set(_radio);

    // Call the handler if any
    if (_radio->transfer_handler)
    {
        _radio->transfer_handler(_radio->transfer_arg);
    }
}

