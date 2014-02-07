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

/**
 * \file platform.h
 *
 *  \date Jul 6, 2011
 *  \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 *  \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

/**
 * \addtogroup platform
 * @{
 */

/**
 * \defgroup API Generic platform interface
 *
 * This module provides generic access to a platform. It provides default
 * serial port access, default phy radio interface, plus methods to handle
 * low power states and more.
 *
 *@{
 */

#include <stdint.h>
#include <stddef.h>

#if !defined(PLATFORM_OS) || (PLATFORM_OS == FREERTOS)
/* FreeRTOS include */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#endif

/* Drivers Includes */
#include "drivers.h"
#include "platform_leds.h"

/* Phy include */
#include "phy.h"

/**
 * Reference to the UART driver used as output for printf.
 */
extern uart_t uart_print;
/**
 * Reference to the UART driver for external communication, or NULL if none.
 */
extern uart_t uart_external;

/**
 * Reference to the default PHY radio interface, if it exists.
 */
extern phy_t platform_phy;

/**
 * Reference to the external I2C driver
 */
extern i2c_t i2c_external;

/**
 * Initialize the platform.
 *
 * This should setup the clocks, the LEDs,
 * the UART and SPI, etc...
 */
void platform_init();

/**
 * Launch the scheduler (FreeRTOS)
 *
 * This method doesn't return
 */
void platform_run();

/**
 * Get the button state.
 *
 * \return 1 if the button is pushed, 0 if it is not.
 */
uint32_t button_state();

/**
 * Enable the button interrupt generation.
 *
 * This enabled the IRQ for the button, and store the handler to be called from
 * this interrupt.
 *
 * \param handler the handler function to be called on button push
 * \param handler_arg an argument to be provided to the handler
 */
void button_set_handler(handler_t handler, handler_arg_t handler_arg);

/**
 * Start the platform specific TICK generation for the FreeRTOS scheduler.
 *
 * \note This method is called by the FreeRTOS kernel, and should NOT be used
 *      anywhere else.
 *
 * \param frequency the frequency at which the ticks should be generated, in Hz.
 * \param handler the handler function to be called at each tick.
 * \param arg the argument to be provided to the handler function.
 */
void platform_start_freertos_tick(uint16_t frequency, handler_t handler,
                                  handler_arg_t arg);

/**
 * Handler prototype for IDLE listener.
 *
 * The IDLE listener is a function that will be called every time the FreeRTOS
 * kernel is about to enter low power. It is useful to perform regular watches
 * over some variable without perturbing the running tasks.
 *
 * \param arg the argument registered with the handler
 * \return 1 if CPU should not be kept awake (not entering low power), or 0 if
 *  it may
 */
typedef int32_t (*platform_idle_handler_t)(handler_arg_t arg);

/**
 * Set a handler to be called on each FreeRTOS idle hook.
 *
 * The registered handler will be called every time the FreeRTOS kernel is about
 * to enter low power.
 *
 * If the handler returns 1, the low power is skipped, otherwise it is entered.
 *
 * \param handler the handler to be called
 * \param arg the argument to provide to the handler
 */
void platform_set_idle_handler(platform_idle_handler_t handler,
                                handler_arg_t arg);

/**
 * Notify the platform that an underground activity is ongoing, to prevent
 * clock speed reducing.
 */
void platform_prevent_low_power();

/**
 * Notify the platform that a notified underground activity is terminated,
 * and the platform may enter real low power modes.
 */
void platform_release_low_power();

/**
 * Get the "high power" duration to potentially detect a malfunction.
 *
 * The high power duration corresponds to the time during which low power mode
 * has not been entered.
 *
 * \return the duration the platform has been in high power mode, in 32kHz ticks
 */
uint32_t platform_get_high_power_duration();

/** Available reset reasons */
typedef enum
{
    PLATFORM_RESET_UNKNOWN = 0,
    PLATFORM_RESET_POR = 1,
    PLATFORM_RESET_LP = 2,
    PLATFORM_RESET_WWD = 3,
    PLATFORM_RESET_IWD = 4,
    PLATFORM_RESET_SOFT = 5,
    PLATFORM_RESET_PIN = 6,
    PLATFORM_RESET_OBL = 7,
} platform_reset_cause_t;

/** The reset cause, or boot reason, set at boot */
extern platform_reset_cause_t platform_reset_cause;

/** Enter a critical section, disable interrupts */
void platform_enter_critical();
/** Exit a critical section
 *
 * Enable back interrupts if there is no nested critical section
 */
void platform_exit_critical();

/**
 * A function of prototype as follows may be defined to automatically start
 * the internal watchdog on startup if it returns 1.
 *
 * This method will be called before any initialization, therefore it should not
 * do any access to internal/external peripherals.
 *
 * int32_t platform_should_start_watchdog();
 */

/**
 * @}
 * @}
 */
#endif /* PLATFORM_H_ */
