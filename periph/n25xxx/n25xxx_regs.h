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
 * \file n25xxx_regs.h
 * \date May 18, 2012
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef N25XXX_REGS_H_
#define N25XXX_REGS_H_

enum n25xxx_instructions
{
    N25XXX_INS__RDID     = 0x9E,
    N25XXX_INS__READ     = 0x03,
    N25XXX_INS__FASTREAD = 0x0B,
    N25XXX_INS__ROTP     = 0x4B,
    N25XXX_INS__WREN     = 0x06,
    N25XXX_INS__WRDI     = 0x04,
    N25XXX_INS__PP       = 0x02,
    N25XXX_INS__POTP     = 0x42,
    N25XXX_INS__SSE      = 0x20,
    N25XXX_INS__SE       = 0xD8,
    N25XXX_INS__BE       = 0xC7,
    N25XXX_INS__PER      = 0x7A,
    N25XXX_INS__PES      = 0x75,
    N25XXX_INS__RDSR     = 0x05,
    N25XXX_INS__WRSR     = 0x01,
    N25XXX_INS__RDLR     = 0xE8,
    N25XXX_INS__WRLR     = 0xE5,
    N25XXX_INS__RFSR     = 0x70,
    N25XXX_INS__CLFSR    = 0x50,
    N25XXX_INS__RDNVCR   = 0xB5,
    N25XXX_INS__WRNVCR   = 0xB1,
    N25XXX_INS__RDVCR    = 0x85,
    N25XXX_INS__WRVCR    = 0x81,
    N25XXX_INS__RDVECR   = 0x65,
    N25XXX_INS__WRVECR   = 0x61
};

#endif /* N25XXX_REGS_H_ */
