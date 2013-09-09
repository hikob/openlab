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
 * test_batfb.c
 *
 *  Created on: Oct 26, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "platform.h"
#include "event.h"
#include "soft_timer.h"

#include "printf.h"

#include "batfb.h"

static void sample(handler_arg_t arg);
static void sample_done_isr(handler_arg_t arg);
static void print_result(handler_arg_t arg);

static soft_timer_t sample_timer;

int main()
{
    // Initialize the platform
    platform_init();

    // Initialize the soft timer library
    soft_timer_init();

    printf("# Testing Batfb (battery feedback)\n");

    soft_timer_set_handler(&sample_timer, sample, NULL);
    soft_timer_start(&sample_timer, soft_timer_ms_to_ticks(500), 1);

    platform_run();

    return 0;
}

static void sample(handler_arg_t arg)
{
    // Start sample
    batfb_sample(sample_done_isr, NULL);
}

static void sample_done_isr(handler_arg_t arg)
{
    event_post_from_isr(EVENT_QUEUE_APPLI, print_result, NULL);
}

static void print_result(handler_arg_t arf)
{
    printf("VBat %f\t\tVcc %f\n", batfb_vbat, batfb_vcc);
}
