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
 * \file unique_id.h
 *
 * \date Jul 28, 2011
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef UNIQUE_ID_H_
#define UNIQUE_ID_H_

/**
 * \addtogroup drivers
 * @{
 */

/**
 * \defgroup UID Unique ID
 *
 * This driver is an interface to the STM32 unique ID registers, offering a
 * factory guaranteed unique 96bit number.
 *
 *@{
 */

#include <stdint.h>

/**
 * Union defining the Unique ID in several formats: either as words, half words
 * or bytes array.
 */
typedef union
{
    uint32_t uid32[3];
    uint16_t uid16[6];
    uint8_t  uid8 [12];
} uid_t;

/**
 * Pointer to the Unique ID.
 */
extern const uid_t const *uid;

/**
 * Print the unique ID using printf on the configured serial output.
 */
void uid_print();

/**
 * @}
 * @}
 */

#endif /* UNIQUE_ID_H_ */
