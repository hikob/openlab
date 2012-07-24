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
 * lps331.h
 *
 *  Created on: Jan 25, 2012
 *      Author: braillon
 */

#ifndef LPS331_H_
#define LPS331_H_

#include <stdint.h>
#include <stdbool.h>
#include "handler.h"

/**
 * This type defines a pressure sensor
 */

typedef enum
{
	LPS331_ONE_SHOT            = 0x00,
	LPS331_P_1HZ_T_1HZ         = 0x10,
	LPS331_P_7HZ_T_1HZ         = 0x20,
	LPS331_P_12_5HZ_T_1HZ      = 0x30,
	LPS331_P_25HZ_T_1HZ        = 0x40,
	LPS331_P_7HZ_T_7HZ         = 0x50,
	LPS331_P_12_5HZ_T_12_5HZ   = 0x60,
	LPS331_P_25HZ_T_25HZ       = 0x70,
} lps331_datarate_t;

/* Read WHOAMI register */
uint8_t lps331_read_whoami();

/* Read CTRL_REGi */
uint8_t lps331_read_crtl_reg(uint8_t i);

/* Switch off sensor */
bool lps331_powerdown();

/* Data rate configuration */
bool lps331_set_datarate(lps331_datarate_t datarate);

/* Pressure reading */
bool lps331_read_pres(uint32_t *p);

/* Temperature reading */
bool lps331_read_temp( int16_t *temp);

/* Data ready handler configuration */
void lps331_set_drdy_int(handler_t data_ready_handler, handler_arg_t data_ready_arg);

#endif /* LPS331_H_ */
