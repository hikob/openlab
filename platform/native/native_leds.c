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
 * native_leds.c
 *
 *  Created: Sept 2011
 *  Author: Antoine Fraboulet <antoine.fraboulet.at.hikob.com>
 */

#include <stdio.h>
#include "platform.h"

#define OFF 0
#define ON  1

static int _led0;
static int _led1;

/* ------------------------------------------------------------ */
/*                                                              */
/* ------------------------------------------------------------ */

void platform_leds_setup()
{
    // Clear LEDs
    leds_off(LED_0 + LED_1);
}

/* ------------------------------------------------------------ */
/*                                                              */
/* ------------------------------------------------------------ */

void leds_on(uint8_t leds)
{
    if (leds & LED_0)
    {
        _led0 = ON;
        fprintf(stderr, "led 0 on\n");
    }

    if (leds & LED_1)
    {
        _led1 = ON;
        fprintf(stderr, "led 1 on\n");
    }
}

/* ------------------------------------------------------------ */
/*                                                              */
/* ------------------------------------------------------------ */

void leds_off(uint8_t leds)
{
    if (leds & LED_0)
    {
        _led0 = OFF;
        fprintf(stderr, "led 0 off\n");
    }

    if (leds & LED_1)
    {
        _led1 = OFF;
        fprintf(stderr, "led 1 off\n");
    }
}

/* ------------------------------------------------------------ */
/*                                                              */
/* ------------------------------------------------------------ */

void leds_toggle(uint8_t leds)
{
    if (leds & LED_0)
    {
        _led0 = 1 - _led0;
        fprintf(stderr, "led 0 toggle, switch to %s\n", _led0 ? "on" : "off");
    }

    if (leds & LED_1)
    {
        _led1 = 1 - _led1;
        fprintf(stderr, "led 1 toggle, switch to %s\n", _led1 ? "on" : "off");
    }
}

/* ------------------------------------------------------------ */
/*                                                              */
/* ------------------------------------------------------------ */
