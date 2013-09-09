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
