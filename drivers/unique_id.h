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
 * unique_id.h
 *
 *  Created on: Jul 28, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef UNIQUE_ID_H_
#define UNIQUE_ID_H_

#include <stdint.h>

typedef union
{
    uint32_t uid32[3];
    uint16_t uid16[6];
    uint8_t  uid8 [12];
} uid_t;

extern const uid_t const *uid;

void uid_print(void);

#endif /* UNIQUE_ID_H_ */
