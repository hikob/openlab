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
 * \file buf_util.c
 * \date Aug 17, 2011
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "buf_util.h"

bool cmp(uint8_t *a, uint8_t *b, uint16_t len)
{
    uint8_t *ta = a, *tb = b;
    uint8_t l = 0;

    while ((*tb != 0) && (*ta != 0))
    {
        if (*tb != *ta)
        {
            return false;
        }

        ta++;
        tb++;
        l++;
    }

    return (len > 0 ? l == len : *ta == *tb);
}

void cpy(uint8_t *src, uint8_t *dst, uint16_t len)
{
    uint16_t i;

    for (i = 0; i < len; i++)
    {
        dst[i] = src[i];
    }
}

void zero(uint8_t *buf, uint16_t len)
{
    uint16_t i;

    for (i = 0; i < len; i++)
    {
        buf[i] = 0;
    }
}

void split(uint8_t *filename, uint8_t *name, uint8_t *ext)
{
    uint16_t cpt = 0, cpt2 = 0;

    while ((filename[cpt] != 0) && (filename[cpt] != '.') && (cpt < 8))
    {
        name[cpt] = filename[cpt];
        cpt++;
    }

    while ((filename[cpt] != 0) && (filename[cpt] != '.'))
    {
        cpt++;
    }

    while ((filename[cpt] != 0) && (cpt2 < 3))
    {
        cpt++;
        ext[cpt2] = filename[cpt];
        cpt2++;
    }
}

uint16_t length(uint8_t *buf, uint16_t max)
{
    uint16_t i = 0;

    while (i != max)
    {
        if (buf[i] == '\0')
        {
            break;
        }

        i++;
    }

    return i;
}
