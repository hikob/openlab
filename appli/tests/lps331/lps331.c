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
 * lps331.c
 *
 *  Created on: Jan 25, 2012
 *      Author: braillon
 */
#include <stdint.h>
#include "platform.h"
#include "printf.h"

#include "lps331ap.h"

int main()
{
	int i;

	// Initialize the platform
	platform_init();

	printf("# Testing LPS331\n");

	printf("# Setting LPS331 datarate...\n");
	lps331ap_set_datarate(LPS331AP_P_12_5HZ_T_12_5HZ);

	printf("# Reading LPS331 WHOAMI register: 0xBB == 0x%02X\n", lps331ap_read_whoami());

	for(i = 1; i <= 2; i++)
	{
		printf("# CTRL_REG%d = 0x%02X\n", i, lps331ap_read_crtl_reg(i));
	}

	while (1)
	{
		uint32_t pres;
        int16_t temp;

		lps331ap_read_pres(&pres);
		lps331ap_read_temp(&temp);

		printf("%x %f %f\n", pres, pres / 4096.0, 22.5 + temp / 480.0);

		for (i = 0; i < 0x80000; i++)
		{
			__asm__("nop");
		}
	}
	return 0;
}
