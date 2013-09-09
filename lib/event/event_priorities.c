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
 * \file event_priorities.c
 *
 * \date May 15, 2013
 * \author Damien Hedde <damien.hedde.at.hikob.com>
 */

#include "event_priorities.h"

__attribute__((__weak__))
const event_priorities_t event_priorities =
{
    /* priority of EVENT_APPLI */
    configMAX_PRIORITIES - 2,
    /* priority of EVENT_NETWORK */
    configMAX_PRIORITIES - 1,
};
