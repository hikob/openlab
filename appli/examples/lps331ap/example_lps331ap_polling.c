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

/** \example example_lps331ap_polling.c
 * This file is an example of the use of LPS331AP pressure sensor using periodical
 * sensor measure (polling).
 * \date Oct 2, 2012
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdint.h>
#include <math.h>
#include "platform.h"
#include "printf.h"
#include "lps331ap.h"

static void app_task(void *);

int main()
{
    // Initialize the platform
    platform_init();

    // Create a task for the application
    xTaskCreate(app_task, (const signed char * const) "lps331", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Run
    platform_run();
    return 0;
}

static void app_task(void *param)
{
    uint32_t pres;
    int16_t temp;

    printf("# Testing LPS331AP\n");

    printf("# Initializing LPS331AP...\n");
    lps331ap_powerdown();

    printf("# Setting LPS331AP pressure sensor\n");
	lps331ap_set_datarate(LPS331AP_P_12_5HZ_T_12_5HZ);

    while (1)
    {
		lps331ap_read_pres(&pres);
		lps331ap_read_temp(&temp);
		
        printf("P = %f\n", pres / 4096.0);
        printf("T = %f\n", 42.5 + temp / 480.0);
        
        vTaskDelay(configTICK_RATE_HZ / 10);
    }
}
