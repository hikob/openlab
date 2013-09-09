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

/**
 * \file adg759.h
 * \brief ADG759 header file
 *
 * \date Dec 21, 2011
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef ADG759_H_
#define ADG759_H_

/**
 * \addtogroup periph
 * @{
 */

/**
 * \defgroup adg759 ADG759 multiplexer
 *
 * \brief This provides all the methods and types required to control the ADG759
 * multiplexer driver.
 *
 * The ADG759 is a four to one analog multiplexer, which may be enabled or disabled
 * (to save power)
 *
 * Refer to your board schematics to know the meaning of the different inputs.
 *
 * @{
 */

/**
 * Abstract type for referencing a multiplexer
 */
typedef const void* adg759_t;
/**
 * \brief Type defining a multiplexer input.
 */
typedef enum
{
    /** First input */
    ADG759_INPUT_1 = 0,
    /** Second input */
    ADG759_INPUT_2 = 1,
    /** Third input */
    ADG759_INPUT_3 = 2,
    /** Fourth input */
    ADG759_INPUT_4 = 3,
} adg759_input_t;

/**
 * \brief Enable the multiplexer.
 *
 * This powers ON the multiplexer.
 *
 * \param adg the reference of the multiplexer
 */
void adg759_enable(adg759_t adg);

/**
 * \brief Disable the multiplexer.
 *
 * This powers OFF the multiplexer
 * \param adg the reference of the multiplexer
 */
void adg759_disable(adg759_t adg);

/**
 * \brief Select a given input of the multiplexer.
 *
 * This select one of the four inputs.
 *
 * \note the ADG759 should be enabled with \ref adg759_enable before being selected
 *
 * \param adg the reference of the multiplexer
 * \param input the input to select, one of \ref adg759_input_t
 */
void adg759_select(adg759_t adg, adg759_input_t input);

/**
 * @}
 * @}
 */

#endif /* ADG759_H_ */
