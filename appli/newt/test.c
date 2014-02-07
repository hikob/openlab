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
 * test.c
 *
 *  Created on: Jul 16, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include "math.h"
#include "platform.h"
#include "printf.h"
#include "debug.h"

#include "soft_timer.h"

#include "adc.h"
#include "pga308.h"
#include "adg759.h"
#include "pca9685.h"
#include "lsm303dlhc.h"
#include "phy.h"

extern adg759_t pga308_mux;

static void app_task(void *param);
static void adc_sample_done(handler_arg_t arg, uint16_t value);
static void read_4_gauges();
static void set_all_triled(float r, float g, float b);
static void phy_rx_handler(phy_status_t status);

static uint32_t phy_channel;
static phy_packet_t phy_pkt;
static int32_t gauges_values[4];

int main()
{
    // Initialize the platform
    platform_init();

    // Create a task for the application
    xTaskCreate(app_task, (const signed char * const) "app",
            configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Run
    platform_run();
    return 0;
}

static void app_task(void *param)
{
    printf("Dalles test appli\n");

    uint32_t led = 0;
    uint32_t count = 0;
    uint32_t color = 0;

    // Enable the pga and mux
    pga308_enable();
    adg759_enable(pga308_mux);

    // Configure the PGA for input gain
    pga308_configure_gain(PGA308_INPUT_GAIN__16, 0x4000, 0x4000,
            PGA308_OUTPUT_GAIN__6);

    // Enable the ADC
    adc_enable(pga308_get_adc());

    // Set ADC handler
    adc_set_handler(pga308_get_adc(), adc_sample_done, NULL);

    // Prepare the ADC
    adc_prepare_single(pga308_get_adc(), pga308_get_adc_channel());

    // Start the LSM303
    lsm303dlhc_acc_config(LSM303DLHC_ACC_RATE_50HZ, LSM303DLHC_ACC_SCALE_4G, LSM303DLHC_ACC_UPDATE_ON_READ);
    lsm303dlhc_mag_config(LSM303DLHC_MAG_RATE_30HZ,
            LSM303DLHC_MAG_SCALE_2_5GAUSS, LSM303DLHC_MAG_MODE_CONTINUOUS,
            LSM303DLHC_TEMP_MODE_OFF);

    while (1)
    {
        phy_idle(platform_phy);
        count++;
        printf("\n###############################\n");
        printf("Test #%u\n", count);
        color += 100 << (8 * led);
        if (count % 16 == 0)
        {
            led += 1;
            if (led > 2)
            {
                led = 0;
            }
        }
        set_all_triled((float) (color & 0xFF) / 256.,
                (float) ((color >> 8) & 0xFF) / 256.,
                (float) ((color >> 16) & 0xFF) / 256.);

        printf("\tLEDs set!\n");

        int16_t axyz[3];
        lsm303dlhc_read_acc(axyz);
        printf("\tAcc:\t%5d\t%5d\t%5d\n", axyz[0], axyz[1], axyz[2]);

        int16_t mxyz[3];
        lsm303dlhc_read_mag(mxyz);
        printf("\tMag:\t%5d\t%5d\t%5d\n", mxyz[0], mxyz[1], mxyz[2]);

        read_4_gauges();
        printf("\tGauges:\t%5d\t%5d\t%5d\t%5d\n", gauges_values[0], gauges_values[1],
                gauges_values[2], gauges_values[3]);

        phy_channel = PHY_2400_MIN_CHANNEL
                + (count % (PHY_2400_MAX_CHANNEL - PHY_2400_MIN_CHANNEL));
        printf("\tPHY RX on channel %u\n", phy_channel);
        phy_set_channel(platform_phy, phy_channel);
        phy_rx_handler(PHY_ERR_INVALID_STATE);

        vTaskDelay(configTICK_RATE_HZ / 2);
    }
}

static void set_triled(uint32_t led, float r, float g, float b)
{
    pca9685_t pca = pca9685_get(led / 4);

    // Set the LEDs
    uint32_t led_r = 3 * (led % 4);
    pca9685_set_led(pca, led_r, 4096 * r);
    pca9685_set_led(pca, led_r + 1, 4096 * g);
    pca9685_set_led(pca, led_r + 2, 4096 * b);
}

static void set_all_triled(float r, float g, float b)
{
    uint32_t i;
    for (i = 0; i < 16; i++)
    {
        set_triled(i, r, g, b);
    }
}
static void phy_rx_handler(phy_status_t status)
{
    if (status == PHY_SUCCESS)
    {
        log_printf("  [RX len %u rssi %d]", phy_pkt.length, phy_pkt.rssi);
    }

    // Enter RX again
    phy_pkt.data = phy_pkt.raw_data;
    phy_pkt.length = 0;
    phy_rx(platform_phy, 0, 0, &phy_pkt, phy_rx_handler);
}

static volatile int32_t adc_value;
static void adc_sample_done(handler_arg_t arg, uint16_t value)
{
    // Store the value
    adc_value = value;
}
static void read_4_gauges()
{
    uint32_t i;

    for (i = 0; i < 4; i++)
    {
        // Set the selected mux
        adg759_select(pga308_mux, ADG759_INPUT_1 + i);

        soft_timer_delay_ms(10);

        // Start new sample
        adc_value = -1;
        adc_sample_single(pga308_get_adc());

        // Wait until value updated
        while (adc_value == -1)
        {
            asm volatile("nop");
        }

        // Store result
        gauges_values[i] = adc_value;
    }
}
