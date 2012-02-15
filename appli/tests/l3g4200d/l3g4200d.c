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
 * Copyright (C) 2011 HiKoB.
 */

/*
 * l3g4200d.c
 *
 *  Created on: Jul 31, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "printf.h"

#include "l3g4200d.h"

int main()
{
    int i;

    // Initialize the platform
    platform_init();

    printf("# Testing L3G4200D\n");

    printf("# Setting L3G4200D datarate...\n");
    l3g4200d_set_datarate(l3g4200d, L3G4200D_800HZ);

    printf("# Setting L3G4200D scale...\n");
    l3g4200d_set_scale(l3g4200d, L3G4200D_2000DPS);

    printf("# Reading L3G4200D WHOAMI register: 0xD3 == 0x%02X\n", l3g4200d_read_whoami(l3g4200d));

    for(i = 1; i <= 5; i++)
    {
        printf("# CTRL_REG%d = 0x%02X\n", i, l3g4200d_read_crtl_reg(l3g4200d, i));
    }

    while(1)
    {
        int16_t g[3];
        uint8_t t;

        l3g4200d_read_rot_speed(l3g4200d, g);
        l3g4200d_read_temp(l3g4200d, &t);

        printf("%d %d %d %d\n", g[0], g[1], g[2], t);

        for(i = 0; i < 0x800; i++)
        {
            __asm__("nop");
        }

        leds_toggle(LED_0 + LED_1);
    }

    return 0;
}
