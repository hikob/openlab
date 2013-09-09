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

/** LSM303DLHC driver
 * \file lsm303dlhc.h
 * \date Aug 3, 2011
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

/** \addtogroup periph
 * @{
 */

/** \defgroup lsm303 LSM303DLHC sensor
 * The LSM303DLHC sensor is a 3-axis accelerometer and magnetometer. This group
 * defines the configuration function and the data access function of this sensor.
 *
 * \see \ref example_lsm303dlhc_polling.c for example.
 *
 * @{
 */

#ifndef LSM303DLHC_H_
#define LSM303DLHC_H_

#include <stdbool.h>
#include <stdint.h>
#include "handler.h"

/** Available accelerometer rate
 * All the data rates are available in normal and low power mode, except 1620Hz mode
 * that is only available in low power mode, the 1344Hz/5376Hz mode is available in
 * both modes. The sampling rate is set to 1344Hz in normal mode and to 5376Hz in low
 * power mode
 */
typedef enum
{
    LSM303DLHC_ACC_RATE_1HZ = 0x10,
    LSM303DLHC_ACC_RATE_10HZ = 0x20,
    LSM303DLHC_ACC_RATE_25HZ = 0x30,
    LSM303DLHC_ACC_RATE_50HZ = 0x40,
    LSM303DLHC_ACC_RATE_100HZ = 0x50,
    LSM303DLHC_ACC_RATE_200HZ = 0x60,
    LSM303DLHC_ACC_RATE_400HZ = 0x70,
    LSM303DLHC_ACC_RATE_1620HZ_LP = 0x80,
    LSM303DLHC_ACC_RATE_1344HZ_N_5376HZ_LP = 0x90
} lsm303dlhc_acc_datarate_t;

/** Available magnetometer rate */
typedef enum
{
    LSM303DLHC_MAG_RATE_0_75HZ = 0x00,
    LSM303DLHC_MAG_RATE_1_5HZ = 0x04,
    LSM303DLHC_MAG_RATE_3HZ = 0x08,
    LSM303DLHC_MAG_RATE_7_5HZ = 0x0C,
    LSM303DLHC_MAG_RATE_15HZ = 0x10,
    LSM303DLHC_MAG_RATE_30HZ = 0x14,
    LSM303DLHC_MAG_RATE_75HZ = 0x18,
    LSM303DLHC_MAG_RATE_220HZ = 0x1C
} lsm303dlhc_mag_datarate_t;

/** Available accelerometer scale
 * The available scales are +/-2G, +/-4G, +/-8G and +/-16G.
 * According to STs datasheet (this is verified experimentally) the real scales
 * are +/-2.048G, +/-4.096G, +/-8.192G and +/-24.576G. This last value is far
 * from the announced scale, but the resolution is consistent with a +/-24G scale.
 */
typedef enum
{
    LSM303DLHC_ACC_SCALE_2G = 0x00,
    LSM303DLHC_ACC_SCALE_4G = 0x10,
    LSM303DLHC_ACC_SCALE_8G = 0x20,
    LSM303DLHC_ACC_SCALE_16G = 0x30
} lsm303dlhc_acc_scale_t;

/** Available data register update mode */
typedef enum
{
    LSM303DLHC_ACC_UPDATE_CONTINUOUS = 0x00,
    LSM303DLHC_ACC_UPDATE_ON_READ = 0x80,
} lsm303dlhc_acc_update_t;

/** Available magnetometer scale */
typedef enum
{
    LSM303DLHC_MAG_SCALE_1_3GAUSS = 0x20,
    LSM303DLHC_MAG_SCALE_1_9GAUSS = 0x40,
    LSM303DLHC_MAG_SCALE_2_5GAUSS = 0x60,
    LSM303DLHC_MAG_SCALE_4_0GAUSS = 0x80,
    LSM303DLHC_MAG_SCALE_4_7GAUSS = 0xA0,
    LSM303DLHC_MAG_SCALE_5_6GAUSS = 0xC0,
    LSM303DLHC_MAG_SCALE_8_1GAUSS = 0xE0
} lsm303dlhc_mag_scale_t;

/** Magnetometer operation mode */
typedef enum
{
    LSM303DLHC_MAG_MODE_CONTINUOUS = 0x00,
    LSM303DLHC_MAG_MODE_SINGLE = 0x01,
    LSM303DLHC_MAG_MODE_OFF = 0x02,
} lsm303dlhc_mag_mode_t;

/** Temperature sensor operation mode */
typedef enum
{
    LSM303DLHC_TEMP_MODE_ON = 0x80,
    LSM303DLHC_TEMP_MODE_OFF = 0x00,
} lsm303dlhc_temp_mode_t;

/** Switch off the sensor
 * This function stops the accelerometer, the magnetometer and the temperature sensor
 * \return 0 if there was no error, non zero value if something went wrong during reading, false otherwise
 */
uint8_t lsm303dlhc_powerdown();

/** \name Magnetometer related functions */
/** @{ */

/** Magnetometer register read
 * \param reg The register
 * \param val A pointer to a uint8_t to store the register value
 * \return 0 on success
 */
unsigned lsm303dlhc_mag_reg_read(uint8_t reg, uint8_t *val);

/** Magnetometer register write
 * \param reg The register
 * \param val The value to write
 * \return 0 on success
 */
unsigned lsm303dlhc_mag_reg_write(uint8_t reg, uint8_t val);

/** Magnetometer and temperature sensor configuration
 * This function configures the data rate and the sensitivity of the magnetometer. It also
 * sets the magnetometer and temperature sensor mode of operation.
 * \param datarate The magnetometer data rate (from 0.75Hz to 220Hz see \ref lsm303dlhc_mag_datarate_t)
 * \param scale The magnetometer full scale (from +/-1.3 gauss to +/-8.1 gauss see \ref lsm303dlhc_mag_scale_t)
 * \param mode The magnetometer sampling mode. It can be off, continuous (according the data rate) or single shot
 * \param temp_mode The LSM303DLHC internal temperature sensor mode of operation (on or off)
 * \return 0 if there was no error, non zero value if something went wrong during reading, false otherwise
 * \see \ref example_lsm303dlhc_polling.c for example
 */
uint8_t lsm303dlhc_mag_config(lsm303dlhc_mag_datarate_t datarate,
                           lsm303dlhc_mag_scale_t scale, lsm303dlhc_mag_mode_t mode,
                           lsm303dlhc_temp_mode_t temp_mode);

/** Configure the data ready interrupt
 * This function sets the interrupt handler that is called when a magnetometer data is available
 * for reading.
 * \param data_ready_handler The interrupt handler. If it is set to NULL, the interrupt is disabled, otherwise, when data is available the handler is called.
 * \param data_ready_arg The parameter to the handler. This parameter may be NULL.
 */
void lsm303dlhc_mag_set_drdy_int(handler_t data_ready_handler,
                                 handler_arg_t data_ready_arg);

/** Read DRDY pin status
 * This function reads the interrupt DRDY pin
 * \return The pins state (true if DRDY is 1, false otherwise)
 */
int lsm303dlhc_mag_get_drdy_pin_value();

/** Request a single conversion
 * This function requests a single magnetometer sample. The sample is not directly available
 * after the call of this function. The data can be read using the lsm303dlhc_read_mag() method
 * only after the conversion is ended, otherwise the previous value will be read.
 * \return 0 if there was no error, non zero value if something went wrong during reading, false otherwise
 */
uint8_t lsm303dlhc_mag_sample_single();

/** Compute the closest valid data rate to a given sampling frequency
 * Find the closest valid data rate less than the given frequency.
 * \param freq The frequency to approximate in Hz
 * \return The closest valid configuration (see \ref lsm303dlhc_mag_datarate_t)
 */
lsm303dlhc_mag_datarate_t lsm303dlhc_mag_compute_datarate(uint32_t freq);

/** Magnetic field reading
 * Raw magnetometer output reading. The measurement has to be multiplied by the resolution
 * to compute a real value. For example a reading of {352, 11, -528} with the +/-1.3gauss scale
 * has to be multiplied by 1/1100 (according to STs datasheet) to get the magnetic field in gauss.
 * In this case the real magnetic field is {0.32, 0.01, -0.48} which is roughly the earth magnetic
 * field measured in France when the X axis is pointing to the North and the Z axis is pointing up.
 *
 * The call is non-blocking only if handler is non-NULL.
 * In that case, handler is called when the reading is done.
 * lsm303dlhc_read_mag_async_finalize must then be called on mag array.
 *
 * \warning The sensor Z axis has not the same sensitivity as X and Y axes, this method rescales the Z value so that it has the same scale as X and Y values.
 * \note The magnetometer has an offset inherent in the manufacturing process, it must be taken into account when dealing with this sensor
 * \note handler is called from interrupt context.
 *
 * \param mag The valid address of an array of 3 int16_t values. The first one is the X axis, the second one the Y axis and the last one the Z axis
 * \param handler The handler to call when it is done.
 * \param handler_arg An argument given to the handler.
 * \return 0 if there was no error, non zero value if something went wrong during reading
 */
unsigned lsm303dlhc_read_mag_async(int16_t *mag,
        result_handler_t handler, handler_arg_t handler_arg);

/** Blocking magnetic field reading
 * \param mag The valid address of an array of 3 int16_t values.
 * \return 0 if there was no error, non zero value if something went wrong during reading
 */
static inline unsigned lsm303dlhc_read_mag(int16_t *mag)
{
    return lsm303dlhc_read_mag_async(mag, NULL, NULL);
}

/** Non-blocking acceleration reading finalization
 * This function must be called only after a non-blocking acceleration reading.
 *
 * \param mag The address of int16_t array given to lsm303dlhc_read_mag_async
 */
void lsm303dlhc_read_mag_async_finalize(int16_t *mag);

/** @} */

/** \name Accelerometer related functions */
/** @{ */

/** Accelerometer register read
 * \param reg The register
 * \param val A pointer to a uint8_t to store the register value
 * \return 0 on success
 */
unsigned lsm303dlhc_acc_reg_read(uint8_t reg, uint8_t *val);

/** Accelerometer register write
 * \param reg The register
 * \param val The value to write
 * \return 0 on success
 */
unsigned lsm303dlhc_acc_reg_write(uint8_t reg, uint8_t val);

/** Accelerometer configuration
 *
 * \warning in continuous update mode, corrupted data might be read (datasheet unclear)
 * \note on-read update mode is the safest, but outdated data may be read.
 *
 * \param datarate The datarate of reading
 * \param scale The measure full-scale
 * \param update The update mode for the data register
 * \return 0 if there was no error, non zero value if something went wrong during reading
 */
uint8_t lsm303dlhc_acc_config(lsm303dlhc_acc_datarate_t datarate,
                           lsm303dlhc_acc_scale_t scale, lsm303dlhc_acc_update_t update);

/** Configure the interrupt INT1
 * This function sets the INT1 interrupt handler
 * \param int1_handler The function to be called when INT1 is issued
 * \param int1_arg The handler parameter
 */
void lsm303dlhc_acc_set_drdy_int1(handler_t int1_handler,
                                 handler_arg_t int1_arg);

/** Read the actual pin status of DRDY INT1 */
int lsm303dlhc_acc_get_drdy_int1_pin_value();

/** Configure the interrupt INT2
 * This function sets the INT2 interrupt handler
 * \param int2_handler The function to be called when INT2 is issued
 * \param int2_arg The handler parameter
 */
void lsm303dlhc_acc_set_drdy_int2(handler_t int2_handler,
                                 handler_arg_t int2_arg);
/** Acceleration reading
 * Raw accelerometer output reading. The measurement has to be multiplied by the resolution
 * to compute a real value. For example a reading of {3, 12, 250} with the +/-8g resolution
 * has to be multiplied by 4e-3 (according to STs datasheet) to get the acceleration in g.
 * In this case the real acceleration is {0.012, 0.048, 1} which is roughly the gravity measured
 * when the Z axis of the sensor is looking up.
 *
 * The call is non-blocking only if handler is non-NULL.
 * In that case, handler is called when the reading is done.
 * lsm303dlhc_read_acc_async_finalize must then be called on acc.
 *
 * \note handler is called from interrupt context.
 * \warning in continuous update mode, the 4 lsb of each axis might be invalid.
 *
 * \param acc The valid address of an array of 3 int16_t values. The first one is the X axis, the second one the Y axis and the last one the Z axis
 * \param handler The handler to call when it is done.
 * \param handler_arg An argument given to the handler.
 */
unsigned lsm303dlhc_read_acc_async(int16_t *acc,
        result_handler_t handler, handler_arg_t handler_arg);

/** Blocking acceleration reading
 * \param acc The valid address of an array of 3 int16_t values.
 * \return 0 if there was no error, non zero value if something went wrong during reading
 */
static inline unsigned lsm303dlhc_read_acc(int16_t *acc)
{
    return lsm303dlhc_read_acc_async(acc, NULL, NULL);
}

/** Non-blocking acceleration reading finalization
 * This function must be called only after a non-blocking acceleration reading.
 *
 * \param acc The address of int16_t array given to lsm303dlhc_read_acc_async
 */
void lsm303dlhc_read_acc_async_finalize(int16_t *acc);


/** @} */

/** Temperature reading
 * Read the temperature sensor output
 * \param temp The output temperature. The temperature in °C is obtained by dividing it by 256
 * \return 0 if there was no error, non zero value if something went wrong during reading, false otherwise
 */
uint8_t lsm303dlhc_read_temp(int16_t *temp);

enum
{
    LSM303DLHC_REG_CTRL_REG1_A = 0x20,
    LSM303DLHC_REG_CTRL_REG2_A = 0x21,
    LSM303DLHC_REG_CTRL_REG3_A = 0x22,
    LSM303DLHC_REG_CTRL_REG4_A = 0x23,
    LSM303DLHC_REG_CTRL_REG5_A = 0x24,
    LSM303DLHC_REG_CTRL_REG6_A = 0x25,
    LSM303DLHC_REG_REFERENCE_A = 0x26,
    LSM303DLHC_REG_STATUS_REG_A = 0x27,
    LSM303DLHC_REG_OUT_X_L_A = 0x28,
    LSM303DLHC_REG_OUT_X_H_A = 0x29,
    LSM303DLHC_REG_OUT_Y_L_A = 0x2A,
    LSM303DLHC_REG_OUT_Y_H_A = 0x2B,
    LSM303DLHC_REG_OUT_Z_L_A = 0x2C,
    LSM303DLHC_REG_OUT_Z_H_A = 0x2D,
    LSM303DLHC_REG_FIFO_CTRL_REG_A = 0x2E,
    LSM303DLHC_REG_FIFO_SRC_REG_A = 0x2F,
    LSM303DLHC_REG_INT1_CFG_A = 0x30,
    LSM303DLHC_REG_INT1_SRC_A = 0x31,
    LSM303DLHC_REG_INT1_THS_A = 0x32,
    LSM303DLHC_REG_INT1_DURATION_A = 0x33,
    LSM303DLHC_REG_INT2_CFG_A = 0x34,
    LSM303DLHC_REG_INT2_SRC_A = 0x35,
    LSM303DLHC_REG_INT2_THS_A = 0x36,
    LSM303DLHC_REG_INT2_DURATION_A = 0x37,
    LSM303DLHC_REG_CLICK_CFG_A = 0x38,
    LSM303DLHC_REG_CLICK_SRC_A = 0x39,
    LSM303DLHC_REG_CLICK_THR_A = 0x3A,
    LSM303DLHC_REG_TIME_LIMIT_A = 0x3B,
    LSM303DLHC_REG_TIME_LATENCY_A = 0x3C,
    LSM303DLHC_REG_TIME_WINDOW_A = 0x3D
};

enum
{
    LSM303DLHC_REG_CRA_REG_M = 0x00,
    LSM303DLHC_REG_CRB_REG_M = 0x01,
    LSM303DLHC_REG_MR_REG_M = 0x02,
    LSM303DLHC_REG_OUT_X_H_M = 0x03,
    LSM303DLHC_REG_OUT_X_L_M = 0x04,
    LSM303DLHC_REG_OUT_Z_H_M = 0x05,
    LSM303DLHC_REG_OUT_Z_L_M = 0x06,
    LSM303DLHC_REG_OUT_Y_H_M = 0x07,
    LSM303DLHC_REG_OUT_Y_L_M = 0x08,
    LSM303DLHC_REG_SR_REG_M = 0x09,
    LSM303DLHC_REG_IRA_REG_M = 0x0A,
    LSM303DLHC_REG_IRB_REG_M = 0x0B,
    LSM303DLHC_REG_IRC_REG_M = 0x0C,
    LSM303DLHC_REG_TEMP_OUT_H_M = 0x31,
    LSM303DLHC_REG_TEMP_OUT_L_M = 0x32
};

/** @} */

/** @} */

#endif /* LSM303DLHC_H_ */
