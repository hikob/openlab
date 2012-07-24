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
 * l3g4200d.h
 *
 *  Created on: Jul 31, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef L3G4200D_H_
#define L3G4200D_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    L3G4200D_100HZ = 0x00,
    L3G4200D_200HZ = 0x40,
    L3G4200D_400HZ = 0x80,
    L3G4200D_800HZ = 0xC0
} l3g4200d_datarate_t;

typedef enum
{
    L3G4200D_250DPS  = 0x00,
    L3G4200D_500DPS  = 0x10,
    L3G4200D_2000DPS = 0x30
} l3g4200d_scale_t;

/* Read WHOAMI register */
uint8_t l3g4200d_read_whoami();

/* Read CTRL_REGi */
uint8_t l3g4200d_read_crtl_reg(uint8_t i);

/* Switch off sensor */
void l3g4200d_powerdown();

/* Data rate configuration */
void l3g4200d_set_datarate(l3g4200d_datarate_t datarate);

/* Scale configuration */
void l3g4200d_set_scale(l3g4200d_scale_t scale, bool bdu);

/* Rotation speed reading */
void l3g4200d_read_rot_speed(int16_t *rot_speed);

/* Temperature reading */
void l3g4200d_read_temp(uint8_t *temp);

#endif /* L3G4200D_H_ */
