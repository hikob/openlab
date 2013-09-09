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
 * tmp102.c
 *
 *  Created on: Oct 24, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "printf.h"

#include "tmp102.h"

int main()
{
    int i;

    // Initialize the platform
    platform_init();

    printf("# Testing TMP102\n");

    int num = tmp102_number();
    int tmp;

    printf("# Starting continuous sampling\n");
    for (tmp=0; tmp<num; tmp++)
    {
        tmp102_sample_continuous(tmp, TMP102_CONV_RATE_8HZ);
    }

    while (1)
    {
        for (tmp=0; tmp<num; tmp++)
        {
            int16_t t;
            t = tmp102_read(tmp) >> 4;

            int16_t t_int, t_dec;
            t_int = t >> 4;
            t_dec = (t & 0xF);

            printf("\t%d.%04d (%d)", t_int, t_dec * 625, t);
        }

        printf("\n");

        for (i = 0; i < 0x80000; i++)
        {
            __asm__("nop");
        }
    }

    return 0;
}
