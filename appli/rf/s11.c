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
 * s11.c
 *
 *  Created on: Sep 5, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include <stdint.h>
#include <stdlib.h>
#include "platform.h"
#include "printf.h"
#include "uart.h"

#include "rf2xx.h"

extern rf2xx_t rf231;

// Task function
static void test_task(void *);

int main()
{
    // Initialize the platform
    platform_init();

    // Create a test task
    xTaskCreate(test_task, (const signed char * const)"test",
                configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Run
    platform_run();
    return 0;
}

void test_task(void *arg)
{
    (void) arg;

    uint8_t channel = 11;

    // Debug
    printf("S11 (RX) Test\n");

    // Step 1: RESET
    rf2xx_reset(rf231);
    rf2xx_wakeup(rf231);

    // Step 2: Set State TRX_OFF
    rf2xx_reg_write(rf231, RF2XX_REG__TRX_STATE, RF2XX_TRX_STATE__FORCE_TRX_OFF);

    // Step 3: Set channel
    rf2xx_reg_write(rf231, RF2XX_REG__PHY_CC_CCA,
                    RF2XX_PHY_CC_CCA_DEFAULT__CCA_MODE | channel);

    // Step 4: Set output power 0dBm
    rf2xx_reg_write(rf231, RF2XX_REG__PHY_TX_PWR,
                    RF2XX_PHY_TX_PWR_TX_PWR_VALUE__0dBm);

    // If radio has external PA, enable DIG3/4
    if (rf2xx_has_pa(rf231))
    {
        // Enable the PA
        rf2xx_pa_enable(rf231);

        // Activate DIG3/4 pins
        uint8_t reg = rf2xx_reg_read(rf231, RF2XX_REG__TRX_CTRL_1);
        reg |= RF2XX_TRX_CTRL_1_MASK__PA_EXT_EN;
        rf2xx_reg_write(rf231, RF2XX_REG__TRX_CTRL_1, reg);
    }

    // Step 5: Verify TRX_OFF status
    if (rf2xx_reg_read(rf231, RF2XX_REG__TRX_STATUS)
            != RF2XX_TRX_STATUS__TRX_OFF)
    {
        printf("Step 5 error, not TRX_OFF!\n");
    }

    // Step 6: Disable SHR detection
    rf2xx_reg_write(rf231, RF2XX_REG__RX_SYN, 0x80);

    // Set IRQ mask in ED_DONE
    rf2xx_reg_write(rf231, RF2XX_REG__IRQ_MASK,
                    RF2XX_IRQ_STATUS_MASK__CCA_ED_DONE);
    // Read IRQ to clear
    (void) rf2xx_reg_read(rf231, RF2XX_REG__IRQ_MASK);

    // Start RX
    rf2xx_set_state(rf231, RF2XX_TRX_STATE__RX_ON);

    printf("RX running on channel %u (%u MHz)...\n", channel, 2405 + 5
           * (channel - 11));

    vTaskDelay(configTICK_RATE_HZ * 2);

    uint32_t count = 0;

    while (1)
    {
        // Write an arbitrary value to PHY_ED_LEVEL
        rf2xx_reg_write(rf231, RF2XX_REG__PHY_ED_LEVEL, 0x12);

        // Wait until ED ready
        uint8_t irq_status;

        do
        {
            irq_status = rf2xx_reg_read(rf231, RF2XX_REG__IRQ_STATUS);
        }
        while ((irq_status & RF2XX_IRQ_STATUS_MASK__CCA_ED_DONE) == 0);

        // Read ED
        int8_t ed = rf2xx_reg_read(rf231, RF2XX_REG__PHY_ED_LEVEL);
        ed -= 91;

        printf("ED #%x: %d dBm\n", count++, ed);

        // Wait a little
        vTaskDelay(configTICK_RATE_HZ / 4);
    }

}

