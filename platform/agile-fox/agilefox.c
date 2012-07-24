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
 * agilefox.c
 *
 *  Created on: Jul 13, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "platform.h"
#include "agilefox.h"

#include "rcc.h"
#include "flash.h"
#include "uart_.h"
#include "unique_id.h"
#include "random.h"
#include "soft_timer.h"

#include "printf.h"

platform_reset_cause_t platform_reset_cause;

void platform_init()
{
    // Store the reset cause
    platform_reset_cause = rcc_get_reset_cause();
    rcc_clear_reset_cause();

    // Set SYSCLK 72MHz from HSE
    rcc_hse_indicate_frequency(16000000);
    rcc_hse_enable();

    // Set 2 wait states for flash access (because SYSCLK is more than 48MHz)
    flash_set_wait_cycle(FLASH_2_CYCLE);

    // Set the Sysclk prescalers: AHB 72MHz, APB2 @ 72MHz and APB1 @ 36MHz
    rcc_sysclk_set_prescalers(RCC_SYSCLK_AHB_PRE_1, RCC_SYSCLK_APB_PRE_2, RCC_SYSCLK_APB_PRE_1);
    rcc_pll_enable(RCC_PLL_SOURCE_HSE, RCC_PLL_MUL_9, RCC_PLL_PREDIV);
    rcc_sysclk_select_source(RCC_SYSCLK_SOURCE_PLL);
    rcc_hsi_disable();

    // enable the LSE
    // TODO: see if LSE is always enabled on STM32F...
    //rcc_lse_enable();

    // Setup the drivers
    platform_drivers_setup();

    // Setup the LEDs
    platform_leds_setup();

    // Setup the button
    platform_button_setup();

    // Enable the interrupts
    asm("cpsie i\n"
        "mov r0, #0\n"
        "msr basepri, r0\n");

    // Setup the libraries
    platform_lib_setup();

    // Enable the interrupts
    asm("cpsie i\n"
        "mov r0, #0\n"
        "msr basepri, r0\n");

    // Setup the peripherals
    platform_periph_setup();

    // Enable the interrupts
    asm("cpsie i\n"
        "mov r0, #0\n"
        "msr basepri, r0\n");
    // Setup the net stack
    platform_net_setup();

    // Enable the interrupts
    asm("cpsie i\n"
        "mov r0, #0\n"
        "msr basepri, r0\n");

    // Restart all timers
    platform_drivers_restart_timers();

    // Feed the random number generator
    random_init(uid->uid32[2]);

    printf("\n\nPlatform starting in ");
    uint32_t i;

    for (i = 1; i > 0; i--)
    {
        printf("%u... ", i);
        soft_timer_delay_s(1);
    }

    // Disable UART if RELEASE and not KEEP UART
#if defined(RELEASE) && (RELEASE == 1) && (!defined(KEEP_UART) || (KEEP_UART == 0))
    printf("\nSilence!\n");
    platform_disable_uart();
#else
    printf("\nGO!\n");
#endif

    // Enable the interrupts
    asm("cpsie i\n"
        "mov r0, #0\n"
        "msr basepri, r0\n");
}

void platform_prevent_low_power() {}
void platform_release_low_power() {}

void xputc(char c)
{
    uart_transfer(uart_print, (uint8_t *) &c, 1);
}
