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
 * \example example_soft_timer_delay.c
 *
 * This examples shows how simple active delays may be used in an application
 * to wait for a given time.
 *
 * \date Oct 2, 2012
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "printf.h"

#include "soft_timer_delay.h"

static void app_task(void *);

int main()
{
    // Initialize the platform
    platform_init();

    // Create a task for the application
    xTaskCreate(app_task, (const signed char * const) "soft_timer", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Run
    platform_run();
    return 0;
}

static void app_task(void *param)
{
    uint32_t t_a, t_b;
    printf("Soft Timer Delay example\n");

    while (1)
    {
        printf("Waiting 10ms... ");
        t_a = soft_timer_time();
        soft_timer_delay_ms(10);
        t_b = soft_timer_time();
        printf("[%u ticks]\n", t_b - t_a);

        printf("Waiting 100ms... ");
        t_a = soft_timer_time();
        soft_timer_delay_ms(100);
        t_b = soft_timer_time();
        printf("[%u ticks]\n", t_b - t_a);

        printf("Waiting 1s... ");
        t_a = soft_timer_time();
        soft_timer_delay_s(1);
        t_b = soft_timer_time();
        printf("[%u ticks]\n", t_b - t_a);

        printf("Waiting 10s... ");
        t_a = soft_timer_time();
        soft_timer_delay_s(10);
        t_b = soft_timer_time();
        printf("[%u ticks]\n", t_b - t_a);
    }
}
