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

/** L3G4200D driver
 * \file l3g4200d.h
 * \date Jul 31, 2011
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

/** \addtogroup periph
 * @{
 */

/** \defgroup l3g4200 L3G4200D sensor
 * The L3G4200D sensor is a 3-axis gyroscope. This group
 * defines the configuration function and the data access function of this sensor.
 *
 * \see \ref example_l3g4200d_polling.c and \ref example_l3g4200d_interrupt.c
 * for examples.
 *
 * @{
 */

#ifndef L3G4200D_H_
#define L3G4200D_H_

#include <stdbool.h>
#include <stdint.h>
#include "handler.h"

/** Available magnetometer rate
 */
typedef enum
{
    L3G4200D_100HZ = 0x00,
    L3G4200D_200HZ = 0x40,
    L3G4200D_400HZ = 0x80,
    L3G4200D_800HZ = 0xC0
} l3g4200d_datarate_t;

/** Available gyroscope scale */
typedef enum
{
    L3G4200D_250DPS  = 0x00,
    L3G4200D_500DPS  = 0x10,
    L3G4200D_2000DPS = 0x30
} l3g4200d_scale_t;

/** Read WHOAMI register
 * This functions returns the content of the WHOAMI register. It allows chip type identification.
 * \return The content of the WHOAMI register
 */
uint8_t l3g4200d_read_whoami();

/** Read CTRL_REGi
 * This functions returns the content of the i-th control register
 * \param i The index of the control register to read. The allowed value are between 1 and 5, other values give unspecified results
 * \return The content of the control register
 */
uint8_t l3g4200d_read_crtl_reg(uint8_t i);

/** Switch off sensor
 * \return 0 if there was no error, non zero value if something went wrong during reading, false otherwise
 */
uint8_t l3g4200d_powerdown();

/** Sensor configuration 
 * This function configures the data rate and the sensitivity of the gyroscope.
 * \param datarate The gyroscope data rate (from 100 to 800Hz see \ref l3g4200d_datarate_t)
 * \param scale The gyroscope full scale (from +/-250°/s to +/-2000°/s see \ref l3g4200d_scale_t)
 * \param bdu Enable the Block Data Update feature. This feature ensure that both data LSB and MSB
 *            are consistent when reading rotation speed asynchronously
 * \return 0 if there was no error, non zero value if something went wrong during reading, false otherwise
 */
uint8_t l3g4200d_gyr_config(l3g4200d_datarate_t datarate, l3g4200d_scale_t scale, bool bdu);


/** Rotation speed reading
 * Non-blocking call if the given handler is non-NULL.
 * In that case the handler is called when the speed reading is done.
 *
 * Raw rotation speed output reading. The measurement has to be multiplied by the resolution
 * to compute a real value. For example a reading of {352, 11, -528} with the +/-250°/s scale
 * has to be multiplied by 8.75e-3 (according to STs datasheet) to get the rotation speed in °/s.
 * In this case the rotation speed is {3.02°/s, 0.09625°/s, -4.62°/s}.
 *
 * \note The handler is called from interrupt context.
 * \note The gyroscope has bias inherent in the manufacturing process, it must be taken into account when dealing with this sensor
 * \param rot_speed The valid address of an array of 3 int16_t values. The first one is the X axis, the second one the Y axis and the last one the Z axis
 * \return 0 if there was no error, non zero value if something went wrong during reading
 */
unsigned l3g4200d_read_rot_speed_async(int16_t *rot_speed, result_handler_t handler, handler_arg_t arg);

/** Rotation speed reading
 * Blocking call.
 *
 * \param rot_speed The valid address of an array of 3 int16_t values. The first one is the X axis, the second one the Y axis and the last one the Z axis
 * \return 0 if there was no error, non zero value if something went wrong during reading
 */
static inline unsigned l3g4200d_read_rot_speed(int16_t *rot_speed)
{
    return l3g4200d_read_rot_speed_async(rot_speed, NULL, NULL);
}

/** Temperature reading
 * Read the sensor internal temperature. This can be used to compensate sensor sentivity and
 * biais variation over temperature.
 * \param temp The temperature the sensitivity is -1K/LSB the offset is not specified in the
 *             datasheet but experiments show that it is around 40°C
 * \return 0 if there was no error, non zero value if something went wrong during reading, false otherwise
 */
uint8_t l3g4200d_read_temp(uint8_t *temp);

/** Read DRDY pin status
 * This function reads the interrupt DRDY pin
 * \return The pins state (true if DRDY is 1, false otherwise)
 */
bool l3g4200d_read_drdy();

/** Configure the data ready interrupts
 * This function sets the interrupt handler that is called when a gyroscope data
 * is available for reading.
 * \param data_ready_handler The interrupt handler. If it is set to NULL, the interrupt is disabled.
 * \param data_ready_arg The parameter to the handler. This parameter may be NULL.
 */
void l3g4200d_set_drdy_int(handler_t data_ready_handler, handler_arg_t data_ready_arg);

/** TODO */
void l3g4200d_set_bw(uint8_t bw);

/** TODO */
void l3g4200d_set_out(uint8_t out);

/** @} */

/** @} */

#endif /* L3G4200D_H_ */
