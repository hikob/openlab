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
 * azurelion.c
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include "platform.h"
#include "azurelion.h"

#include "FreeRTOS.h"
#include "task.h"

#include "watchdog.h"
#include "rcc.h"
#include "rcc_sysclk_i.h"
#include "rcc_clock_i.h"
#include "flash.h"
#include "pwr.h"
#include "stm32l1xx/rtc.h"
#include "gpio.h"
#include "uart.h"
#include "unique_id.h"
#include "random.h"
#include "soft_timer.h"

#include "printf.h"
#include "debug.h"

__attribute__((weak)) int32_t platform_should_start_watchdog();
__attribute__((weak)) void platform_daughter_setup();

#define TRIM 0x60

#define TEST_USB 0

void platform_init()
{
    // Enable watchdog if requested
    if (platform_should_start_watchdog && platform_should_start_watchdog())
    {
        watchdog_enable(WATCHDOG_DIVIDER_256, 0xFFF);
    }

    // Store the reset cause
    platform_reset_cause = rcc_get_reset_cause();
    rcc_clear_reset_cause();

    // Enable the pwr clock
    rcc_apb_enable(RCC_APB_BUS_PWR, RCC_APB_BIT_PWR);

#if (defined(KEEP_UART) && (KEEP_UART)) || (defined(TEST_USB) && (TEST_USB))
    // Set PWR settings
    pwr_main_mode(PWR_VRANGE_1);

    // Enable HSE
    rcc_hse_enable();
    rcc_hse_indicate_frequency(16000000);

    // Set 64bit flash access
    flash_set_1ws();

    // Use PLL 96MHz, AHB 32MHz APBx 8MHz
    rcc_pll_enable(RCC_PLL_SOURCE_HSE, RCC_PLL_MUL_6, RCC_PLL_DIV_3);
    rcc_sysclk_set(RCC_SYSCLK_SOURCE_PLL, RCC_SYSCLK_AHB_PRE_1,
                   RCC_SYSCLK_APB_PRE_4, RCC_SYSCLK_APB_PRE_4);
#else
    // Set PWR settings: RANGE2
    pwr_main_mode(PWR_VRANGE_2);

    // Enable HSI
    rcc_hsi_enable();

    // Set 64bit flash access
    flash_set_1ws();

    // Use HSI 16MHz, APBx 8MHz
    rcc_sysclk_set(RCC_SYSCLK_SOURCE_HSI, RCC_SYSCLK_AHB_PRE_1,
                   RCC_SYSCLK_APB_PRE_2, RCC_SYSCLK_APB_PRE_2);
#endif

    // Set MSI low frequency
    rcc_msi_update_frequency_i(RCC_MSI_FREQ_131kHz, TRIM);

    // enable the LSE
    rcc_lse_enable();

    // Start the RTC
    rtc_enable();

    // Setup the drivers
    platform_drivers_setup();

    log_printf("\n\n");

    // Setup the LEDs
    platform_leds_setup();

    // Setup the libraries
    platform_lib_setup();

    // Setup the peripherals
    platform_periph_setup();

    // Setup the net stack
    platform_net_setup();

    // Restart all timers
    platform_drivers_restart_timers();

    // Feed the random number generator
    random_init(uid->uid32[2]);

    log_printf("\nPlatform RST %u HCLK: %uMHz\n", platform_reset_cause,
               rcc_sysclk_get_clock_frequency(RCC_SYSCLK_CLOCK_HCLK) / 1000000);

    log_printf("Platform starting in ");
    uint32_t i;

    for (i = 1; i > 0; i--)
    {
        log_printf("%u... ", i);
        soft_timer_delay_s(1);
    }

    // Disable UART if RELEASE and not KEEP UART
#if defined(RELEASE) && (RELEASE == 1) && (!defined(KEEP_UART) || (KEEP_UART == 0))
    log_printf("\nSilence!\n");
    platform_disable_uart();
#else
    log_printf("\nGO!\n");
#endif

#if !defined(ENABLE_DEBUG) || (ENABLE_DEBUG == 0)
    // Set all unused IOs to analog (disables JTAG)
    platform_io_setup();
#else
    rcc_enable_dbg_features();
#endif

    // DaughterBoard if any
    if (platform_daughter_setup)
    {
        platform_daughter_setup();
    }
}

static volatile int32_t underground_activities = 0;
static uint32_t lowpower_exit_time = 0;
void platform_prevent_low_power()
{
    if (underground_activities == 0)
    {
        lowpower_exit_time = soft_timer_time();
    }

    underground_activities++;

    if (underground_activities > 8)
    {
        log_error("Too many low power prevent!");
        HALT();
    }
}
void platform_release_low_power()
{
    underground_activities--;

    if (underground_activities < 0)
    {
        log_error("Too many low power release!");
        HALT();
    }
}
uint32_t platform_get_high_power_duration()
{
    if (underground_activities)
    {
        return soft_timer_time() - lowpower_exit_time;
    }
    else
    {
        return 0;
    }
}

static struct
{
    platform_idle_handler_t handler;
    handler_arg_t arg;
} idle_data = {NULL, NULL};

void platform_set_idle_handler(platform_idle_handler_t handler, handler_arg_t arg)
{
    idle_data.handler = handler;
    idle_data.arg = arg;
}
void vApplicationIdleHook(xTaskHandle *pxTask, signed portCHAR *pcTaskName)
{
    // Call handler if any
    if (idle_data.handler)
    {
        if (idle_data.handler(idle_data.arg))
        {
            // Do not halt the CPU
            return;
        }
    }

#if (defined(KEEP_UART) && (KEEP_UART == 1)) || \
        (defined(TEST_USB) && (TEST_USB)) || \
        (defined(ENABLE_DEBUG) && (ENABLE_DEBUG == 1))
    // Mask interrupts
    asm volatile("cpsid i");

    // Set SLEEP mode if there are underground activities
    pwr_enter_sleep(PWR_VRANGE_2);
    asm volatile("wfi");

    // Unmask interrupts
    asm volatile("cpsie i");
#else

    if (underground_activities)
    {
        // Mask interrupts
        asm volatile("cpsid i");

        // Set SLEEP mode if there are underground activities
        pwr_enter_sleep(PWR_VRANGE_2);
        asm volatile("wfi");

        // Unmask interrupts
        asm volatile("cpsie i");
    }
    else
    {
        // Set LOW POWER SLEEP mode otherwise
        // Mask interrupts
        asm volatile("cpsid i");

        // Check the timers for drift
        platform_drivers_check_timers();

        // Set power settings: Vrange2, low power when sleep
        pwr_enter_low_power_sleep();

        // Set HCLK from MSI (already at right frequency)
        rcc_sysclk_set_i(RCC_SYSCLK_SOURCE_MSI, RCC_SYSCLK_AHB_PRE_1,
                         RCC_SYSCLK_APB_PRE_1, RCC_SYSCLK_APB_PRE_1);

        // Disable the HSI
        rcc_hsi_disable_i();

        // Wait for interrupt
        asm volatile("wfi");

        // Set MSI 4MHz
        rcc_msi_update_frequency_i(RCC_MSI_FREQ_4194kHz, 0x00);

        // Enable HSI
        rcc_hsi_enable_i();

        // Select HSI
        rcc_sysclk_set_i(RCC_SYSCLK_SOURCE_HSI, RCC_SYSCLK_AHB_PRE_1,
                         RCC_SYSCLK_APB_PRE_2, RCC_SYSCLK_APB_PRE_2);

        // Set MSI low frequency
        rcc_msi_update_frequency_i(RCC_MSI_FREQ_131kHz, TRIM);

        // Unmask interrupts
        asm volatile("cpsie i");
    }

#endif // KEEP_UART
}
