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
 * platform.h
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

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
#include "gpio.h"
#include "uart.h"
#include "adc.h"
#include "platform_leds.h"

/* Phy include */
#include "phy.h"

extern uart_t uart_print;
extern phy_t phy;

/**
 * Initialize the platform.
 *
 * This should setup the clocks, the LEDs,
 * the UART and SPI, etc...
 */
void platform_init();

/**
 * Launch the scheduler.
 *
 * This method doesn't return
 */
void platform_run();

/**
 * Get the button state.
 */
uint32_t button_state();

/**
 * Enable the button interrupt and store the handler.
 */
void button_set_handler(handler_t handler, handler_arg_t handler_arg);

/**
 * Start the platform specific TICK generation for the FreeRTOS scheduler.
 *
 * \param frequency the frequency at which the ticks should be generated, in Hz.
 * \param handler the handler function to be called at each tick.
 * \param arg the argument to be provided to the handler function.
 */
void platform_start_freertos_tick(uint16_t frequency, handler_t handler,
                                  handler_arg_t arg);

/** Handler for IDLE listener, should return 1 if CPU should not be halted */
typedef int32_t (*platform_idle_handler_t)(handler_arg_t arg);

/** Set a handler to be called on each FreeRTOS idle hook */
void platform_set_idle_handler(platform_idle_handler_t, handler_arg_t);

/**
 * Disable the platform UART, used when in release mode.
 */
void platform_disable_uart();

/**
 * Enable the platform USB.
 */
void platform_usb_enable();

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
extern platform_reset_cause_t platform_reset_cause;

#endif /* PLATFORM_H_ */
