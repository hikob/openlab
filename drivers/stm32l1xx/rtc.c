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
 * rtc.c
 *
 *  Created on: Jul 7, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "rtc.h"
#include "rtc_registers.h"
#include "rcc_clock.h"
#include "rcc_rtc.h"
#include "pwr.h"
#include "exti.h"
#include "nvic.h"
#include "nvic_.h"

static void rtc_wakeup(handler_arg_t arg);
static handler_t wakeup_handler;

void rtc_enable()
{
    // Check if the RTC is started
    if (rcc_rtc_is_enabled())
    {
        // Yes, nothing to do
        return;
    }

    // No, enable it

    // Allow access to RTC registers
    pwr_disable_backup_write_protection();

    // Enable the RTC clock
    rcc_rtc_enable();

    // Reset the RTC domain
    rcc_rtc_reset();

    // Set the RTC clock
    rcc_rtc_set_clock_source(RCC_RTC_CLOCK_LSE);

    // block further access to RTC registers
    pwr_enable_backup_write_protection();
}

void rtc_disable()
{
    // Check if the RTC is enabled
    if (!rcc_rtc_is_enabled())
    {
        // No, nothing to do
        return;
    }

    // Disable it

    // Allow access to RTC registers
    pwr_disable_backup_write_protection();

    // Reset the RTC domain
    rcc_rtc_reset();

    // Enable the RTC clock
    rcc_rtc_disable();

    // block further access to RTC registers
    pwr_enable_backup_write_protection();
}

void rtc_calendar_init(uint16_t year, uint8_t month, uint8_t day,
                       uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    // Allow access to RTC registers
    pwr_disable_backup_write_protection();

    //! \todo something needs to go here

    // block further access to RTC registers
    pwr_disable_backup_write_protection();
}

void rtc_set_periodic_wakeup(rtc_wakeup_clock_t clock, uint16_t period,
                             handler_t handler, handler_arg_t arg)
{
    // Store handler
    wakeup_handler = handler;

    // Allow access to RTC registers
    pwr_disable_backup_write_protection();

    // Unlock the RTC Write protection
    rtc_unlock_write_protection();

    // Disable the Wakeup Timer
    *rtc_get_CR() &= ~RTC_CR__WUTE;

    // Wait for access cleared to Wakeup timer
    while ((*rtc_get_ISR() & RTC_ISR__WUTWF) == 0)
    {
        ;
    }

    // Program the value in the Wakeup Timer
    *rtc_get_WUTR() = period;

    // Select desired clock source
    *rtc_get_CR() &= ~RTC_CR__WUCKSEL_MASK;
    *rtc_get_CR() |= clock;

    // Re-enable the Wakeup Timer and associated interrupt
    *rtc_get_CR() |= RTC_CR__WUTE | RTC_CR__WUTIE;

    // Clear WUTF flag
    *rtc_get_ISR() &= ~RTC_ISR__WUTF;

    // Enable the RTC write protection
    rtc_lock_write_protection();

    // Enable pwr write protection
    pwr_enable_backup_write_protection();

    // Enable the interrupt
    // Set EXTI handler
    exti_set_handler(EXTI_LINE_RTC_WAKEUP, rtc_wakeup, arg);

    // Configure EXTI line for WakeUp in interrupt mode, rising edge
    exti_enable_interrupt_line(EXTI_LINE_RTC_WAKEUP, EXTI_TRIGGER_RISING);

    // Enable RTC_WKUP IRQ in the NVIC
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_RTC_WKUP);
}
static void rtc_wakeup(handler_arg_t arg)
{
    // Disable RTC write protection in PWR
    pwr_disable_backup_write_protection();

    // Clear WUTF to enable next interrupt
    *rtc_get_ISR() &= ~RTC_ISR__WUTF;

    // Enable back the protection
    pwr_enable_backup_write_protection();

    // Call handler if any
    if (wakeup_handler)
    {
        wakeup_handler(arg);
    }
}
uint8_t rtc_get_seconds_units()
{
    return (uint8_t)(*rtc_get_TR() & 0xF);
}
uint8_t rtc_get_seconds_tens()
{
    return (uint8_t)((*rtc_get_TR() >> 4) & 0xF);
}
uint8_t rtc_get_minutes_units()
{
    return (uint8_t)((*rtc_get_TR() >> 8) & 0xF);
}
uint8_t rtc_get_minutes_tens()
{
    return (uint8_t)((*rtc_get_TR() >> 12) & 0xF);
}

uint8_t rtc_get_hours_units()
{
    return (uint8_t)((*rtc_get_TR() >> 16) & 0xF);
}
uint8_t rtc_get_hours_tens()
{
    return (uint8_t)((*rtc_get_TR() >> 20) & 0x3);
}
