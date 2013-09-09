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

/** \example example_l3g4200d_interrupt.c
 * This file is an example of the use of L3G4200D gyroscope sensor using the data
 * ready interrupt mechanism.
 *
 * \date Sep 27, 2012
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include <math.h>
#include "platform.h"
#include "printf.h"
#include "l3g4200d.h"

static void app_task(void *);
static xSemaphoreHandle sync;

static void gyro_data(handler_arg_t arg)
{
    signed portBASE_TYPE pxHigherPriorityTaskWoken;

    // Signal the application that a sample is available
    xSemaphoreGiveFromISR(sync, &pxHigherPriorityTaskWoken);
}

int main()
{
    // Initialize the platform
    platform_init();

    // Create synchronization mutex
    sync = xSemaphoreCreateCounting(1, 0);

    // Create a task for the application
    xTaskCreate(app_task, (const signed char * const) "l3g4200", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Run
    platform_run();
    return 0;
}

#define GYR_RES (8.75e-3)  // The resolution is 8.75mdps for the +/-250dps scale

static void app_task(void *param)
{
    uint8_t i;
    int16_t g[3];

    printf("# Testing L3G4200D\n");

    printf("# Initializing L3G4200D...\n");
    l3g4200d_powerdown();

    printf("# Setting L3G4200D gyroscope\n");
    l3g4200d_gyr_config(L3G4200D_200HZ, L3G4200D_250DPS, true);
    
    printf("# Reading L3G4200D WHOAMI register: 0xD3 == 0x%02X\n", l3g4200d_read_whoami());
    for (i = 1; i <= 5; i++)
    {
        printf("# CTRL_REG%d = 0x%02X\n", i, l3g4200d_read_crtl_reg(i));
    }
    
    printf("# Setup L3G4200D data handler\n");
    l3g4200d_set_drdy_int(gyro_data, NULL);
    
    // Start a dummy reading to boostrap the interrupts
    l3g4200d_read_rot_speed(g);

    while (1)
    {
        // Wait for an interrupt to come
        xSemaphoreTake(sync, portMAX_DELAY);

        // Read the values
        l3g4200d_read_rot_speed(g);
    
        printf("G = (%f\t%f\t%f)\n", g[0] * GYR_RES, g[1] * GYR_RES, g[2] * GYR_RES);
    }
}
