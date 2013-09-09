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
 * pwr.h
 *
 *  Created on: May 28, 2013
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef PWR_H_
#define PWR_H_

#include "pwr_registers.h"
#include "cm3_scb_registers.h"
#include "rcc.h"

static inline void pwr_enable_backup_write_protection()
{
    rcc_apb_enable(RCC_APB_BUS_PWR, RCC_APB_BIT_PWR);
    rcc_apb_enable(RCC_APB_BUS_BKP, RCC_APB_BIT_BKP);

    // Set DBP bit in PWR_CR
    *pwr_get_CR() |= PWR_CR__DBP;

}
static inline void pwr_disable_backup_write_protection()
{
    // Clear DBP bit in PWR_CR
    *pwr_get_CR() &= ~PWR_CR__DBP;
}

static inline void pwr_enter_sleep()
{
    // Clear SLEEPDEEP
    *cm3_scb_get_SCR() = 0;
    asm volatile("wfi");
}

static inline void pwr_enter_stop()
{
    // Set SLEEPDEEP
    *cm3_scb_get_SCR() = CM3_SCB_SCR__SLEEPDEEP;

    // Clear PDDS
    *pwr_get_CR() &= ~PWR_CR__PDDS;
    asm volatile("wfi");
}
#endif /* PWR_H_ */
