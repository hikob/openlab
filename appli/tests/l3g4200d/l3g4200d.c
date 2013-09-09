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
 * l3g4200d.c
 *
 *  Created on: Jul 31, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "l3g4200d.h"
#include "event.h"
#include "printf.h"

#define GYRO_INTER

#ifdef GYRO_INTER

static int16_t gyro_values[3];
static void gyro_data(handler_arg_t arg);
static void read_gyro(handler_arg_t arg);

static void gyro_data(handler_arg_t arg)
{
    event_post(EVENT_QUEUE_APPLI, read_gyro, arg);
}

static void read_gyro(handler_arg_t arg)
{
    static int i = 0;
    l3g4200d_read_rot_speed(gyro_values);
    if (i == 10)
    {
        printf("G: %d %d %d\n", gyro_values[0], gyro_values[1], gyro_values[2]);
        i = 0;
    }
    i++;
}

#endif

int main()
{
    int i;

    // Initialize the platform
    platform_init();

    printf("# Testing L3G4200D\n");

    printf("# Setting L3G4200D datarate and scale...\n");
    l3g4200d_gyr_config(L3G4200D_200HZ, L3G4200D_2000DPS, true);

    printf("# Reading L3G4200D WHOAMI register: 0xD3 == 0x%02X\n", l3g4200d_read_whoami());
    for (i = 1; i <= 5; i++)
    {
        printf("# CTRL_REG%d = 0x%02X\n", i, l3g4200d_read_crtl_reg(i));
    }

#ifdef GYRO_INTER

    l3g4200d_set_drdy_int(gyro_data, NULL);
    read_gyro(NULL);

    platform_run();

#else

    while (1)
    {
        int16_t g[3];
        uint8_t t;

        l3g4200d_read_rot_speed(g);
        l3g4200d_read_temp(&t);

        printf("%d\t%d\t%d\t%d\n", g[0], g[1], g[2], t);

        for (i = 0; i < 0x800; i++)
        {
            __asm__("nop");
        }

        leds_toggle(LED_0 + LED_1);
    }

#endif

    return 0;
}


