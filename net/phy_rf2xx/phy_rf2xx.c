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
 * phy_rf2xx.c
 *
 *  Created on: Jul 11, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "phy_rf2xx.h"
#include "rf2xx.h"

// FreeRTOS include
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

// Global lib
#include "event.h"
#include "printf.h"

// Net lib
#include "lib/net_timer.h"

/* Private Functions */
// Interrupt handlers
static void dig2_capture_handler(handler_arg_t phy, uint16_t timer_value);
static void rx_start_handler(handler_arg_t phy, uint16_t timer_value);
static void rx_timeout_handler(handler_arg_t phy, uint16_t timer_value);
static void tx_start_handler(handler_arg_t phy, uint16_t timer_value);
static void irq_handler(handler_arg_t phy);
static void fifo_read_done_handler(handler_arg_t phy);

// API implementations
static void reset(phy_rf2xx_t *_phy);
static void sleep(phy_rf2xx_t *_phy);
static void idle(phy_rf2xx_t *_phy);

// Functions
static void handle_irq(handler_arg_t arg);
static void handle_rx_start(handler_arg_t arg);
static void handle_rx_end(handler_arg_t arg);
static void handle_rx_timeout(handler_arg_t arg);
static void handle_tx_end(handler_arg_t arg);
static void start_rx(handler_arg_t arg);

void phy_rf2xx_init(phy_rf2xx_t *_phy, rf2xx_t radio, timer_t timer,
                    timer_channel_t channel)
{
    // Store the pointers
    _phy->radio = radio;
    _phy->timer = timer;
    _phy->channel = channel;

    // Initialize the packet pointer
    _phy->pkt = NULL;

    // Do a reset
    reset(_phy);
}

void phy_reset(phy_t phy)
{
    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Do the real reset
    reset(_phy);
}

void phy_sleep(phy_t phy)
{
    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Do the real sleep
    sleep(_phy);
}

void phy_idle(phy_t phy)
{
    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Do the real idle
    idle(_phy);
}
phy_status_t phy_set_channel(phy_t phy, uint8_t channel)
{
    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Check state
    switch(_phy->state)
    {
        case PHY_STATE_SLEEP:
            // Wakeup
            rf2xx_wakeup(_phy->radio);
            break;
        case PHY_STATE_IDLE:
            // Nothing to do
            break;
        default:
            log_error("Invalid state %u", _phy->state);
            // State is invalid for channel setting, return error
            return PHY_ERR_INVALID_STATE;
    }

    // Check min and max value
    if(channel < PHY_MIN_CHANNEL)
    {
        channel = PHY_MIN_CHANNEL;
    }
    else if(channel > PHY_MAX_CHANNEL)
    {
        channel = PHY_MAX_CHANNEL;
    }

    // Write new data to register
    rf2xx_reg_write(_phy->radio, RF2XX_REG__PHY_CC_CCA,
                    RF2XX_PHY_CC_CCA_DEFAULT__CCA_MODE | channel);

    // Go back to sleep if it was in this state
    if(_phy->state == PHY_STATE_SLEEP)
    {
        rf2xx_sleep(_phy->radio);
    }

    return PHY_SUCCESS;
}

phy_status_t phy_set_power(phy_t phy, phy_power_t power)
{
    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Check state
    switch(_phy->state)
    {
        case PHY_STATE_SLEEP:
            // Wakeup
            rf2xx_wakeup(_phy->radio);
            break;
        case PHY_STATE_IDLE:
            // Nothing to do
            break;
        default:
            log_error("Invalid state %u", _phy->state);

            // State is invalid for channel setting, return error
            return PHY_ERR_INVALID_STATE;
    }

    // Convert power value to register value
    switch(power)
    {
        case PHY_POWER_m30dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m17dBm;
            break;
        case PHY_POWER_m20dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m17dBm;
            break;
        case PHY_POWER_m10dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m9dBm;
            break;
        case PHY_POWER_m5dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m5dBm;
            break;
        case PHY_POWER_0dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__0dBm;
            break;
        case PHY_POWER_3dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__3dBm;
            break;
        case PHY_POWER_5dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__3dBm;
            break;
        default:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__0dBm;
    }

    // Write new data to register
    rf2xx_reg_write(_phy->radio, RF2XX_REG__PHY_TX_PWR,
                    RF2XX_PHY_TX_PWR_DEFAULT__PA_BUF_LT
                    | RF2XX_PHY_TX_PWR_DEFAULT__PA_LT | power);

    // Go back to sleep if it was in this state
    if(_phy->state == PHY_STATE_SLEEP)
    {
        rf2xx_sleep(_phy->radio);
    }

    return PHY_SUCCESS;
}

uint8_t phy_cca(phy_t phy)
{
    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Check state
    switch(_phy->state)
    {
        case PHY_STATE_SLEEP:
            // Wakeup
            rf2xx_wakeup(_phy->radio);
            break;
        case PHY_STATE_IDLE:
            // Nothing to do
            break;
        default:
            // Invalid state!
            log_error("Invalid state %u", _phy->state);
            return 0;
    }

    // Disable interrupt
    rf2xx_irq_disable(_phy->radio);

    // Start RX
    rf2xx_set_state(_phy->radio, RF2XX_TRX_STATE__RX_ON);

    uint8_t reg;

    // Wait until RX is entered
    do
    {
        reg = rf2xx_get_status(_phy->radio);
    }
    while((reg & RF2XX_TRX_STATUS__RX_ON) == 0);

    // Request a CCA
    reg = rf2xx_reg_read(_phy->radio, RF2XX_REG__PHY_CC_CCA);
    reg |= RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST;
    rf2xx_reg_write(_phy->radio, RF2XX_REG__PHY_CC_CCA, reg);

    // Wait until CCA is performed
    do
    {
        reg = rf2xx_reg_read(_phy->radio, RF2XX_REG__TRX_STATUS);
    }
    while((reg & RF2XX_TRX_STATUS_MASK__CCA_DONE) == 0);

    // Stop RX
    rf2xx_set_state(_phy->radio, RF2XX_TRX_STATE__FORCE_TRX_OFF);

    // Go back to sleep if needed
    if(_phy->state == PHY_STATE_SLEEP)
    {
        // Sleep
        rf2xx_sleep(_phy->radio);
    }

    // Return CCA STATUS
    return !!(reg & RF2XX_TRX_STATUS_MASK__CCA_STATUS);
}
phy_status_t phy_rx(phy_t phy, uint32_t rx_time, uint32_t timeout_time,
                    phy_packet_t *pkt, phy_handler_t handler)
{
    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Check the provided packet
    if(pkt == NULL)
    {
        log_error("Invalid provided RX packet: NULL");
        HALT();
    }

    // Check state
    switch(_phy->state)
    {
        case PHY_STATE_SLEEP:
            // Wakeup
            rf2xx_wakeup(_phy->radio);
            break;
        case PHY_STATE_IDLE:
            // Nothing to do
            break;
        default:
            // Invalid state!
            log_error("Invalid state %u", _phy->state);
            return PHY_ERR_INVALID_STATE;
    }

    // Store timeout time
    _phy->rx_timeout = timeout_time;

    // Store packet pointer and handler
    _phy->pkt = pkt;
    _phy->handler = handler;

    // Clear timestamp
    _phy->pkt->timestamp = 0;
    _phy->pkt->t_rx_start = 0;
    _phy->pkt->t_rx_end = 0;

    // Store State
    _phy->state = PHY_STATE_RX_WAIT;

    // compute delta
    uint16_t now = timer_time(_phy->timer);
    int16_t delta = ((uint16_t) rx_time) - now;

    if( // Check if an RX time is specified
        rx_time &&
        // Check start time has not passed
        (delta > 1))
    {
        // Set RX start time
        timer_set_channel_compare(_phy->timer, _phy->channel, rx_time & 0xFFFF,
                                  (timer_handler_t) rx_start_handler, _phy);
    }
    else
    {
        // Set RX now
        start_rx(_phy);
    }

    return PHY_SUCCESS;
}
phy_status_t phy_tx(phy_t phy, uint32_t tx_time, phy_packet_t *pkt,
                    phy_handler_t handler)
{
    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Check the provided packet
    if(pkt == NULL)
    {
        log_error("Invalid provided TX packet: NULL");
        HALT();
    }

    // Check length is valid
    if(pkt->length > PHY_MAX_TX_LENGTH)
    {
        log_error("length too big: %u", pkt->length);
        return PHY_ERR_INVALID_LENGTH;
    }

    // Check state
    switch(_phy->state)
    {
        case PHY_STATE_SLEEP:
            // Wakeup
            rf2xx_wakeup(_phy->radio);
            break;
        case PHY_STATE_IDLE:
            // Nothing to do
            break;
        default:
            log_error("Invalid state %u", _phy->state);
            return PHY_ERR_INVALID_STATE;
    }

    // Store State
    _phy->state = PHY_STATE_TX;

    // Store packet
    _phy->pkt = pkt;

    // Store the handler
    _phy->handler = handler;

    // Disable interrupt
    rf2xx_irq_disable(_phy->radio);

    // Set IRQ to refect TRX END (end of TX) only
    rf2xx_reg_write(_phy->radio, RF2XX_REG__IRQ_MASK,
                    RF2XX_IRQ_STATUS_MASK__TRX_END);

    // Read IRQ to clear it
    rf2xx_reg_read(_phy->radio, RF2XX_REG__IRQ_STATUS);

    // Enable IRQ interrupt
    rf2xx_irq_enable(_phy->radio);

    // Change to PLL ON
    rf2xx_set_state(_phy->radio, RF2XX_TRX_STATE__PLL_ON);

    uint32_t launch_now = 0;
    int16_t spare_time = 0;

    // Check if TX time is delayed
    if(tx_time)
    {
        // Update TX alarm time
        tx_time -= PHY_TIMING__TX_OFFSET;

        // Check if Time to start is not elapsed
        spare_time = (int16_t)((tx_time & 0xFFFF) - timer_time(_phy->timer));

        if(spare_time > 1)
        {
            // Set timer
            timer_set_channel_compare(_phy->timer, _phy->channel, tx_time
                                      & 0xFFFF, (timer_handler_t) tx_start_handler, _phy);
        }
        else
        {
            // TX time has elapsed, start ASAP.
            launch_now = 1;
        }
    }
    else
    {
        // No TX time, start ASAP
        launch_now = 1;
    }

    // Wait until PLL ON state
    uint8_t status;

    do
    {
        status = rf2xx_get_status(_phy->radio);
    }
    while(status != RF2XX_TRX_STATUS__PLL_ON);

    // Copy the packet to the radio FIFO
    rf2xx_fifo_write_first(_phy->radio, _phy->pkt->length + 2);
    rf2xx_fifo_write_remaining_async(_phy->radio, _phy->pkt->data,
                                     _phy->pkt->length, NULL, NULL);

    // Launch now if required
    if(launch_now)
    {
        tx_start_handler(_phy, timer_time(_phy->timer));
        //        printf("(NOW! %d)", spare_time);
    }
    else
    {
        //        printf("(%d spare)", spare_time);
    }

    // Return Success
    return PHY_SUCCESS;
}

/* Private Functions, the lock must be taken before calling them */

static void reset(phy_rf2xx_t *_phy)
{
    // Stop timer
    timer_set_channel_compare(_phy->timer, _phy->channel, 0, NULL, NULL);

    // Configure the radio interrupts
    rf2xx_irq_disable(_phy->radio);
    rf2xx_irq_configure(_phy->radio, irq_handler, _phy);

    // Configure DIG2 pin
    rf2xx_dig2_disable(_phy->radio);
    rf2xx_dig2_configure(_phy->radio, dig2_capture_handler, _phy);

    // Reset the radio chip
    rf2xx_reset(_phy->radio);

    // Set default register values
    uint8_t reg;

    // Set DIG2 as output for RX_START
    reg = rf2xx_reg_read(_phy->radio, RF2XX_REG__TRX_CTRL_1);
    reg |= RF2XX_TRX_CTRL_1_MASK__IRQ_2_EXT_EN;
    rf2xx_reg_write(_phy->radio, RF2XX_REG__TRX_CTRL_1, reg);

    // Enable Dynamic Frame Buffer Protection
    reg = RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE;
    rf2xx_reg_write(_phy->radio, RF2XX_REG__TRX_CTRL_2, reg);

    // Set stored TX power
    reg = RF2XX_PHY_TX_PWR_DEFAULT__PA_BUF_LT | RF2XX_PHY_TX_PWR_DEFAULT__PA_LT
          | PHY_POWER_0dBm;
    rf2xx_reg_write(_phy->radio, RF2XX_REG__PHY_TX_PWR, reg);

    // Disable CLKM signal
    reg = RF2XX_TRX_CTRL_0_DEFAULT__PAD_IO
          | RF2XX_TRX_CTRL_0_DEFAULT__PAD_IO_CLKM
          | RF2XX_TRX_CTRL_0_DEFAULT__CLKM_SHA_SEL
          | RF2XX_TRX_CTRL_0_CLKM_CTRL__OFF;
    rf2xx_reg_write(_phy->radio, RF2XX_REG__TRX_CTRL_0, reg);

    // Set XCLK TRIM TODO: this highly depends on the board
    reg = RF2XX_XOSC_CTRL__XTAL_MODE_CRYSTAL | 0x0;
    rf2xx_reg_write(_phy->radio, RF2XX_REG__XOSC_CTRL, reg);

    // Set Power Down state
    sleep(_phy);
}

static void sleep(phy_rf2xx_t *_phy)
{
    // Set Idle
    idle(_phy);

    // Power off the radio
    rf2xx_sleep(_phy->radio);

    // Save state
    _phy->state = PHY_STATE_SLEEP;
}

static void idle(phy_rf2xx_t *_phy)
{
    // Disable the interrupts
    rf2xx_irq_disable(_phy->radio);

    // Stop timer
    timer_set_channel_compare(_phy->timer, _phy->channel, 0, NULL, NULL);

    // Check state
    switch(_phy->state)
    {
        case PHY_STATE_SLEEP:
            // Wakeup
            rf2xx_wakeup(_phy->radio);
            break;
        default:
            // Nothing to do
            break;
    }

    // Force IDLE
    rf2xx_set_state(_phy->radio, RF2XX_TRX_STATE__FORCE_TRX_OFF);

    // Clear pkt pointer
    _phy->pkt = NULL;

    // Save state
    _phy->state = PHY_STATE_IDLE;
}
static void start_rx(handler_arg_t arg)
{
    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = arg;

    // Check state and pkt
    if(_phy->state != PHY_STATE_RX_WAIT)
    {
        log_error("start_rx but state not RX wait: %x", _phy->state);
        HALT();
    }

    if(_phy->pkt == NULL)
    {
        log_error("start_rx but pkt NULL");
        HALT();
    }

    // Set State
    _phy->state = PHY_STATE_RX;

    // Disable interrupt
    rf2xx_irq_disable(_phy->radio);

    // Restart RX: force TRX_OFF
    rf2xx_set_state(_phy->radio, RF2XX_TRX_STATE__FORCE_TRX_OFF);

    // Reset IRQ to TRX END only
    rf2xx_reg_write(_phy->radio, RF2XX_REG__IRQ_MASK,
                    RF2XX_IRQ_STATUS_MASK__TRX_END);

    // Read IRQ to clear it
    rf2xx_reg_read(_phy->radio, RF2XX_REG__IRQ_STATUS);

    // Enable IRQ interrupt
    rf2xx_irq_enable(_phy->radio);

    // Enable DIG2 time-stamping
    rf2xx_dig2_enable(_phy->radio);

    // Start RX
    rf2xx_set_state(_phy->radio, RF2XX_TRX_STATE__RX_ON);

    // Loop until RX_ON is entered
    uint8_t status;

    do
    {
        status = rf2xx_get_status(_phy->radio);
    }
    while((status & RF2XX_TRX_STATUS_MASK__TRX_STATUS)
            != RF2XX_TRX_STATUS__RX_ON);

    // Set timer for timeout, if any
    if(_phy->rx_timeout)
    {
        timer_set_channel_compare(_phy->timer, _phy->channel, _phy->rx_timeout
                                  & 0xFFFF, (timer_handler_t) rx_timeout_handler, _phy);
    }
    else
    {
        // Disable timer
        timer_set_channel_compare(_phy->timer, _phy->channel, 0, NULL, NULL);
    }
}

static void handle_rx_start(handler_arg_t arg)
{
    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = arg;

    // Check state and pkt
    if(_phy->state != PHY_STATE_RX)
    {
        log_error("handle_rx_start but state not RX: %x", _phy->state);
        HALT();
    }

    if(_phy->pkt == NULL)
    {
        log_error("handle_rx_start but pkt NULL");
        HALT();
    }

    // Force IDLE
    rf2xx_set_state(_phy->radio, RF2XX_TRX_STATE__FORCE_TRX_OFF);

    // Check the CRC is good
    if(!(rf2xx_reg_read(_phy->radio, RF2XX_REG__PHY_RSSI)
            & RF2XX_PHY_RSSI_MASK__RX_CRC_VALID))
    {
        // Error CRC, call handler
        log_warning("RX CRC error");

        // Force Idle
        idle(_phy);

        // Call handler
        if(_phy->handler)
        {
            _phy->handler(PHY_RX_CRC_ERROR);
        }

        return;
    }

    // Read length byte (first byte)
    _phy->pkt->length = rf2xx_fifo_read_first(_phy->radio);

    // Check valid length
    if((_phy->pkt->length == 0) || (_phy->pkt->length > PHY_MAX_RX_LENGTH))
    {
        // Error length, end transfer
        rf2xx_fifo_read_remaining(_phy->radio, _phy->pkt->data, 0);

        // Force Idle
        idle(_phy);

        // Call handler
        if(_phy->handler)
        {
            _phy->handler(PHY_RX_LENGTH_ERROR);
        }

        return;
    }

    // Store RX start time
    _phy->pkt->t_rx_start = net_timer_time();

    // Retrieve remaining of the data asynchronously (+1) to have the LQI
    uint8_t length = _phy->pkt->length + 1;

    rf2xx_fifo_read_remaining_async(_phy->radio, _phy->pkt->data, length,
                                    fifo_read_done_handler, _phy);
}

static void handle_rx_end(handler_arg_t arg)
{
    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = arg;

    // Check state and pkt
    if(_phy->state != PHY_STATE_RX)
    {
        log_error("handle_rx_end but state not RX: %x", _phy->state);
        HALT();
    }

    if(_phy->pkt == NULL)
    {
        log_error("handle_rx_end but pkt NULL");
        HALT();
    }

    // Store RX END time
    _phy->pkt->t_rx_end = net_timer_time();

    // Read the ED value (~RSSI)
    _phy->pkt->rssi = -91
                      + rf2xx_reg_read(_phy->radio, RF2XX_REG__PHY_ED_LEVEL);

    // Read the LQI (last byte read from the framebuffer)
    _phy->pkt->lqi = _phy->pkt->data[_phy->pkt->length];

    // Remove status bytes from length
    _phy->pkt->length -= 2;

    if(_phy->pkt->t_rx_end - _phy->pkt->t_rx_start > 500)
    {
        log_error("Too much time to read a packet, length = %u",
                  _phy->pkt->length + 1);
        log_info("rx_end : %u", _phy->pkt->t_rx_end - _phy->pkt->t_rx_start);
        HALT();
    }

    // Go to idle
    idle(_phy);

    // Call RX handler if any
    if(_phy->handler)
    {
        _phy->handler(PHY_SUCCESS);
    }
}
static void handle_rx_timeout(handler_arg_t arg)
{
    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = arg;

    // Handle Timeout should only occur in RX state
    if((_phy->state != PHY_STATE_RX_WAIT) && (_phy->state != PHY_STATE_RX))
    {
        log_error("invalid state for phy handle_rx_timeout: %x", _phy->state);
        HALT();
        return;
    }

    // Set Idle
    idle(_phy);

    // Notify receiving failed
    if(_phy->handler)
    {
        _phy->handler(PHY_RX_TIMEOUT_ERROR);
    }
}

static void handle_tx_end(handler_arg_t arg)
{
    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = arg;

    // Go to Idle
    idle(_phy);

    // Notify sending is done if handler is not null
    if(_phy->handler)
    {
        _phy->handler(PHY_SUCCESS);
    }
}

static void handle_irq(handler_arg_t arg)
{
    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = arg;

    // Get the IRQ status
    uint8_t irq_status = rf2xx_reg_read(_phy->radio, RF2XX_REG__IRQ_STATUS);

    // Switch on the state
    switch(_phy->state)
    {
        case PHY_STATE_RX:

            // Check if TRX_END happened
            if(irq_status == RF2XX_IRQ_STATUS_MASK__TRX_END)
            {
                // Start processing
                handle_rx_start(_phy);
            }
            else
            {
                // Error
                log_error("invalid handle_irq %x in RX state (radio state %x)",
                          irq_status, rf2xx_get_status(_phy->radio));
            }

            break;
        case PHY_STATE_TX:

            // Check if TRX_END happened
            if(irq_status == RF2XX_IRQ_STATUS_MASK__TRX_END)
            {
                // Start processing
                handle_tx_end(_phy);
            }
            else
            {
                // Error
                log_error("invalid handle_irq %x in TX state (radio state %x)",
                          irq_status, rf2xx_get_status(_phy->radio));
            }

            break;
        default:
            // Weird state
            log_error("handle_irq %x in state %x (radio state %x)", irq_status,
                      _phy->state, rf2xx_get_status(_phy->radio));
            return;
    }
}

/* Those are called from interrupt service routines */
static void dig2_capture_handler(handler_arg_t arg, uint16_t timer_value)
{
    // Cast to PHY
    phy_rf2xx_t *_phy = arg;

    // Check state
    if(_phy->state != PHY_STATE_RX)
    {
        log_error("DIG2 but state not RX");
        HALT();
    }

    // Stop RX timeout alarm
    timer_set_channel_compare(_phy->timer, _phy->channel, 0, NULL, NULL);

    // Store timer value, if rx packet specified
    if(_phy->pkt)
    {
        _phy->pkt->timestamp = net_timer_convert_time(timer_value);
    }
}
static void rx_start_handler(handler_arg_t arg, uint16_t timer_value)
{
    // Request a post of start_rx
    event_post_from_isr(EVENT_QUEUE_NETWORK_HIGH, start_rx, arg);
}

static void tx_start_handler(handler_arg_t arg, uint16_t timer_value)
{
    // Cast to PHY
    phy_rf2xx_t *_phy = arg;

    // Start TX by setting SLP_TR
    rf2xx_slp_tr_set(_phy->radio);

    // Store time
    _phy->pkt->timestamp = net_timer_time() + PHY_TIMING__TX_OFFSET;

    // Store State
    _phy->state = PHY_STATE_TX;

    // Disable timer
    timer_set_channel_compare(_phy->timer, _phy->channel, 0, NULL, NULL);

    // Clear SLP_TR
    rf2xx_slp_tr_clear(_phy->radio);
}

static void rx_timeout_handler(handler_arg_t arg, uint16_t timer_value)
{
    // Cast to PHY
    phy_rf2xx_t *_phy = arg;

    // Disable timer
    timer_set_channel_compare(_phy->timer, _phy->channel, 0, NULL, NULL);

    // Call handle_rx_timeout handler from event task
    event_post_from_isr(EVENT_QUEUE_NETWORK_HIGH, handle_rx_timeout, arg);
}

static void irq_handler(handler_arg_t arg)
{
    // Cast to PHY
    phy_rf2xx_t *_phy = arg;

    // Store IRQ time
    _phy->pkt->eop_time = net_timer_time();

    // Call IRQ handler from event task
    event_post_from_isr(EVENT_QUEUE_NETWORK_HIGH, handle_irq, arg);
}

static void fifo_read_done_handler(handler_arg_t arg)
{
    // Call RX end handler from event task
    event_post_from_isr(EVENT_QUEUE_NETWORK_HIGH, handle_rx_end, arg);
}
