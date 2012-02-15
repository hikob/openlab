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
 * rf231_timings.c
 *
 *  Created on: Nov 9, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include <stdlib.h>
#include "platform.h"
#include "printf.h"

#include "phy.h"

static void test_task(void *);

static uint32_t t_sleep, t_sleep_total;
static uint32_t t_wakeup, t_wakeup_total;
static uint32_t t_cca, t_cca_total;
static uint32_t t_rx, t_rx_total;

static uint32_t count;
static phy_packet_t my_pkt;

int main()
{
    // Initialize the platform
    platform_init();

    // Create a test task
    xTaskCreate(test_task, (const signed char * const)"test",
                4 * configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    platform_run();
    return 0;
}
void test_task(void *arg)
{
    printf("Testing the RF231 timings\n");

    // Init. the radio
    phy_reset(phy);
    phy_idle(phy);

    // Enable CLKM @ 250kHz
    uint8_t reg = RF2XX_TRX_CTRL_0_DEFAULT__PAD_IO
                  | RF2XX_TRX_CTRL_0_DEFAULT__PAD_IO_CLKM
                  | RF2XX_TRX_CTRL_0_CLKM_CTRL__250kHz;
    rf2xx_reg_write(rf231, RF2XX_REG__TRX_CTRL_0, reg);

    // Set XCLK TRIM
    reg = RF2XX_XOSC_CTRL__XTAL_MODE_CRYSTAL | 0xE;
    rf2xx_reg_write(rf231, RF2XX_REG__XOSC_CTRL, reg);

    // Reset the total values
    t_sleep_total = 0;
    t_wakeup_total = 0;
    t_cca_total = 0;
    t_rx_total = 0;

    count = 0;

    while(1)
    {
        uint16_t t_a, t_b;

        // Start Idle
        phy_idle(phy);

        vTaskDelay(configTICK_RATE_HZ / 25);

        // SLEEP
        t_a = net_timer_time();
        phy_sleep(phy);
        t_b = net_timer_time();

        // Compute sleep time
        t_sleep = t_b - t_a;
        t_sleep_total += t_sleep;

        vTaskDelay(configTICK_RATE_HZ / 25);
        // WAKEUP
        t_a = net_timer_time();
        phy_idle(phy);
        t_b = net_timer_time();

        // Compute wakeup time
        t_wakeup = t_b - t_a;
        t_wakeup_total += t_wakeup;

        vTaskDelay(configTICK_RATE_HZ / 25);
        // CCA
        t_a = net_timer_time();
        phy_cca(phy);
        t_b = net_timer_time();

        // Compute cca time
        t_cca = t_b - t_a;
        t_cca_total += t_cca;
        phy_idle(phy);

        vTaskDelay(configTICK_RATE_HZ / 25);
        // RX
        t_a = net_timer_time();
        phy_rx(phy, 0, 0, &my_pkt, NULL);
        t_b = net_timer_time();

        // Compute RX time
        t_rx = t_b - t_a;
        t_rx_total += t_rx;

        count++;

        // Print results
        printf("[%u]\t%u(%u)\t%u(%u)\t%u(%u)\t%u(%u)\n", count, t_sleep_total
               / count, t_sleep, t_wakeup_total / count, t_wakeup, t_cca_total
               / count, t_cca, t_rx_total / count, t_rx);
    }

    while(1)
    {
        asm volatile("nop");
    }

    return;
}

