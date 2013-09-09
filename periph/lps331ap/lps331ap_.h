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
 * lps331ap_.h
 *
 *  Created on: Jan 25, 2012
 *      Author: braillon
 */

#ifndef LPS331AP__H_
#define LPS331AP__H_

#include "exti.h"
#include "i2c.h"

/* Configuration */
uint8_t lps331ap_config(i2c_t i2c, exti_line_t data_ready_line);

#endif /* LPS331__H_ */
