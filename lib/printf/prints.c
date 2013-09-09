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
 * \file prints.c
 * \date Oct 4, 2012
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include <stdbool.h>
#include "printf_.h"

void prints(printf_param_t *p, const char *string, uint16_t width, bool pad_zero)
{
    const char *ptr;

    // Substract string length to width
    if (width > 0)
    {
        ptr = string;

        while (*ptr)
        {
            ptr++;

            if (width > 0)
            {
                width--;
            }
        }
    }

    // Pad if necessary
    while (width > 0)
    {
        p->out(pad_zero ? '0' : ' ', p);
        width--;
    }

    // Write the string
    while (*string)
    {
        p->out(*string, p);
        string++;
    }
}
