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

/** LPS331AP driver
 * \file lps331ap.h
 * \date Jan 25, 2012
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

/** \addtogroup periph
 * @{
 */

/** \defgroup lps331ap LPS331AP sensor
 * The LPS331AP sensor is a pressure sensor. This group
 * defines the configuration function and the data access function of this sensor.
 *
 * \see \ref example_lps331ap_polling.c for an example.
 *
 * @{
 */

#ifndef LPS331AP_H_
#define LPS331AP_H_

#include <stdint.h>
#include <stdbool.h>
#include "handler.h"

/** Available pressure and temperature sampling rate */
typedef enum
{
	LPS331AP_ONE_SHOT            = 0x00,
	LPS331AP_P_1HZ_T_1HZ         = 0x10,
	LPS331AP_P_7HZ_T_1HZ         = 0x20,
	LPS331AP_P_12_5HZ_T_1HZ      = 0x30,
	LPS331AP_P_25HZ_T_1HZ        = 0x40,
	LPS331AP_P_7HZ_T_7HZ         = 0x50,
	LPS331AP_P_12_5HZ_T_12_5HZ   = 0x60,
	LPS331AP_P_25HZ_T_25HZ       = 0x70,
} lps331ap_datarate_t;

/** Read WHOAMI register
 * This functions returns the content of the WHOAMI register. It allows chip type identification.
 * \return The content of the WHOAMI register
 */
uint8_t lps331ap_read_whoami();

/** Read CTRL_REGi
 * This functions returns the content of the i-th control register
 * \param i The index of the control register to read. The allowed value are between 1 and 3, other values give unspecified results
 * \return The content of the control register
 */
uint8_t lps331ap_read_crtl_reg(uint8_t i);

/** Switch off sensor
 * \return 0 if there was no error, non zero value if something went wrong during reading, false otherwise
 */
uint8_t lps331ap_powerdown();

/** Data rate configuration 
 * This function configures the data rate of the sensor.
 * \param datarate The pressure and temperature sensors sampling data rate (from 1 to 25Hz see \ref lps331ap_datarate_t)
 * \warning The 25Hz sampling rate does not seem to work on this sensor
 * \return 0 if there was no error, non zero value if something went wrong during reading, false otherwise
 */
uint8_t lps331ap_set_datarate(lps331ap_datarate_t datarate);

/** Pressure reading
 * Raw pressure output reading. The measurement has to be multiplied by the resolution
 * to compute a real value. To get the value in milli-bar, the value has to be divided by 4096 (i.e. shifted 12 bits to the right).
 * For example a value of 4096000 corresponds to 1 bar.
 * \param p The valid address of an uint32_t value. This value is actually coded on 24 bits
 * \return 0 if there was no error, non zero value if something went wrong during reading, false otherwise
 */
uint8_t lps331ap_read_pres(uint32_t *p);

/** Temperature reading
 * Raw temperature reading. The measurement has to be multiplied by the resolution and an offset has to be added.
 * The formula that gives the temperature une °C is T(°C) = 42.5 + temp / 480
 * \warning The temperature conversion formula given in ST's datasheet does not seen right, at least the offset is not 42.5°C
 * \param *temp The valid address of an int16_t value.
 * \return 0 if there was no error, non zero value if something went wrong during reading, false otherwise
 */
uint8_t lps331ap_read_temp( int16_t *temp);

/** Configure the data ready interrupts
 * This function sets the interrupt handler that is called when a temperature or pressure data
 * is available for reading.
 * \param data_ready_handler The interrupt handler. If it is set to NULL, the interrupt is disabled.
 * \param data_ready_arg The parameter to the handler. This parameter may be NULL.
 */
void lps331ap_set_drdy_int(handler_t data_ready_handler, handler_arg_t data_ready_arg);

/** @} */

/** @} */

#endif /* LPS331_H_ */
