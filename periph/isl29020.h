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

/** ISL29020 driver
 * \file isl29020.h
 *
 *  \date Dec 3, 2012
 *  \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

/** \addtogroup periph
 * @{
 */

/** \defgroup isl29020 ISL29020 light sensor
 * The ISL29020 sensor is a ambient/IR light sensor. This group
 * defines the configuration function and the data access function of this sensor.
 *
 * \see \ref example_isl29020.c for an example.
 *
 * @{
 */

#ifndef ISL29020_H_
#define ISL29020_H_

/** Available light sources to sample */
typedef enum
{
    ISL29020_LIGHT__AMBIENT = 0,
    ISL29020_LIGHT__INFRARED = 1,
} isl29020_light_t;

/** Available resolutions of the sensor */
typedef enum
{
    ISL29020_RESOLUTION__16bit = 0,
    ISL29020_RESOLUTION__12bit = 1,
    ISL29020_RESOLUTION__8bit = 2,
    ISL29020_RESOLUTION__4bit = 3,
} isl29020_resolution_t;

/** Available ranges of the sensor */
typedef enum
{
    ISL29020_RANGE__1000lux = 0,
    ISL29020_RANGE__4000lux = 1,
    ISL29020_RANGE__16000lux = 2,
    ISL29020_RANGE__64000lux = 3,
} isl29020_range_t;

/**
 * Configure the sampling parameters.
 *
 * \param light the type of sensor to use, either ambient or IR;
 * \param res the resolution of the measure, note that this impact the duration
 *      of the measure: 16bit/100ms, 12bit/6.25ms, 8bit/390µs, 4bit/24µs
 * \param range the range the sampling
 */
void isl29020_prepare(isl29020_light_t light,
        isl29020_resolution_t res, isl29020_range_t range);

/**
 * Request a single sample, the device will go power down automatically
 * afterwards
 */
void isl29020_sample_once();

/**
 * Request continuous sampling of the light sensor.
 */
void isl29020_sample_continuous();

/**
 * Stop any conversion and put the sensor in power down mode
 */
void isl29020_powerdown();

/**
 * Read the sample value.
 *
 * \return the value in lux
 */
float isl29020_read_sample();

/** @} */

/** @} */

#endif /* ISL29020_H_ */
