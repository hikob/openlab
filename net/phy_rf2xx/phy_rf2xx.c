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
 * phy_rf2xx.c
 *
 *  Created on: Jul 11, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include <stdlib.h>

#include "platform.h"

#include "phy_rf2xx.h"
#include "rf2xx.h"

// Global lib
#include "event.h"
#include "soft_timer_delay.h"

#include "printf.h"
#include "debug.h"

/* Private Functions */
/** Convert Power from PHY power to RF231 power */
static int convert_power(phy_power_t power);

// Interrupt handlers
static void dig2_capture_handler(handler_arg_t arg, uint16_t timer_value);
static void rx_start_handler(handler_arg_t arg, uint16_t timer_value);
static void rx_timeout_handler(handler_arg_t arg, uint16_t timer_value);
static void tx_start_handler(handler_arg_t arg, uint16_t timer_value);
static void irq_handler(handler_arg_t arg);
static void fifo_read_done_handler(handler_arg_t arg);

// API implementations (mutex must be taken)
static void reset(phy_rf2xx_t *_phy);
static void sleep(phy_rf2xx_t *_phy);
static void idle(phy_rf2xx_t *_phy);

// Functions posted (must include protection)
static void handle_irq(handler_arg_t arg);
static void handle_rx_end(handler_arg_t arg);
static void handle_rx_timeout(handler_arg_t arg);
static void start_rx(handler_arg_t arg);

// Handy function (mutex must be taken)
static phy_status_t handle_rx_start(phy_rf2xx_t *_phy);

#define RF_MAX_WAIT soft_timer_ms_to_ticks(1)

#if !defined(PLATFORM_OS) || (PLATFORM_OS == FREERTOS)
#include "FreeRTOS.h"
#include "semphr.h"
static xSemaphoreHandle mutex = NULL;
static inline void seminit() 
{
    if (mutex == NULL)
    {
        mutex = xSemaphoreCreateMutex();
    }
}
static inline void take()
{
    xSemaphoreTake(mutex, configTICK_RATE_HZ);
}
static inline void give()
{
    xSemaphoreGive(mutex);
}
#else
static inline void seminit() {}
static inline void take()    {}
static inline void give()    {}
#endif

// ******************** API methods ************************** //

void phy_rf2xx_init(phy_rf2xx_t *_phy, rf2xx_t radio, openlab_timer_t timer,
        timer_channel_t channel)
{
    // Create mutex if required
    seminit();
    take();

    // Store the pointers
    _phy->radio = radio;
    _phy->timer = timer;
    _phy->channel = channel;

    // Initialize the packet pointer
    _phy->pkt = NULL;

    // Do a reset
    reset(_phy);

    give();
}

void phy_reset(phy_t phy)
{
    take();

    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Do the real reset
    reset(_phy);

    give();
}

void phy_sleep(phy_t phy)
{
    take();

    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Do the real sleep
    sleep(_phy);

    give();
}

void phy_idle(phy_t phy)
{
    take();

    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Do the real idle
    idle(_phy);

    give();
}

phy_status_t phy_set_channel(phy_t phy, uint8_t channel)
{
    take();

    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Check state
    switch (_phy->state)
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

            give();
            // State is invalid for channel setting, return error
            return PHY_ERR_INVALID_STATE;
    }

    // Check min and max value
    if (rf2xx_get_type(_phy->radio) == RF2XX_TYPE_2_4GHz)
    {
        if (channel < PHY_2400_MIN_CHANNEL)
        {
            channel = PHY_2400_MIN_CHANNEL;
        }
        else if (channel > PHY_2400_MAX_CHANNEL)
        {
            channel = PHY_2400_MAX_CHANNEL;
        }
    }
    else
    {
        channel = PHY_868_MIN_CHANNEL;
    }

    // Write new data to register
    rf2xx_reg_write(_phy->radio, RF2XX_REG__PHY_CC_CCA,
            RF2XX_PHY_CC_CCA_DEFAULT__CCA_MODE | channel);

    // Go back to sleep if it was in this state
    if (_phy->state == PHY_STATE_SLEEP)
    {
        rf2xx_sleep(_phy->radio);
    }

    give();
    return PHY_SUCCESS;
}

phy_status_t phy_set_power(phy_t phy, phy_power_t power)
{
    take();

    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Check state
    switch (_phy->state)
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

            give();

            // State is invalid for channel setting, return error
            return PHY_ERR_INVALID_STATE;
    }

    if (rf2xx_get_type(_phy->radio) == RF2XX_TYPE_2_4GHz)
    {
        power = convert_power(power);

        // Write new data to register
        rf2xx_reg_write(_phy->radio, RF2XX_REG__PHY_TX_PWR,
                RF2XX_PHY_TX_PWR_DEFAULT__PA_BUF_LT
                        | RF2XX_PHY_TX_PWR_DEFAULT__PA_LT | power);
    }
    else
    {
        //! \todo set TX power for 868MHz, for now set -1dBm, max tolerated (cf p 107)
        rf2xx_reg_write(_phy->radio, RF2XX_REG__PHY_TX_PWR, 0x47);
    }

    // Go back to sleep if it was in this state
    if (_phy->state == PHY_STATE_SLEEP)
    {
        rf2xx_sleep(_phy->radio);
    }

    give();
    return PHY_SUCCESS;
}

static phy_status_t phy_ed_cca_measure(phy_t phy, int32_t *result, int32_t ed)
{
    take();

    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Check state
    switch (_phy->state)
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

            give();
            return PHY_ERR_INVALID_STATE;
    }

    // Disable interrupt
    rf2xx_irq_disable(_phy->radio);

    // Start RX
    rf2xx_set_state(_phy->radio, RF2XX_TRX_STATE__RX_ON);

    uint8_t reg;
    uint32_t t_end;

    // Wait until RX is entered
    t_end = soft_timer_time() + RF_MAX_WAIT;

    do
    {
        reg = rf2xx_get_status(_phy->radio);

        // Check for block
        if (!soft_timer_a_is_before_b(soft_timer_time(), t_end))
        {
            log_error("RF delay expired #0");

            give();
            return PHY_ERR_INTERNAL;
        }
    } while ((reg & RF2XX_TRX_STATUS__RX_ON) == 0);

    if (ed)
    {
        // Enable ED END IRQ
        rf2xx_reg_write(_phy->radio, RF2XX_REG__IRQ_MASK,
                RF2XX_IRQ_STATUS_MASK__CCA_ED_DONE);

        // Request a ED measurement
        rf2xx_reg_write(_phy->radio, RF2XX_REG__PHY_ED_LEVEL, 0xAA);

        // Wait until ED is performed
        t_end = soft_timer_time() + RF_MAX_WAIT;

        do
        {
            reg = rf2xx_reg_read(_phy->radio, RF2XX_REG__IRQ_STATUS);

            // Check for block
            if (!soft_timer_a_is_before_b(soft_timer_time(), t_end))
            {
                log_error("RF delay expired #1");

                give();
                return PHY_ERR_INTERNAL;
            }
        } while ((reg & RF2XX_IRQ_STATUS_MASK__CCA_ED_DONE) == 0);

        // Set ED value
        *result = -91 + rf2xx_reg_read(_phy->radio, RF2XX_REG__PHY_ED_LEVEL);
    }
    else
    {
        // Request a CCA
        reg = rf2xx_reg_read(_phy->radio, RF2XX_REG__PHY_CC_CCA);
        reg |= RF2XX_PHY_CC_CCA_MASK__CCA_REQUEST;
        rf2xx_reg_write(_phy->radio, RF2XX_REG__PHY_CC_CCA, reg);

        // Wait until CCA is performed
        t_end = soft_timer_time() + RF_MAX_WAIT;

        do
        {
            reg = rf2xx_reg_read(_phy->radio, RF2XX_REG__TRX_STATUS);

            // Check for block
            if (!soft_timer_a_is_before_b(soft_timer_time(), t_end))
            {
                log_error("RF delay expired #1");

                give();
                return PHY_ERR_INTERNAL;
            }
        } while ((reg & RF2XX_TRX_STATUS_MASK__CCA_DONE) == 0);

        // Set CCA STATUS
        *result = !!(reg & RF2XX_TRX_STATUS_MASK__CCA_STATUS);
    }

    // Stop RX
    rf2xx_set_state(_phy->radio, RF2XX_TRX_STATE__FORCE_TRX_OFF);

    // Go back to sleep if needed
    if (_phy->state == PHY_STATE_SLEEP)
    {
        // Sleep
        rf2xx_sleep(_phy->radio);
    }

    give();

    // Return success
    return PHY_SUCCESS;
}

phy_status_t phy_ed(phy_t phy, int32_t *ed)
{
    return phy_ed_cca_measure(phy, ed, 1);
}

phy_status_t phy_cca(phy_t phy, int32_t *cca)
{
    return phy_ed_cca_measure(phy, cca, 0);
}
phy_status_t phy_rx(phy_t phy, uint32_t rx_time, uint32_t timeout_time,
        phy_packet_t *pkt, phy_handler_t handler)
{
    take();

    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Check the provided packet
    if (pkt == NULL)
    {
        log_error("Invalid provided RX packet: NULL");
        HALT();
    }

    // Check state
    switch (_phy->state)
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

            give();
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

    // Compute delta
    uint32_t now = soft_timer_time();
    int16_t delta_rx = rx_time - now;
    int16_t delta_timeout = timeout_time - now;

    // Check for invalid time
    if ((rx_time && (delta_rx < 1)) || (timeout_time && (delta_timeout < 1))
            || (rx_time && timeout_time && (timeout_time - rx_time < 1)))
    {
        // Invalid timing (too late) go back to previous state
        log_warning("RX too late or timeout too late");
        // Check state
        switch (_phy->state)
        {
            case PHY_STATE_SLEEP:
                // Back to sleep
                sleep(_phy);
                break;
            case PHY_STATE_IDLE:
                // Back to idle
                idle(_phy);
                break;
            default:
                break;
        }

        give();
        return PHY_ERR_TOO_LATE;
    }

    // Store State
    _phy->state = PHY_STATE_RX_WAIT;

    // Block low power
    platform_prevent_low_power();

    // Check if an RX time is specified
    if (rx_time)
    {
        // Set RX start time
        timer_set_channel_compare(_phy->timer, _phy->channel, rx_time & 0xFFFF,
                (timer_handler_t) rx_start_handler, _phy);

        give();
    }
    else
    {
        // Release mutex before
        give();

        // Set RX now
        start_rx(_phy);
    }

    return PHY_SUCCESS;
}

phy_status_t phy_tx(phy_t phy, uint32_t tx_time, phy_packet_t *pkt,
        phy_handler_t handler)
{
    take();

    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Check the provided packet
    if (pkt == NULL)
    {
        log_error("Invalid provided TX packet: NULL");

        give();
        return PHY_ERR_INTERNAL;
    }

    // Check length is valid
    if (pkt->length > PHY_MAX_TX_LENGTH)
    {
        log_error("length too big: %u", pkt->length);

        give();
        return PHY_ERR_INVALID_LENGTH;
    }

    // Check state
    switch (_phy->state)
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

            give();
            return PHY_ERR_INVALID_STATE;
    }

    // Store packet
    _phy->pkt = pkt;

    // Store the handler
    _phy->handler = handler;

    // Disable interrupt
    rf2xx_irq_disable(_phy->radio);

    // Set IRQ to reflect TRX END (end of TX) only
    rf2xx_reg_write(_phy->radio, RF2XX_REG__IRQ_MASK,
            RF2XX_IRQ_STATUS_MASK__TRX_END);

    // Read IRQ to clear it
    rf2xx_reg_read(_phy->radio, RF2XX_REG__IRQ_STATUS);

    // If radio has external PA, enable DIG3/4
    if (rf2xx_has_pa(_phy->radio))
    {
        // Enable the PA
        rf2xx_pa_enable(_phy->radio);

        // Activate DIG3/4 pins
        uint8_t reg = rf2xx_reg_read(_phy->radio, RF2XX_REG__TRX_CTRL_1);
        reg |= RF2XX_TRX_CTRL_1_MASK__PA_EXT_EN;
        rf2xx_reg_write(_phy->radio, RF2XX_REG__TRX_CTRL_1, reg);
    }

    // Enable IRQ interrupt
    rf2xx_irq_enable(_phy->radio);

    // Change to PLL ON
    rf2xx_set_state(_phy->radio, RF2XX_TRX_STATE__PLL_ON);

    uint32_t launch_now = 0;
    int16_t spare_time = 0;

    // Backup state and store new State
    uint32_t last_state = _phy->state;
    _phy->state = PHY_STATE_TX_WAIT;

    // Check if TX time is delayed
    if (tx_time)
    {
        // Update TX alarm time
        tx_time -= PHY_TIMING__TX_OFFSET;

        // Check if Time to start is not elapsed
        spare_time = tx_time - soft_timer_time();

        if (spare_time > 1)
        {
            // Set timer
            timer_set_channel_compare(_phy->timer, _phy->channel,
                    tx_time & 0xFFFF, tx_start_handler, _phy);

            // Set output compare pin if available
            if (_phy->timer_is_slptr)
            {
                timer_activate_channel_output(_phy->timer, _phy->channel,
                        TIMER_OUTPUT_MODE_SET_ACTIVE);
                rf2xx_slp_tr_config_timer(_phy->radio);
            }
        }
        else
        {
            log_warning("TX too late: %d", -spare_time);

            // Go back to previous state
            _phy->state = last_state;
            if (_phy->state == PHY_STATE_SLEEP)
            {
                // Back to sleep
                sleep(_phy);
            }
            else
            {
                // Back to idle
                idle(_phy);
            }

            give();
            return PHY_ERR_TOO_LATE;
        }
    }
    else
    {
        // No TX time, start ASAP
        launch_now = 1;
    }

    // Wait until PLL ON state
    uint8_t status;
    uint32_t t_end = soft_timer_time() + RF_MAX_WAIT;

    do
    {
        status = rf2xx_get_status(_phy->radio);

        // Check for block
        if (!soft_timer_a_is_before_b(soft_timer_time(), t_end))
        {
            log_error("RF delay expired #2");

            // Go back to previous state
            _phy->state = last_state;
            if (_phy->state == PHY_STATE_SLEEP)
            {
                // Back to sleep
                sleep(_phy);
            }
            else
            {
                // Back to idle
                idle(_phy);
            }

            give();
            return PHY_ERR_INVALID_STATE;
        }
    } while (status != RF2XX_TRX_STATUS__PLL_ON);

    // Copy the packet to the radio FIFO
    rf2xx_fifo_write_first(_phy->radio, _phy->pkt->length + 2);
    rf2xx_fifo_write_remaining_async(_phy->radio, _phy->pkt->data,
            _phy->pkt->length, NULL, NULL);

    // Block low power
    platform_prevent_low_power();

    // Launch now if required
    if (launch_now)
    {
        tx_start_handler(_phy, timer_time(_phy->timer));
    }

    give();

    // Return Success
    return PHY_SUCCESS;
}

phy_power_t phy_convert_power(float power)
{
    phy_power_t result;

    if (power < -29.5f)
    {
        result = PHY_POWER_m30dBm;
    }
    else if (power < -28.5f)
    {
        result = PHY_POWER_m29dBm;
    }
    else if (power < -27.5f)
    {
        result = PHY_POWER_m28dBm;
    }
    else if (power < -26.5f)
    {
        result = PHY_POWER_m27dBm;
    }
    else if (power < -25.5f)
    {
        result = PHY_POWER_m26dBm;
    }
    else if (power < -24.5f)
    {
        result = PHY_POWER_m25dBm;
    }
    else if (power < -23.5f)
    {
        result = PHY_POWER_m24dBm;
    }
    else if (power < -22.5f)
    {
        result = PHY_POWER_m23dBm;
    }
    else if (power < -21.5f)
    {
        result = PHY_POWER_m22dBm;
    }
    else if (power < -20.5f)
    {
        result = PHY_POWER_m21dBm;
    }
    else if (power < -19.5f)
    {
        result = PHY_POWER_m20dBm;
    }
    else if (power < -18.5f)
    {
        result = PHY_POWER_m19dBm;
    }
    else if (power < -17.5f)
    {
        result = PHY_POWER_m18dBm;
    }
    else if (power < -16.5f)
    {
        result = PHY_POWER_m17dBm;
    }
    else if (power < -15.5f)
    {
        result = PHY_POWER_m16dBm;
    }
    else if (power < -14.5f)
    {
        result = PHY_POWER_m15dBm;
    }
    else if (power < -13.5f)
    {
        result = PHY_POWER_m14dBm;
    }
    else if (power < -12.5f)
    {
        result = PHY_POWER_m13dBm;
    }
    else if (power < -11.5f)
    {
        result = PHY_POWER_m12dBm;
    }
    else if (power < -10.5f)
    {
        result = PHY_POWER_m11dBm;
    }
    else if (power < -9.5f)
    {
        result = PHY_POWER_m10dBm;
    }
    else if (power < -8.5f)
    {
        result = PHY_POWER_m9dBm;
    }
    else if (power < -7.5f)
    {
        result = PHY_POWER_m8dBm;
    }
    else if (power < -6.5f)
    {
        result = PHY_POWER_m7dBm;
    }
    else if (power < -5.5f)
    {
        result = PHY_POWER_m6dBm;
    }
    else if (power < -4.5f)
    {
        result = PHY_POWER_m5dBm;
    }
    else if (power < -3.5f)
    {
        result = PHY_POWER_m4dBm;
    }
    else if (power < -2.5f)
    {
        result = PHY_POWER_m3dBm;
    }
    else if (power < -1.5f)
    {
        result = PHY_POWER_m2dBm;
    }
    else if (power < -0.5f)
    {
        result = PHY_POWER_m1dBm;
    }
    else if (power < 0.35f)
    {
        result = PHY_POWER_0dBm;
    }
    else if (power < 0.85f)
    {
        result = PHY_POWER_0_7dBm;
    }
    else if (power < 1.15f)
    {
        result = PHY_POWER_1dBm;
    }
    else if (power < 1.55f)
    {
        result = PHY_POWER_1_3dBm;
    }
    else if (power < 1.9f)
    {
        result = PHY_POWER_1_8dBm;
    }
    else if (power < 2.15f)
    {
        result = PHY_POWER_2dBm;
    }
    else if (power < 2.55f)
    {
        result = PHY_POWER_2_3dBm;
    }
    else if (power < 2.9f)
    {
        result = PHY_POWER_2_8dBm;
    }
    else if (power < 3.5f)
    {
        result = PHY_POWER_3dBm;
    }
    else if (power < 4.5f)
    {
        result = PHY_POWER_4dBm;
    }
    else
    {
        result = PHY_POWER_5dBm;
    }

    return result;
}
phy_status_t phy_jam(phy_t phy, uint8_t channel, phy_power_t power)
{
    take();

    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = phy;

    // Check state
    switch (_phy->state)
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

            give();
            return PHY_ERR_INVALID_STATE;
    }

    // Store State
    _phy->state = PHY_STATE_JAMMING;

    // Disable interrupt
    rf2xx_irq_disable(_phy->radio);

    // Step 1: RESET
    rf2xx_reset(_phy->radio);
    rf2xx_wakeup(_phy->radio);

    // Enable the PA
    rf2xx_pa_enable(_phy->radio);

    // Step 2: Set IRQ mask PLL ON
    rf2xx_reg_write(_phy->radio, RF2XX_REG__IRQ_MASK,
            RF2XX_IRQ_STATUS_MASK__PLL_LOCK);
    rf2xx_reg_read(_phy->radio, RF2XX_REG__IRQ_STATUS);

    // Step 3: disable TX_AUTO_CRC_ON
    rf2xx_reg_write(_phy->radio, RF2XX_REG__TRX_CTRL_1, 0);

    // Step 4: Set State TRX_OFF
    rf2xx_reg_write(_phy->radio, RF2XX_REG__TRX_STATE,
            RF2XX_TRX_STATE__FORCE_TRX_OFF);

    // Step 5: Set clock at pin 17
    rf2xx_reg_write(_phy->radio, RF2XX_REG__TRX_CTRL_0, 0x01);

    // Step 6: Set tx_channel
    rf2xx_reg_write(_phy->radio, RF2XX_REG__PHY_CC_CCA,
            RF2XX_PHY_CC_CCA_DEFAULT__CCA_MODE | channel);

    // Step 7: Set output power as specified
    rf2xx_reg_write(_phy->radio, RF2XX_REG__PHY_TX_PWR,
            convert_power(power));

    // Step 8: Verify TRX_OFF status
    rf2xx_reg_read(_phy->radio, RF2XX_REG__TRX_STATUS);

    // If radio has external PA, enable DIG3/4
    if (rf2xx_has_pa(_phy->radio))
    {
        // Enable the PA
        rf2xx_pa_enable(_phy->radio);

        // Activate DIG3/4 pins
        uint8_t reg = rf2xx_reg_read(_phy->radio, RF2XX_REG__TRX_CTRL_1);
        reg |= RF2XX_TRX_CTRL_1_MASK__PA_EXT_EN;
        rf2xx_reg_write(_phy->radio, RF2XX_REG__TRX_CTRL_1, reg);
    }

    // Step 9: Enable continuous transmission test mode step #1
    rf2xx_reg_write(_phy->radio, RF2XX_REG__TST_CTRL_DIGI, 0x0F);

    // Step 10: not used for PRBS
    rf2xx_reg_write(_phy->radio, 0x0C, 0x03);

    // Step 11: idem
    rf2xx_reg_write(_phy->radio, 0x0A, 0xA7);

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
        frame_buf[i] = rand(); /* Modulated data */
    }

    // Write data
    rf2xx_fifo_write(_phy->radio, frame_buf, 128);

    // Step 13: Enable continuous transmission test mode step #2
    rf2xx_reg_write(_phy->radio, RF2XX_REG__PART_NUM, 0x54);

    // Step 14: Enable continuous transmission test mode step #3
    rf2xx_reg_write(_phy->radio, RF2XX_REG__PART_NUM, 0x46);

    // Step 15: Enable PLL_ON state
    rf2xx_reg_write(_phy->radio, RF2XX_REG__TRX_STATE, RF2XX_TRX_STATE__PLL_ON);

    // Step 16: Wait for PLL_LOCK
    while (!rf2xx_reg_read(_phy->radio, RF2XX_REG__IRQ_STATUS)
            & RF2XX_IRQ_STATUS_MASK__PLL_LOCK)
    {
        asm volatile("nop");
    }

    // Step 17: Enter BUSY TX
    rf2xx_reg_write(_phy->radio, RF2XX_REG__TRX_STATE, RF2XX_TRX_STATE__TX_START);


    give();

    // Return Success
    return PHY_SUCCESS;
}
// ***************** Internal methods (mutex taken before) ******************* //

static int convert_power(phy_power_t power)
{
    // Convert power value to register value
    switch (power)
    {
        case PHY_POWER_m30dBm:
        case PHY_POWER_m29dBm:
        case PHY_POWER_m28dBm:
        case PHY_POWER_m27dBm:
        case PHY_POWER_m26dBm:
        case PHY_POWER_m25dBm:
        case PHY_POWER_m24dBm:
        case PHY_POWER_m23dBm:
        case PHY_POWER_m22dBm:
        case PHY_POWER_m21dBm:
        case PHY_POWER_m20dBm:
        case PHY_POWER_m19dBm:
        case PHY_POWER_m18dBm:
        case PHY_POWER_m17dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m17dBm;
            break;
        case PHY_POWER_m16dBm:
        case PHY_POWER_m15dBm:
        case PHY_POWER_m14dBm:
        case PHY_POWER_m13dBm:
        case PHY_POWER_m12dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m12dBm;
            break;
        case PHY_POWER_m11dBm:
        case PHY_POWER_m10dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m9dBm;
            break;
        case PHY_POWER_m9dBm:
        case PHY_POWER_m8dBm:
        case PHY_POWER_m7dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m7dBm;
            break;
        case PHY_POWER_m6dBm:
        case PHY_POWER_m5dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m5dBm;
            break;
        case PHY_POWER_m4dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m4dBm;
            break;
        case PHY_POWER_m3dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m3dBm;
            break;
        case PHY_POWER_m2dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m2dBm;
            break;
        case PHY_POWER_m1dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__m1dBm;
            break;
        case PHY_POWER_0dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__0dBm;
            break;
        case PHY_POWER_0_7dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__0_7dBm;
            break;
        case PHY_POWER_1dBm:
        case PHY_POWER_1_3dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__1_3dBm;
            break;
        case PHY_POWER_1_8dBm:
        case PHY_POWER_2dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__1_8dBm;
            break;
        case PHY_POWER_2_3dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__2_3dBm;
            break;
        case PHY_POWER_2_8dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__2_8dBm;
            break;
        case PHY_POWER_3dBm:
        case PHY_POWER_4dBm:
        case PHY_POWER_5dBm:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__3dBm;
            break;
        default:
            power = RF2XX_PHY_TX_PWR_TX_PWR_VALUE__0dBm;
            break;
    }
    return power;
}
static void reset(phy_rf2xx_t *_phy)
{
    // Stop timer
    timer_set_channel_compare(_phy->timer, _phy->channel, 0, NULL, NULL);

    // Stop any Asynchronous access
    rf2xx_fifo_access_cancel(_phy->radio);

    // Configure the radio interrupts
    rf2xx_irq_disable(_phy->radio);
    rf2xx_irq_configure(_phy->radio, irq_handler, _phy);

    // Configure DIG2 pin
    if (rf2xx_has_dig2(_phy->radio))
    {
        rf2xx_dig2_disable(_phy->radio);
        rf2xx_dig2_configure(_phy->radio, dig2_capture_handler, _phy);
    }

    // Reset the SLP_TR output
    rf2xx_slp_tr_clear(_phy->radio);

    // Reset the radio chip
    rf2xx_reset(_phy->radio);

    // Set default register values
    uint8_t reg;

    // Set DIG2 as output for RX_START if enabled
    if (rf2xx_has_dig2(_phy->radio))
    {
        reg = rf2xx_reg_read(_phy->radio, RF2XX_REG__TRX_CTRL_1);
        reg |= RF2XX_TRX_CTRL_1_MASK__IRQ_2_EXT_EN;
        rf2xx_reg_write(_phy->radio, RF2XX_REG__TRX_CTRL_1, reg);
    }

    if (rf2xx_get_type(_phy->radio) == RF2XX_TYPE_2_4GHz)
    {
        // Enable Dynamic Frame Buffer Protection, standard data rate (250kbps)
        reg = RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE;
        rf2xx_reg_write(_phy->radio, RF2XX_REG__TRX_CTRL_2, reg);

        // Set max TX power
        reg = RF2XX_PHY_TX_PWR_DEFAULT__PA_BUF_LT
                | RF2XX_PHY_TX_PWR_DEFAULT__PA_LT
                | RF2XX_PHY_TX_PWR_TX_PWR_VALUE__3dBm;
        rf2xx_reg_write(_phy->radio, RF2XX_REG__PHY_TX_PWR, reg);
    }
    else
    {
        // Enable Dynamic Frame Buffer Protection, OQPSK-200
        reg = RF2XX_TRX_CTRL_2_MASK__RX_SAFE_MODE
                | RF2XX_TRX_CTRL_2_MASK__BPSK_OQPSK
                | RF2XX_TRX_CTRL_2_OQPSK_DATA_RATE__200_500;
        rf2xx_reg_write(_phy->radio, RF2XX_REG__TRX_CTRL_2, reg);

        // -1dBm to match EU rules (cf p107 of datasheet) => 0x47
        rf2xx_reg_write(_phy->radio, RF2XX_REG__PHY_TX_PWR, 0x47);
    }

    // Disable CLKM signal
    reg = RF2XX_TRX_CTRL_0_DEFAULT__PAD_IO
            | RF2XX_TRX_CTRL_0_DEFAULT__PAD_IO_CLKM
            | RF2XX_TRX_CTRL_0_DEFAULT__CLKM_SHA_SEL
            | RF2XX_TRX_CTRL_0_CLKM_CTRL__OFF;
    rf2xx_reg_write(_phy->radio, RF2XX_REG__TRX_CTRL_0, reg);

    /** Set XCLK TRIM
     * \todo this highly depends on the board
     */
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

    // Remove handler and packet
    _phy->handler = NULL;
    _phy->pkt = NULL;

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
    switch (_phy->state)
    {
        case PHY_STATE_SLEEP:
            // Wakeup
            rf2xx_wakeup(_phy->radio);
            break;

        case PHY_STATE_RX:
        case PHY_STATE_RX_WAIT:
        case PHY_STATE_TX:
        case PHY_STATE_TX_WAIT:
            platform_release_low_power();
            break;

        default:
            // Nothing to do
            break;
    }

    // Cancel any ongoing transfer
    rf2xx_fifo_access_cancel(_phy->radio);

    // Force IDLE
    rf2xx_set_state(_phy->radio, RF2XX_TRX_STATE__FORCE_TRX_OFF);

    // If radio has external PA, disable DIG3/4
    if (rf2xx_has_pa(_phy->radio))
    {
        // Enable the PA
        rf2xx_pa_disable(_phy->radio);

        // De-activate DIG3/4 pins
        uint8_t reg = rf2xx_reg_read(_phy->radio, RF2XX_REG__TRX_CTRL_1);
        reg &= ~RF2XX_TRX_CTRL_1_MASK__PA_EXT_EN;
        rf2xx_reg_write(_phy->radio, RF2XX_REG__TRX_CTRL_1, reg);
    }

    // Clear pkt pointer
    _phy->pkt = NULL;

    // Save state
    _phy->state = PHY_STATE_IDLE;
}

// *********************** INPUT handlers (posted from ISR) ************************ //

static void start_rx(handler_arg_t arg)
{
    take();

    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = arg;

    // Check state and pkt
    if (_phy->state != PHY_STATE_RX_WAIT)
    {
        log_error("start_rx but state not RX wait: %x", _phy->state);

        give();
        return;
    }

    if (_phy->pkt == NULL)
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

    // Reset IRQ to TRX END and RX_START if no DIG2 available
    rf2xx_reg_write(_phy->radio, RF2XX_REG__IRQ_MASK,
            RF2XX_IRQ_STATUS_MASK__TRX_END
                    | (rf2xx_has_dig2(_phy->radio) ? 0 :
                            RF2XX_IRQ_STATUS_MASK__RX_START));

    // Read IRQ to clear it
    rf2xx_reg_read(_phy->radio, RF2XX_REG__IRQ_STATUS);

    // If radio has external PA, enable DIG3/4
    if (rf2xx_has_pa(_phy->radio))
    {
        // Enable the PA
        rf2xx_pa_enable(_phy->radio);

        // Activate DIG3/4 pins
        uint8_t reg = rf2xx_reg_read(_phy->radio, RF2XX_REG__TRX_CTRL_1);
        reg |= RF2XX_TRX_CTRL_1_MASK__PA_EXT_EN;
        rf2xx_reg_write(_phy->radio, RF2XX_REG__TRX_CTRL_1, reg);
    }

    // Enable IRQ interrupt
    rf2xx_irq_enable(_phy->radio);

    // Enable DIG2 time-stamping
    if (rf2xx_has_dig2(_phy->radio))
    {
        rf2xx_dig2_enable(_phy->radio);
    }

    // Start RX
    rf2xx_set_state(_phy->radio, RF2XX_TRX_STATE__RX_ON);

    // Loop until RX_ON is entered
    uint8_t status;
    uint32_t t_end = soft_timer_time() + RF_MAX_WAIT;

    do
    {
        status = rf2xx_get_status(_phy->radio);

        // Check for block
        if (!soft_timer_a_is_before_b(soft_timer_time(), t_end))
        {
            log_error("RF delay expired #3");
            break;
        }
    } while ((status & RF2XX_TRX_STATUS_MASK__TRX_STATUS)
            != RF2XX_TRX_STATUS__RX_ON);

    // Set timer for timeout, if any
    if (_phy->rx_timeout)
    {
        timer_set_channel_compare(_phy->timer, _phy->channel,
                _phy->rx_timeout & 0xFFFF, (timer_handler_t) rx_timeout_handler,
                _phy);
    }
    else
    {
        // Disable timer
        timer_set_channel_compare(_phy->timer, _phy->channel, 0, NULL, NULL);
    }

    give();
}

static phy_status_t handle_rx_start(phy_rf2xx_t *_phy)
{
    // Check state and pkt
    if (_phy->state != PHY_STATE_RX)
    {
        log_error("handle_rx_start but state not RX: %x", _phy->state);
        HALT();
    }

    if (_phy->pkt == NULL)
    {
        log_error("handle_rx_start but pkt NULL");
        HALT();
    }

    // Force IDLE
    rf2xx_set_state(_phy->radio, RF2XX_TRX_STATE__FORCE_TRX_OFF);

    // Check the CRC is good
    if (!(rf2xx_reg_read(_phy->radio, RF2XX_REG__PHY_RSSI)
            & RF2XX_PHY_RSSI_MASK__RX_CRC_VALID))
    {
        // Stop timer
        timer_set_channel_compare(_phy->timer, _phy->channel, 0, NULL, NULL);

        // Force Idle
        idle(_phy);

        // Request handler call
        return PHY_RX_CRC_ERROR;
    }

    // Read length byte (first byte)
    _phy->pkt->length = rf2xx_fifo_read_first(_phy->radio);

    // Check valid length (not zero and enough space to store it)
    if ((_phy->pkt->length == 0)
            || (_phy->pkt->data + _phy->pkt->length
                    > _phy->pkt->raw_data + PHY_MAX_RX_LENGTH))
    {
        // Error length, end transfer
        rf2xx_fifo_read_remaining(_phy->radio, _phy->pkt->data, 0);

        // Force Idle
        idle(_phy);

        return PHY_RX_LENGTH_ERROR;
    }

    // Store RX start time
    _phy->pkt->t_rx_start = soft_timer_time();

    // Retrieve remaining of the data asynchronously (+1) to have the LQI
    uint8_t length = _phy->pkt->length + 1;

    rf2xx_fifo_read_remaining_async(_phy->radio, _phy->pkt->data, length,
            fifo_read_done_handler, _phy);

    return PHY_SUCCESS;
}

// *********************** INPUT handlers (posted from ISR) ************************ //

static void handle_rx_end(handler_arg_t arg)
{
    take();

    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = arg;

    // Check state and pkt
    if (_phy->state != PHY_STATE_RX)
    {
        log_error("handle_rx_end but state not RX: %x", _phy->state);

        give();
        return;
    }

    if (_phy->pkt == NULL)
    {
        log_error("handle_rx_end but pkt NULL");
        HALT();
    }

    // Store RX END time
    _phy->pkt->t_rx_end = soft_timer_time();

    // Read the ED value (~RSSI)
    _phy->pkt->rssi = -91
            + rf2xx_reg_read(_phy->radio, RF2XX_REG__PHY_ED_LEVEL);

    // Read the LQI (last byte read from the framebuffer)
    _phy->pkt->lqi = _phy->pkt->data[_phy->pkt->length];

    // Remove status bytes from length
    _phy->pkt->length -= 2;

    int16_t dt = _phy->pkt->t_rx_end - _phy->pkt->t_rx_start;
    if (dt > 500)
    {
        log_error("Too much time to read a packet, length = %u",
                _phy->pkt->length + 1);
        log_info("rx_end : %u", _phy->pkt->t_rx_end - _phy->pkt->t_rx_start);
        HALT();
    }

    // Go to idle
    idle(_phy);

    give();

    // Call RX handler if any
    if (_phy->handler)
    {
        _phy->handler(PHY_SUCCESS);
    }
}
static void handle_rx_timeout(handler_arg_t arg)
{
    take();

    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = arg;

    // Handle Timeout should only occur in RX state
    if ((_phy->state != PHY_STATE_RX_WAIT) && (_phy->state != PHY_STATE_RX))
    {
        // Interrupt may have slipped, discard
        log_warning("invalid state for phy handle_rx_timeout: %x", _phy->state);

        give();
        return;
    }

    // Set Idle
    idle(_phy);

    give();

    // Notify receiving failed
    if (_phy->handler)
    {
        _phy->handler(PHY_RX_TIMEOUT_ERROR);
    }
}

static void handle_irq(handler_arg_t arg)
{
    take();

    // Cast to RF2XX PHY
    phy_rf2xx_t *_phy = arg;

    // Get the IRQ status
    uint8_t irq_status = rf2xx_reg_read(_phy->radio, RF2XX_REG__IRQ_STATUS);

    // Switch on the state
    switch (_phy->state)
    {
        case PHY_STATE_RX:
            // Check if RX_START happened
            if (irq_status == RF2XX_IRQ_STATUS_MASK__RX_START)
            {
                // Store IRQ timestamp in SFD
                if (!rf2xx_has_dig2(_phy->radio))
                {
                    _phy->pkt->timestamp = _phy->pkt->eop_time;
                }
            }
            // Check if TRX_END happened
            else if (irq_status == RF2XX_IRQ_STATUS_MASK__TRX_END)
            {
                // Start processing
                phy_status_t status = handle_rx_start(_phy);
                // Call handler on error
                if (status != PHY_SUCCESS)
                {
                    give();
                    if (_phy->handler)
                    {
                        _phy->handler(status);
                    }
                    return;
                }
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
            if (irq_status == RF2XX_IRQ_STATUS_MASK__TRX_END)
            {
                // Go to Idle
                idle(_phy);

                give();

                // Notify sending is done if handler is not null
                if (_phy->handler)
                {
                    _phy->handler(PHY_SUCCESS);
                }
                return;
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
            break;
    }

    give();
}

// ************************** Interrupt Routines ************************** //

/* Those are called from interrupt service routines */
static void dig2_capture_handler(handler_arg_t arg, uint16_t timer_value)
{
    // Cast to PHY
    phy_rf2xx_t *_phy = arg;

    // Check state
    if (_phy->state != PHY_STATE_RX)
    {
        log_error("DIG2 but state not RX");
        HALT();
    }

    // Stop RX timeout alarm
    timer_set_channel_compare(_phy->timer, _phy->channel, 0, NULL, NULL);

    // Store timer value, if rx packet specified
    if (_phy->pkt)
    {
        _phy->pkt->timestamp = soft_timer_convert_time(timer_value);
    }
}
static void rx_start_handler(handler_arg_t arg, uint16_t timer_value)
{
    // is not used
    (void) timer_value;

    // Request a post of start_rx
    event_post_from_isr(EVENT_QUEUE_NETWORK, start_rx, arg);
}

static void tx_start_handler(handler_arg_t arg, uint16_t timer_value)
{
    // Cast to PHY
    phy_rf2xx_t *_phy = arg;

    if (_phy->state == PHY_STATE_TX_WAIT)
    {
        // Start TX by setting SLP_TR if it wasn't set
        rf2xx_slp_tr_set(_phy->radio);

        if (_phy->timer_is_slptr)
        {
            // Disable timer output
            timer_activate_channel_output(_phy->timer, _phy->channel, TIMER_OUTPUT_MODE_FORCE_INACTIVE);

            // Store time
            _phy->pkt->timestamp = soft_timer_convert_time(
                    timer_value) + PHY_TIMING__TX_OFFSET;
        }
        else
        {
            // Store time
            _phy->pkt->timestamp = soft_timer_time() + PHY_TIMING__TX_OFFSET;
        }

        // Store State
        _phy->state = PHY_STATE_TX;

        // Disable timer
        timer_set_channel_compare(_phy->timer, _phy->channel, 0, NULL, NULL);

        // Clear SLP_TR
        rf2xx_slp_tr_clear(_phy->radio);
    }
}

static void rx_timeout_handler(handler_arg_t arg, uint16_t timer_value)
{
    // is not used
    (void) timer_value;

    // Cast to PHY
    phy_rf2xx_t *_phy = arg;

    // Disable timer
    timer_set_channel_compare(_phy->timer, _phy->channel, 0, NULL, NULL);

    // Call handle_rx_timeout handler from event task
    event_post_from_isr(EVENT_QUEUE_NETWORK, handle_rx_timeout, arg);
}

static void irq_handler(handler_arg_t arg)
{
    // Cast to PHY
    phy_rf2xx_t *_phy = arg;

    // Store IRQ time in EOP
    _phy->pkt->eop_time = soft_timer_time();

    // Call IRQ handler from event task
    event_post_from_isr(EVENT_QUEUE_NETWORK, handle_irq, arg);
}

static void fifo_read_done_handler(handler_arg_t arg)
{
    // Call RX end handler from event task
    event_post_from_isr(EVENT_QUEUE_NETWORK, handle_rx_end, arg);
}
