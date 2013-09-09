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
 * max6x.c
 *
 *  Created on: Feb 27, 2012
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 *      Author: Guillaume Roche <guillaume.roche.at.hikob.com>
 */

#include "max6x_utils.h"
#include "time.h"

uint32_t max6_timeutc_t_to_timestamp(max6x_nav_timeutc_t *p)
{
    struct tm t;
    t.tm_year = p->year - 1900;
    t.tm_mon = p->month - 1; // Month, 0 - jan
    t.tm_mday = p->day; // Day of the month
    t.tm_hour = p->hour;
    t.tm_min = p->min;
    t.tm_sec = p->sec;
    t.tm_isdst = 0; // Is DST on? 1 = yes, 0 = no, -1 = unknown
    return mktime(&t);
}
