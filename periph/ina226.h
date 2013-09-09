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
 * \file ina226.h
 * INA226 header file
 * \date Jul 12, 2012
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef INA226_H_
#define INA226_H_

/**
 * \addtogroup periph
 * @{
 */

/**
 * \defgroup ina226 INA226 power measurement
 *
 * This provides all the methods and types required to control the INA226
 * power measurement sensor.
 *
 * The INA226 is a current measurement sensor, which reads a voltage across a
 * shunt resistor to compute the current.
 *
 * @{
 */

#include <stdint.h>
#include "handler.h"

/**
 * Calibrate the INA226.
 *
 * Calibrate the INA226 based on the shunt resistor and the max expected current.
 * This will compute the optimum settings and write them to the chip registers.
 *
 * \param r_shunt the shunt resistor value
 * \param max_current the expected maximum current to flow through the resistor
 */
void ina226_calibrate(float r_shunt, float max_current);

/**
 * Possible sampling periods.
 */
typedef enum
{
    INA226_PERIOD_140us = 0,
    INA226_PERIOD_204us = 1,
    INA226_PERIOD_332us = 2,
    INA226_PERIOD_588us = 3,
    INA226_PERIOD_1100us = 4,
    INA226_PERIOD_2116us = 5,
    INA226_PERIOD_4156us = 6,
    INA226_PERIOD_8244us = 7,
} ina226_sampling_period_t;

/** Possible averaging values */
typedef enum
{
    INA226_AVERAGE_1 = 0,
    INA226_AVERAGE_4 = 1,
    INA226_AVERAGE_16 = 2,
    INA226_AVERAGE_64 = 3,
    INA226_AVERAGE_128 = 4,
    INA226_AVERAGE_256 = 5,
    INA226_AVERAGE_512 = 6,
    INA226_AVERAGE_1024 = 7,
} ina226_averaging_factor_t;

/**
 * Configure the INA226 for sampling.
 *
 * This selects the sampling rate and average factor of the chip.
 *
 * \param period the sampling period
 * \param average the averaging factor
 */
void ina226_configure(ina226_sampling_period_t period,
        ina226_averaging_factor_t average);

/**
 * Disable the INA226.
 *
 * This places the INA226 in power down
 */
void ina226_disable();

/**
 * Enable the ALERT pin to generate interrupts on each data ready
 *
 * \param handler the handler to be called on data ready
 * \param arg an optional argument to the handler
 */
void ina226_alert_enable(handler_t handler, handler_arg_t arg);
/**
 * Disable the ALERT pin to generate interrupts on each data ready
 */
void ina226_alert_disable();

/**
 * Read the sampled values.
 *
 * This reads the values from the INA226 registers and convert them to SI
 * units according to the calibration values.
 *
 * The availability of a new set of values may be checked with \ref
 * ina226_conversion_ready.
 *
 * \param voltage a pointer to store the bus voltage, in V, or NULL if not requested
 * \param current a pointer to store the load current, in A, or NULL if not requested
 * \param power a pointer to store the load power, in W, or NULL if not requested
 * \param shunt_voltage a pointer to store the shunt voltage, in V, or NULL if not requested
 */
void ina226_read(float *voltage, float *current, float *power, float* shunt_voltage);

/**
 * Check if a new set of sample is available.
 *
 * This reads a register of the INA226 to check if a new set of samples is
 * available for reading through \ref ina226_read.
 *
 * \return 1 if a new set is available for reading, 0 if not.
 */
int ina226_conversion_ready();

/**
 * @}
 * @}
 */

#endif /* INA226_H_ */
