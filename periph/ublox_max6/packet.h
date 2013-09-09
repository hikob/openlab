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
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 *
 */


#ifndef PACKET_H_
#define PACKET_H_

typedef struct
{
    uint8_t  class, id;
    uint16_t length;
    uint32_t iTOW;
    int32_t  lon, lat, height, hMSL;
    uint32_t hAcc, vAcc;
} __attribute__((packed)) max6x_nav_posllh_t; // Size: 32

typedef struct
{
    uint8_t  class, id;
    uint16_t length;
    uint32_t iTOW;
    int32_t  fTOW;
    int16_t  week;
    uint8_t  gpsFix, flags;
    int32_t  ecefX, ecefY, ecefZ;
    uint32_t pAcc;
    int32_t  ecefVX, ecefVY, ecefVZ;
    uint32_t sAcc;
    uint16_t pDOP;
    uint8_t  res1, numSV;
    uint32_t res2;
} __attribute__((packed)) max6x_nav_sol_t; // Size: 56

typedef struct
{
    uint8_t class, id;
    uint16_t length;
    uint32_t iTOW;
    uint8_t numCh, globalFlags;
    uint16_t reserved;
} __attribute__((packed)) max6x_nav_svinfo_header_t; // Size: 12

typedef struct
{
    uint8_t channel, svid, flags, quality, cno;
    int8_t elev;
    int16_t azim;
    int32_t prRes;
} __attribute__((packed)) max6x_nav_svinfo_channel_t; // Size: 12

typedef struct
{
    uint8_t  class, id;
    uint16_t length;
    uint32_t iTOW, tAcc;
    int32_t  nano;
    uint16_t year;
    uint8_t month, day, hour, min, sec, valid;
} __attribute__((packed)) max6x_nav_timeutc_t; // Size: 32

#endif
