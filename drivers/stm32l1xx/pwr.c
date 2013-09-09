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
 * Copyright (C) 2012 HiKoB.
 */

/*
 * pwr.c
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "pwr.h"
#include "pwr_registers.h"
#include "cm3_scb_registers.h"

void pwr_main_mode(pwr_vrange_t vrange)
{
    // Set given voltage range , no low power bit
    *pwr_get_CR() = (vrange << 11);

    // Wait until voltage  stabilized
    while (*pwr_get_CSR() & PWR_CSR__VOSF)
    {
        ;
    }
}

void pwr_enter_low_power_run()
{
    // Set low power bits
    *pwr_get_CR() = PWR_CR__LPRUN | PWR_CR__VOS_RANGE2 | PWR_CR__FWU
                    | PWR_CR__ULP | PWR_CR__LPSDSR;

    // Wait until voltage  stabilized
    while (*pwr_get_CSR() & PWR_CSR__VOSF)
    {
        ;
    }
}

void pwr_enter_sleep(pwr_vrange_t vrange)
{
    // Set given voltage range 1, no low power bit
    *pwr_get_CR() = (vrange << 11);

    // Wait until voltage  stabilized
    while (*pwr_get_CSR() & PWR_CSR__VOSF)
    {
        ;
    }

    // Clear CM3 SLEEPDEEP
    *cm3_scb_get_SCR() &= ~CM3_SCB_SCR__SLEEPDEEP;
}

void pwr_enter_low_power_sleep()
{
    // Set low power bits, voltage range 2
    *pwr_get_CR() = PWR_CR__VOS_RANGE2 | PWR_CR__FWU | PWR_CR__ULP
                    | PWR_CR__LPSDSR;

    // Wait until voltage  stabilized
    while (*pwr_get_CSR() & PWR_CSR__VOSF)
    {
        ;
    }

    // Clear SLEEPDEEP in SCB
    *cm3_scb_get_SCR() &= ~CM3_SCB_SCR__SLEEPDEEP;
}
void pwr_enter_low_power_sleep_keep_vrefint()
{
    // Set voltage range 2 with LPSDR only
    *pwr_get_CR() = PWR_CR__VOS_RANGE2 | PWR_CR__LPSDSR;

    // Clear SLEEPDEEP in SCB
    *cm3_scb_get_SCR() &= ~CM3_SCB_SCR__SLEEPDEEP;
}
void pwr_enter_stop()
{
    // Set low power bits
    *pwr_get_CR() = PWR_CR__VOS_RANGE2 | PWR_CR__FWU | PWR_CR__ULP
                    | PWR_CR__LPSDSR;

    // Set SLEEPDEEP
    *cm3_scb_get_SCR() = CM3_SCB_SCR__SLEEPDEEP;

    // Wait until voltage  stabilized
    while (*pwr_get_CSR() & PWR_CSR__VOSF)
    {
        ;
    }
}

void pwr_enter_standby()
{
    // Set low power bits, clear flags
    *pwr_get_CR() = PWR_CR__VOS_RANGE2 | PWR_CR__CSBF | PWR_CR__CWUF
                    | PWR_CR__PDDS | PWR_CR__LPSDSR;

    // Set SLEEPDEEP
    *cm3_scb_get_SCR() = CM3_SCB_SCR__SLEEPDEEP;

    // Wait until voltage  stabilized
    while (*pwr_get_CSR() & PWR_CSR__VOSF)
    {
        ;
    }
}

/* **** Write Protection **** */

void pwr_disable_backup_write_protection()
{
    // Set the DBP bit
    *pwr_get_CR() |= PWR_CR__DBP;
}

void pwr_enable_backup_write_protection()
{
    // Clear the DBP bit
    *pwr_get_CR() &= ~PWR_CR__DBP;
}
