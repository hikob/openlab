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
 * Copyright (C) 2013 HiKoB.
 */

/*
 * test_scanf.c
 *
 *  Created on: Jan 28, 2013
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "platform.h"
#include "printf.h"
#include "scanf.h"
#include "debug.h"

#define ASSERT(exp)\
    do {\
        if(!(exp))\
        {\
            log_error("%s at line %d", #exp, __LINE__);\
        }\
    }\
    while(0);

int main()
{
    char str[] = "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
    char str2[] = "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
    char str3[] = "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
    char str4[] = "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
    char str5[] = "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";
    int i;
    unsigned int u;
    float f;
    char c;

    platform_init();

    printf("%d\n", sscanf("Hello world!!! From Openlab\n", "%s", str));

    ASSERT(sscanf("Hello world!!! From Openlab\n", "%s", str) == 1);
    printf("|%s|\n", str);

    ASSERT(sscanf("12345 slkjfd", "%d", &i) == 1);
    printf("%d\n", i);
    
    ASSERT(sscanf(" -1248 64563", "%d", &i) == 1);
    printf("%d\n", i);

    ASSERT(sscanf(" \t\n54321 EMLkTk ", "%u", &u) == 1);
    printf("%u\n", u);
    
    ASSERT(sscanf("     -1   emlrklk ", "%u", &u) == 0);
    printf("%u\n", u);

    ASSERT(sscanf("  DEADBEEF ABCD", "%x", &u) == 1);
    printf("%x\n", u);

    ASSERT(sscanf("3.14159265358979323846264338", "%f", &f) == 1);
    printf("%f\n", f);

    ASSERT(sscanf("-2.71828", "%f", &f) == 1);
    printf("%f\n", f);
    
    ASSERT(sscanf("-2.71828e2", "%f", &f) == 1);
    printf("%f\n", f);
    
    ASSERT(sscanf("-27.1828e-1", "%f", &f) == 1);
    printf("%f\n", f);

    ASSERT(sscanf("   alkje lkj lakzje ", "%c", &c) == 1);
    printf("%c\n", c);

    ASSERT(sscanf("Hello world!!! From Openlab\n", "%s%s%s%s%s", str, str2, str3, str4, str5) == 4);
    printf("|%s|%s|%s|%s|%s|\n", str, str2, str3, str4, str5);

    ASSERT(sscanf("key = BEEF\nkey1 = 3.14\nkey2 = \"value\"", "%s = %x%s = %f%s = %s", str, &u, str2, &f, str3, str4) == 6);
    printf("%s = %s|%s = %x|%s = %f\n", str3, str4, str, u, str2, f);

    while(1);

    return 0;
}
