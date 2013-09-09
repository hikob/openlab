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
 * eeprom_.h
 *
 *  Created on: Jan 2, 2013
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef EEPROM__H_
#define EEPROM__H_

#include "eeprom.h"

/** Structure containing the available eeprom */
typedef struct
{
    const eeprom_t *internal, *external;
} eeprom_config_t;

/** Config structure to be provided by the platform */
extern const eeprom_config_t eeprom_config;

#endif /* EEPROM__H_ */
