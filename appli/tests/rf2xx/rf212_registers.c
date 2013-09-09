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
 * rf212_registers.c
 *
 *  Created on: Jul 15, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "printf.h"

#include "rf2xx.h"
#include "rf2xx/rf2xx_regs.h"

extern rf2xx_t rf212;

int main()
{
    uint8_t part_num, version_num;

    // Initialize the platform
    platform_init();

    printf("Testing RF2XX\n");

    // Set initial values
    leds_on(LED_0);
    leds_off(LED_1);

    printf("Initializing RF212...");
    // Init. the radio
    rf2xx_wakeup(rf212);
    printf("OK\n");

    printf("Reading RF212 PART_NUM (should be 7): ");
    part_num = rf2xx_reg_read(rf212, RF2XX_REG__PART_NUM);
    printf("%x\n", part_num);

    printf("Reading RF212 VERSION_NUM (should be 1): ");
    version_num = rf2xx_reg_read(rf212, RF2XX_REG__VERSION_NUM);
    printf("%x\n", version_num);

    while (1)
    {
        int i;

        for (i = 0; i < 0x10000; i++)
        {
            __asm__("nop");
        }

        leds_toggle(LED_0 + LED_1);
    }

    return 0;
}
