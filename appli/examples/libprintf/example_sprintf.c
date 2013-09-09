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

/** \example example_sprintf.c
 * This example demonstrate how to use the light weight sprintf
 * \date Oct 5, 2012
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "platform.h"
#include "printf.h"
#include "random.h"

int main()
{
    char buf[80];
    char *ptr = "Hello world!";
    char *np = 0;
    int i = 5, n, ret;
    unsigned int bs = sizeof(int) * 8;
    float special_floats[] =
    {
        1.0 / 0.0,             // inf
        -1.0 / 0.0,            //-inf
        1.0 / 0.0 - 1.0 / 0.0, // NaN
        3.40282346e38,      // Greatest normalized number
        1.17549435e-38      // Smallest normalized number
    };

    platform_init();
    
    printf("sprintf\n===================\n");

    sprintf(buf, "%s\n", ptr); printf(buf);
    sprintf(buf, "%s is null pointer\n", np); printf(buf);
    sprintf(buf, "%d = 5\n", i); printf(buf);
    i = (1 << (bs - 1)) + 1;
    sprintf(buf, "%d = - max int\n", i); printf(buf);
    sprintf(buf, "char %c = 'a'\n", 'a'); printf(buf);
    sprintf(buf, "hex %x = ff\n", 0xff); printf(buf);
    sprintf(buf, "hex %02x = 00\n", 0); printf(buf);
    sprintf(buf, "signed %d = unsigned %u = hex %x\n", -3, -3, -3); printf(buf);
    sprintf(buf, "%d %s(s)\n", 0, "message"); printf(buf);
    sprintf(buf, "%d %s(s) with %%\n", 0, "message"); printf(buf);
    sprintf(buf, "Padded string: \"%10s\"\n", "test"); printf(buf);
    sprintf(buf, " 3: %04d zero padded\n", 3); printf(buf);
    sprintf(buf, " 3: %4d space padded\n", 3); printf(buf);
    sprintf(buf, "-3: %04d zero padded\n", -3); printf(buf);
    sprintf(buf, "-3: %4d space padded\n", -3); printf(buf);
    sprintf(buf, "-324: %4d space padded\n", -324); printf(buf);
    sprintf(buf, "-9324: %4d space padded\n", -9324); printf(buf);

    printf("sprintf return value\n===================\n");

    for(i = 0; i < 10; i++)
    {
        n = random_rand16() * random_rand16();
        ret = sprintf(buf, "%d\n", n); printf(buf);
        printf("sprintf last return value: %d\n", ret);
    }

    printf("\nsprintf for float\n======================\n");

    for(i = -30; i <= 30; i++)
    {
        sprintf(buf, "%f\t", ((float)i) / 2); printf(buf);
    }

    printf("\n");

    for (i = 0; i < sizeof(special_floats) / sizeof(special_floats[0]); i++)
    {
        sprintf(buf, "%f\n", special_floats[i]); printf(buf);
    }

    return 0;
}
