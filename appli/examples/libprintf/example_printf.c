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

/** \example example_printf.c
 * This example demonstrate how to use the light weight printf
 * \date Oct 5, 2012
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "platform.h"
#include "printf.h"
#include "random.h"

int main()
{
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
    
    printf("printf\n===================\n");

    printf("%s\n", ptr);
    printf("%s is null pointer\n", np);
    printf("%d = 5\n", i);
    i = (1 << (bs - 1)) + 1;
    printf("%d = - max int\n", i);
    printf("char %c = 'a'\n", 'a');
    printf("hex %x = ff\n", 0xff);
    printf("hex %02x = 00\n", 0);
    printf("signed %d = unsigned %u = hex %x\n", -3, -3, -3);
    printf("%d %s(s)\n", 0, "message");
    printf("%d %s(s) with %%\n", 0, "message");
    printf("Padded string: \"%10s\"\n", "test");
    printf(" 3: %04d zero padded\n", 3);
    printf(" 3: %4d space padded\n", 3);
    printf("-3: %04d zero padded\n", -3);
    printf("-3: %4d space padded\n", -3);
    printf("-324: %4d space padded\n", -324);
    printf("-9324: %4d space padded\n", -9324);

    printf("printf return value\n===================\n");

    for(i = 0; i < 10; i++)
    {
        n = random_rand16() * random_rand16();
        ret = printf("%d\n", n);
        printf("printf last return value: %d\n", ret);
    }

    printf("\nprintf for float\n======================\n");

    for(i = -30; i <= 30; i++)
    {
        printf("%f\t", ((float)i) / 2);
    }

    printf("\n");

    for (i = 0; i < sizeof(special_floats) / sizeof(special_floats[0]); i++)
    {
        printf("%f\n", special_floats[i]);
    }

    return 0;
}
