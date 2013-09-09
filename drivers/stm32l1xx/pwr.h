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
 * pwr.h
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef PWR_H_
#define PWR_H_
typedef enum
{
    PWR_VRANGE_1 = 0x1,
    PWR_VRANGE_2 = 0x2,
    PWR_VRANGE_3 = 0x3,
} pwr_vrange_t;

/**
 * Set the voltage regulator in normal main mode, with a specified voltage
 * range.
 *
 * \param vrange the voltage range to set.
 */
void pwr_main_mode(pwr_vrange_t vrange);

/**
 * Set the power control in low power run mode.
 *
 * The system frequency must be below 128kHz.
 *
 */
void pwr_enter_low_power_run();
/**
 * Set the SLEEP mode.
 *
 * The wfi instruction must be called to enter this mode.
 *
 * \param vrange the voltage range to set.
 */
void pwr_enter_sleep(pwr_vrange_t vrange);
/**
 * Set the voltage regulator in low power mode.
 *
 * The wfi instruction must be called to enter this mode.
 */
void pwr_enter_low_power_sleep();
/**
 * Set the voltage regulator in low power mode, but keep Vrefint enabled.
 *
 * The wfi instruction must be called to enter this mode.
 */
void pwr_enter_low_power_sleep_keep_vrefint();
/**
 * Set the STOP mode.
 *
 * The wfi instruction must be called to enter this mode.
 */
void pwr_enter_stop();
/**
 * Set the STANDBY mode.
 *
 * The wfi instruction must be called to enter this mode.
 */
void pwr_enter_standby();

/**
 * Disable the backup write protection in the PWR peripheral, to allow
 * configuring the LSE clock;
 */
void pwr_disable_backup_write_protection();
/**
 * Enable back the write protection.
 */
void pwr_enable_backup_write_protection();

#endif /* PWR_H_ */
