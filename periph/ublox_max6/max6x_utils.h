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

/**
 * max6x.h
 *
 *  Created on: Feb 27, 2012
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef MAX6X_UTILS_H_
#define MAX6X_UTILS_H_

#include <stdint.h>
#include <stdbool.h>
#include "packet.h"

uint32_t max6_timeutc_t_to_timestamp(max6x_nav_timeutc_t *p);

#endif
