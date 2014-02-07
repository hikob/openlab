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
 * rf231_timings.c
 *
 *  Created on: Nov 9, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include <stdlib.h>
#include "platform.h"
#include "printf.h"

#include "soft_timer.h"
#include "event.h"

#include "phy.h"
#include "rf2xx.h"

static void start_test(void *);

static void test_start(handler_arg_t arg);

static uint32_t t_sleep, t_sleep_total;
static uint32_t t_wakeup, t_wakeup_total;
static uint32_t t_cca, t_cca_total;
static uint32_t t_rx, t_rx_total;

static uint32_t count;
static phy_packet_t my_pkt;

static soft_timer_t stim;

int main()
{
    // Initialize the platform
    platform_init();

    event_init();
    soft_timer_init();
    event_post(EVENT_QUEUE_APPLI, start_test, NULL );

    platform_run();
    return 0;
}

static void start_test(handler_arg_t arg)
{
    printf("Testing the RF231 timings\n");

    // Init. the radio
    phy_reset(platform_phy);
    phy_idle(platform_phy);

    // Reset the total values
    t_sleep_total = 0;
    t_wakeup_total = 0;
    t_cca_total = 0;
    t_rx_total = 0;

    count = 0;

    test_start(NULL );
}

static void test_start(handler_arg_t arg)
{
    // Start Idle
    phy_idle(platform_phy);
    count ++;

    soft_timer_delay_ms(50);

    uint16_t t_a, t_b;

    // SLEEP
    t_a = soft_timer_time();
    phy_sleep(platform_phy);
    t_b = soft_timer_time();

    // Compute sleep time
    t_sleep = t_b - t_a;
    t_sleep_total += t_sleep;

    soft_timer_delay_ms(50);

    // WAKEUP
    t_a = soft_timer_time();
    phy_idle(platform_phy);
    t_b = soft_timer_time();

    // Compute wakeup time
    t_wakeup = t_b - t_a;
    t_wakeup_total += t_wakeup;

    soft_timer_delay_ms(50);

    // RX
    t_a = soft_timer_time();
    phy_rx(platform_phy, 0, 0, &my_pkt, NULL );
    t_b = soft_timer_time();
    phy_idle(platform_phy);

    // Compute RX time
    t_rx = t_b - t_a;
    t_rx_total += t_rx;

    soft_timer_delay_ms(50);

    // CCA
    int32_t cca;
    t_a = soft_timer_time();
    phy_cca(platform_phy, &cca);
    t_b = soft_timer_time();

    soft_timer_delay_ms(50);

    // Compute cca time
    t_cca = t_b - t_a;
    t_cca_total += t_cca;
    phy_idle(platform_phy);

    // Print results
    printf("[%u]\t%u(%u)\t%u(%u)\t%u(%u)\t%u(%u)\n", count,
            t_sleep_total / count, t_sleep, t_wakeup_total / count, t_wakeup,
            t_cca_total / count, t_cca, t_rx_total / count, t_rx);

    // Schedule next
    soft_timer_set_handler(&stim, test_start, NULL );
    soft_timer_start(&stim, soft_timer_ms_to_ticks(100), 0);
}

