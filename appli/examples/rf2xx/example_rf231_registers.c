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
 * \example example_rf231_registers.c
 * This file is an example that shows how to read some RF231 registers.
 *
 * \date Oct 1, 2012
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */


#include <stdint.h>
#include "platform.h"
#include "printf.h"

#include "rf2xx.h"
#include "rf2xx/rf2xx_regs.h"

extern rf2xx_t rf231;

static void app_task(void *);

int main()
{
    // Initialize the platform
    platform_init();

    // Create a task for the application
    xTaskCreate(app_task, (const signed char * const) "rf231", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Run
    platform_run();
    return 0;
}

static void app_task(void *param)
{
    printf("# Testing RF231 VERSION and PART_NUM registers\n");

    printf("# Preparing radio chip\n");
    rf2xx_reset(rf231);
    rf2xx_wakeup(rf231);

    uint32_t i = 0;
    while (1)
    {
        // Read PART_NUM and VERSION registers
        uint8_t part_num, version;

        part_num = rf2xx_reg_read(rf231, RF2XX_REG__PART_NUM);
        version = rf2xx_reg_read(rf231, RF2XX_REG__VERSION_NUM);

        printf("[%u] PART_NUM: %x (should be 3), VERSION: %x (should be 2)\n",
                i, part_num, version);

        // Wait for 1/2 second
        vTaskDelay(configTICK_RATE_HZ / 2);
        i++;
    }
}


