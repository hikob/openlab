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
 * packet_storage.c
 *
 *  Created on: Sep 14, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "packet.h"

enum
{
    PACKET_BUFFER_DEFAULT_SIZE = 6,
};

__attribute__((weak)) packet_t packet_storage_packet_buffer[PACKET_BUFFER_DEFAULT_SIZE];
__attribute__((weak)) uint32_t packet_storage_packet_flags[(PACKET_BUFFER_DEFAULT_SIZE
        - 1) / 32 + 1];

__attribute__((weak)) packet_storage_t packet_storage =
{
        .packet_buffer = packet_storage_packet_buffer,
        .packet_flags = packet_storage_packet_flags,
        .packet_number = PACKET_BUFFER_DEFAULT_SIZE
};
