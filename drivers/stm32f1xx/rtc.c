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
 * rtc.c
 *
 *  Created on: May 28, 2013
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdint.h>

#include "rtc.h"
#include "rtc_registers.h"
#include "rcc_registers.h"
#include "pwr.h"
#include "nvic_.h"
#include "exti.h"

#include "debug.h"

static struct
{
    handler_t sec_handler;
    handler_arg_t sec_arg;
} rtc =
{ .sec_handler = NULL};

static void wait_sync()
{
    *rtc_get_CRL() &= ~RTC_CRL__RSF;
    while ((*rtc_get_CRL() & RTC_CRL__RSF) == 0)
        ;
}

static void enter_config()
{
    /* Loop until RTOFF flag is set */
    while ((*rtc_get_CRL() & RTC_CRL__RTOFF) == 0)
        ;

    *rtc_get_CRL() |= RTC_CRL__CNF;
}

static void exit_config()
{
    *rtc_get_CRL() &= ~RTC_CRL__CNF;

    /* Loop until RTOFF flag is set */
    while ((*rtc_get_CRL() & RTC_CRL__RTOFF) == 0)
        ;
}

void rtc_enable()
{
    pwr_enable_backup_write_protection();

    // Select source
    *rcc_get_BDCR() &= ~RCC_BDCR__RTCSEL_MASK;
    *rcc_get_BDCR() |= 0x1 << 8;

    // enable
    *rcc_get_BDCR() |= RCC_BDCR__RTCEN;

    // Wait all
    wait_sync();

    /* Loop until RTOFF flag is set */
    while ((*rtc_get_CRL() & RTC_CRL__RTOFF) == 0)
        ;

    *rtc_get_CRH() &= ~(RTC_CRH__ALRIE | RTC_CRH__SECIE| RTC_CRH__OWIE);

    // Enable NVIC
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_RTC);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_RTC_ALARM);
}

void rtc_disable()
{
    // Clear source
    *rcc_get_BDCR() &= ~RCC_BDCR__RTCSEL_MASK;

    // Disable
    *rcc_get_BDCR() &= ~RCC_BDCR__RTCEN;
}

uint32_t rtc_time()
{
    return (*rtc_get_CNTH() << 16) + *rtc_get_CNTL();
}

void rtc_set_time(uint32_t time)
{
    enter_config();

    *rtc_get_CNTH() = (time >> 16);
    *rtc_get_CNTL() = time & 0xFFFF;

    exit_config();
}

void rtc_set_alarm(uint32_t time, handler_t handler, handler_arg_t arg)
{
    enter_config();

    /* Set the ALARM MSB word */
    *rtc_get_ALRH() = (time >> 16);

    /* Set the ALARM LSB word */
    *rtc_get_ALRL() = time & 0xFFFF;

    exit_config();

    if (handler)
    {
        // Enable interrupt in EXTI
        exti_enable_interrupt_line(EXTI_LINE_RTC_ALARM, EXTI_TRIGGER_RISING);
        exti_set_handler(EXTI_LINE_RTC_ALARM, handler, arg);
    }
    else
    {
        exti_disable_interrupt_line(EXTI_LINE_RTC_ALARM);
    }
}

void rtc_enable_second_interrupt(handler_t handler, handler_arg_t arg)
{
    // Store
    rtc.sec_handler = handler;
    rtc.sec_arg = arg;

    if (handler)
    {
        // RTC second interrupt
        *rtc_get_CRH() |= RTC_CRH__SECIE;
        *rtc_get_CRL() &= ~RTC_CRL__SECF;
    }
    else
    {
        // RTC second interrupt
        *rtc_get_CRH() &= ~RTC_CRH__SECIE;
    }
}
void rtc_isr()
{
    // Check source
    if (*rtc_get_CRL() & *rtc_get_CRH() & RTC_CRL__SECF)
    {
        // Second!
        *rtc_get_CRL() = ((*rtc_get_CRL()) & 0xF0) | (~RTC_CRL__SECF & 0x0F);

        if (rtc.sec_handler)
        {
            rtc.sec_handler(rtc.sec_arg);
        }
    }
    if (*rtc_get_CRL() & *rtc_get_CRH() & RTC_CRL__OWF)
    {
        // Overflow!
        log_debug("Overflow");
        // Clear flag
        *rtc_get_CRL() = ((*rtc_get_CRL()) & 0xF0) | (~RTC_CRL__OWF & 0x0F);
    }
}
