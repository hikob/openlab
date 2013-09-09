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

/** \example example_snprintf.c
 * This example demonstrate how to use the light weight snprintf
 * \date Oct 5, 2012
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "platform.h"
#include "printf.h"
#include "random.h"

int main()
{
    char buf[20], buf2[10];
    uint32_t n, i, ret, k;

    platform_init();

    for(k = 0; k < 10; k++)
    {
        n = ((sizeof(buf) / sizeof(buf[0]) - 1) * random_rand16()) >> 16;

        for(i = 0; i < n; i++)
        {
            buf[i] = 'A' + (random_rand16() & 0xF);
        }
        buf[i] = '\0';

        ret = snprintf(buf2, sizeof(buf2) / sizeof(buf2[0]), "%s", buf);

        printf("snprintf(\"%s\") = %d (\"%s\")\n", buf, ret, buf2);
    }

    return 0;
}
