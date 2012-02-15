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
 * delay.c
 *
 *  Created on: Jul 8, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>
#include <stddef.h>
#include "delay.h"
#include "timer.h"

#include "printf.h"

static struct
{
    timer_t timer;
} delay;

static int a_before_b(uint16_t a, uint16_t b)
{
    int16_t delta;
    delta = b - a;

    // Delta is the remaining time
    return delta > 0;
}

void delay_init(timer_t timer)
{
    // Store timer
    delay.timer = timer;
}

void delay_us(uint16_t us)
{
    // Loop until delay elapsed
    uint16_t alarm = timer_time(delay.timer) + 1;

    // Add the tick count representing the time
    alarm += timer_get_frequency(delay.timer) * us / 1000000;

    // Loop until alarm is reached
    while(a_before_b(timer_time(delay.timer), alarm))
    {
        __asm__ __volatile__("nop");
    }
}
void delay_ms(uint16_t ms)
{
    while(ms--)
    {
        delay_us(1000);
    }
}
void delay_s(uint16_t s)
{
    while(s--)
    {
        delay_ms(1000);
    }
}
