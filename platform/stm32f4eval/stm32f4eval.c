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
 * stm32f4eval.c
 *
 *  Created on: Sep 17, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "stm32f4eval.h"

#include "watchdog.h"
#include "cm3_scb_registers.h"
#include "rcc.h"
#include "flash.h"
#include "uart_.h"
#include "unique_id.h"
#include "random.h"
#include "soft_timer.h"

#include "printf.h"
#include "debug.h"

__attribute__((weak)) int32_t platform_should_start_watchdog();

void platform_init()
{
    // Enable watchdog if requested
    if (platform_should_start_watchdog && platform_should_start_watchdog())
    {
        watchdog_enable(WATCHDOG_DIVIDER_256, 0xFFF);
    }

    // Enable floating point
    *cm3_scb_get_CPACR() |= 0xF << 20;  /* set CP10 and CP11 Full Access */

    // Enable syscfg
    rcc_apb_enable(RCC_APB_BUS_SYSCFG, RCC_APB_BIT_SYSCFG);

    // At reset, HCLK is 16MHz, set flash latency to 5 wait state to handle 168MHz
    flash_set_latency(5);

    // Configure PLL to have 336MHz VCO, then 48MHz for USB and 168 MHz for sysclk
    rcc_pll_enable(RCC_PLL_SOURCE_HSI, 8, 168, RCC_PLL_MAIN_DIV_2, 7);

    // Now SYSCLK is at 168MHz, set AHB divider to 1, APB1 to 4 and APB2 to 4
    /*
     *  The frequency of the AHB domain is 168 MHz.
     *  The frequency of the APBx domain is 42 MHz.
     */
    rcc_sysclk_set_prescalers(RCC_SYSCLK_AHB_PRE_1, RCC_SYSCLK_APB_PRE_4,
            RCC_SYSCLK_APB_PRE_4);

    // Select PLL as SYSCLK source clock
    rcc_sysclk_select_source(RCC_SYSCLK_SOURCE_PLL);

    // Setup the drivers
    platform_drivers_setup();

    // Setup the LEDs
    platform_leds_setup();

    // Setup the libraries
    platform_lib_setup();

    // Setup the peripherals
    platform_periph_setup();

    // Setup the net stack
    platform_net_setup();

    // Feed the random number generator
    random_init(uid->uid32[2]);

    log_printf(
            "HCLK @%uMHz, SYSTICK @%uMHz", rcc_sysclk_get_clock_frequency(RCC_SYSCLK_CLOCK_HCLK) / 1000000,
            rcc_sysclk_get_clock_frequency(RCC_SYSCLK_CLOCK_SYSTICK_CLK) / 1000000);

    log_printf("\n\nPlatform starting in ");
    uint32_t i;

    for (i = 1; i > 0; i--)
    {
        log_printf("%u... ", i);
        soft_timer_delay_s(1);
    }
    log_printf("\nGO!\n");
}

void platform_start_freertos_tick(uint16_t frequency, handler_t handler,
        handler_arg_t arg)
{
    // For now, until the RTC works, start the systick.
    nvic_enable_systick(frequency, handler, arg);
}
