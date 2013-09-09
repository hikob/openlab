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
 * lp_nothing.c
 *
 *  Created on: Nov 2, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include "platform.h"
#include "printf.h"

#include "pwr.h"
#include "rcc.h"
#include "flash.h"

int main()
{
    // Initialize the platform
    platform_init();

    // Enter low power run mode

    // Set SYSCLK less than 131kHz from MSI (with trim)
    rcc_msi_enable(RCC_MSI_FREQ_131kHz, 0xF0);

    // Set the Sysclk prescalers
    rcc_sysclk_set(RCC_SYSCLK_SOURCE_MSI, RCC_SYSCLK_AHB_PRE_1,
                   RCC_SYSCLK_APB_PRE_1, RCC_SYSCLK_APB_PRE_1);

    // Disable HSI
    rcc_hsi_disable();

    // Set Flash 0 wait state
    flash_set_0ws();

    // Enter low power run mode
    pwr_enter_low_power_run();

    while (1)
    {
        __asm__ __volatile__("nop");
    }

    return 0;
}

