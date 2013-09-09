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
 * test_printf.c
 *
 *  Created on: Oct 6, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "platform.h"
#include "printf.h"
#include "ansi.h"

int main()
{
    char *ptr = "Hello world!";
    char *np = 0;
    int i = 5, r;
    unsigned int bs = sizeof(int) * 8;
    int mi;
    char buf[80];
    float f[] =
    {
        1.0 / 0.0,             // inf
        -1.0 / 0.0,            //-inf
        1.0 / 0.0 - 1.0 / 0.0, // NaN
        0,
        1,
        4,
        15,
        -3.2,
        -12.43,
        -1254.14,
        0.25,
        3.14159265358979,
        2.0 * (1 << 30),
        1.0 / (1 << 20),
        1.23456789e-10,
        9.9999e37,
        3.40282346e38,      // Greatest normalized number
        1.17549435e-38      // Smallest normalized number
    };

    platform_init();

    ansi_clear_screen();
    ansi_goto(1, 1);

    printf("printf library test\n===================\n");

    ansi_goto(30, 1);
    printf("%s\n", ptr);

    ansi_goto(1, 4);
    printf("%s is null pointer\n", np);
    printf("%d = 5\n", i);
    mi = (1 << (bs - 1)) + 1;
    printf("%d = - max int\n", mi);
    printf("char %c = 'a'\n", 'a');
    printf("hex %x = ff\n", 0xff);
    printf("hex %02x = 00\n", 0);
    printf("signed %d = unsigned %u = hex %x\n", -3, -3, -3);
    printf("%d %s(s)", 0, "message");
    printf("\n");
    printf("%d %s(s) with %%\n", 0, "message");
    sprintf(buf, "Padded string: \"%10s\"\n", "test");
    printf("%s", buf);
    sprintf(buf, " 3: %04d zero padded\n", 3);
    printf("%s", buf);
    sprintf(buf, " 3: %4d space padded\n", 3);
    printf("%s", buf);
    sprintf(buf, "-3: %04d zero padded\n", -3);
    printf("%s", buf);
    sprintf(buf, "-3: %4d space padded\n", -3);
    printf("%s", buf);
    sprintf(buf, "-324: %4d space padded\n", -324);
    printf("%s", buf);
    sprintf(buf, "-9324: %4d space padded\n", -9324);
    printf("%s", buf);

    if((r = sprintf(buf, "%d", 12345)) == 5)
    {
        printf("sprintf return value is OK\n");
    }
    else
    {
        printf("sprintf return value is wrong, returned value is %d instead of 5\n", r);
    }

    if((r = snprintf(buf, 4, "%d", 12345)) == 5)
    {
        printf("sprintf return value is OK, result should be 123 == %s\n", buf);
    }
    else
    {
        printf("sprintf return value is wrong, returned value is %d instead of 5\n", r);
    }

    if((r = snprintf(buf, 5, "%d", 12345)) == 5)
    {
        printf("sprintf return value is OK, result should be 1234 == %s\n", buf);
    }
    else
    {
        printf("sprintf return value is wrong, returned value is %d instead of 5\n", r);
    }

    if((r = snprintf(buf, 6, "%d", 12345)) == 5)
    {
        printf("sprintf return value is OK, result should be 12345 == %s\n", buf);
    }
    else
    {
        printf("sprintf return value is wrong, returned value is %d instead of 5\n", r);
    }
 
    if((r = snprintf(buf, 7, "%d", 12345)) == 5)
    {
        printf("sprintf return value is OK, result should be 12345 == %s\n", buf);
    }
    else
    {
        printf("sprintf return value is wrong, returned value is %d instead of 5\n", r);
    }   

    printf("\nPrintf for float test:\n======================\n");

    printf("sizeof(float) = %d    sizeof(double) = %d\r\n", (int)sizeof(float), (int)sizeof(double));

    for (mi = 0; mi < sizeof(f) / sizeof(f[0]); mi++)
    {
        void* p = (&f[mi]);
        printf("(uint32_t)0x%08X = (float)%f\r\n", *((uint32_t *)p), f[mi]);
    }

    return 0;
}
