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
 * softtim.c
 *
 *  Created on: Feb 13, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "soft_timer.h"
#include "printf.h"
#include "debug.h"

static void do_test(handler_arg_t arg);

#define TEST_NUM 10

static soft_timer_t timers[TEST_NUM];

typedef struct
{
    uint32_t i;
    uint32_t count;
} stat_t;

static stat_t stats[TEST_NUM];

static uint32_t t_start;

static void char_rx(handler_arg_t arg, uint8_t c)
{
    HALT();
}

int main()
{
    // Initialize the platform
    platform_init();

    // Initialize the soft timer library
    soft_timer_init();

    uart_set_rx_handler(uart_print, char_rx, NULL);

    // Configure the timers and start
    uint32_t i;

    for (i = 0; i < TEST_NUM; i++)
    {
        stats[i].i = i;
        stats[i].count = 0;

        soft_timer_set_handler(timers + i, do_test, stats + i);
        soft_timer_start(timers + i, soft_timer_ms_to_ticks(10) * (i + 1), 1);
    }

    t_start = soft_timer_time();

    // Run
    platform_run();

    return 0;
}

static void do_test(handler_arg_t arg)
{
    static uint32_t n = 0;

    stat_t *stat = (stat_t *) arg;

    // Increment count
    stat->count++;

    if (stat->i == (TEST_NUM - 1))
    {
        uint32_t now = soft_timer_time();

        uint32_t delta = now - t_start;

        uint32_t i;
        printf("[%u", n++);

        for (i = 0; i < TEST_NUM; i++)
        {
            printf("\t%u:%u", stats[i].i, soft_timer_ticks_to_ms(delta
                    / stats[i].count));
        }

        printf("]\n");
    }
}
