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

/* FreeRTOS include */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

/* Drivers Includes */
#include "gpio.h"
#include "uart.h"

/* Phy include */
#include "phy.h"

#if defined(AZURE_LION)
#include "azure-lion/azurelion.h"
#elif defined(AZURE_LION_0_2)
#include "azure-lion-0.2/azurelion.h"
#elif defined(AGILE_FOX)
#include "agile-fox/agilefox.h"
#elif defined(INEMO)
#include "inemo/inemo.h"
#elif defined(NATIVE)
#include "native/native.h"
#elif defined(WISECOW)
#include "wise-cow/wisecow.h"
#elif defined(WISECOW_0_3)
#include "wise-cow-0.3/wisecow.h"
#else
#error "Platform not defined"
#endif

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

enum
{
    LED_0 = 0x1,
    LED_1 = 0x2,
    LED_2 = 0x4,
};

/**
 * Turn on some LEDs.
 *
 * \param leds a bitmap selecting the LEDs
 */
void leds_on(uint8_t leds);
/**
 * Turn off some LEDs.
 *
 * \param leds a bitmap selecting the LEDs
 */
void leds_off(uint8_t leds);
/**
 * Toggle on some LEDs.
 *
 * \param leds a bitmap selecting the LEDs
 */
void leds_toggle(uint8_t leds);

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

/**
 * Disable the platform UART, used when in release mode.
 */
void platform_disable_uart();

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

#endif /* PLATFORM_H_ */
