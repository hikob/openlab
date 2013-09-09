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
 * timer_registers.h
 *
 *  Created on: Jul 6, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef TIMER_REGISTERS_H_
#define TIMER_REGISTERS_H_

#include "memmap.h"
#include "timer_.h"

static inline volatile uint16_t *timer_get_CR1(const _timer_t *timer)
{
    return mem_get_reg16(timer->base_address + TIMx_CR1_OFFSET);
}
static inline volatile uint16_t *timer_get_CR2(const _timer_t *timer)
{
    return mem_get_reg16(timer->base_address + TIMx_CR2_OFFSET);
}
static inline volatile uint16_t *timer_get_SMCR(const _timer_t *timer)
{
    return mem_get_reg16(timer->base_address + TIMx_SMCR_OFFSET);
}
static inline volatile uint16_t *timer_get_DIER(const _timer_t *timer)
{
    return mem_get_reg16(timer->base_address + TIMx_DIER_OFFSET);
}
static inline volatile uint32_t *timer_get_DIER_bitband(const _timer_t *timer, uint32_t bit)
{
    return mem_get_bitband(timer->base_address + TIMx_DIER_OFFSET, bit);
}
static inline volatile uint16_t *timer_get_SR(const _timer_t *timer)
{
    return mem_get_reg16(timer->base_address + TIMx_SR_OFFSET);
}
static inline volatile uint32_t *timer_get_SR_bitband(const _timer_t *timer, uint32_t bit)
{
    return mem_get_bitband(timer->base_address + TIMx_SR_OFFSET, bit);
}
static inline volatile uint16_t *timer_get_EGR(const _timer_t *timer)
{
    return mem_get_reg16(timer->base_address + TIMx_EGR_OFFSET);
}
static inline volatile uint16_t *timer_get_CCMRx(const _timer_t *timer, uint32_t ccmr)
{
    if (ccmr == 1)
    {
        return mem_get_reg16(timer->base_address + TIMx_CCMR1_OFFSET);
    }
    else
    {
        return mem_get_reg16(timer->base_address + TIMx_CCMR2_OFFSET);
    }
}
static inline volatile uint16_t *timer_get_CCER(const _timer_t *timer)
{
    return mem_get_reg16(timer->base_address + TIMx_CCER_OFFSET);
}
static inline volatile uint32_t *timer_get_CNT(const _timer_t *timer)
{
    return mem_get_reg32(timer->base_address + TIMx_CNT_OFFSET);
}
static inline volatile uint16_t *timer_get_PSC(const _timer_t *timer)
{
    return mem_get_reg16(timer->base_address + TIMx_PSC_OFFSET);
}
static inline volatile uint32_t *timer_get_ARR(const _timer_t *timer)
{
    return mem_get_reg32(timer->base_address + TIMx_ARR_OFFSET);
}
static inline volatile uint32_t *timer_get_CCRx(const _timer_t *timer,
        uint32_t channel)
{
    return mem_get_reg32(timer->base_address + TIMx_CCR1_OFFSET + (4 * channel));
}
#if defined(TIMx_BDTR_OFFSET)
static inline volatile uint32_t *timer_get_BDTR(const _timer_t *timer)
{
    return mem_get_reg32(timer->base_address + TIMx_BDTR_OFFSET);
}
#endif

// Bit descriptions
enum
{
    TIMER_CR1__CKD_MASK = 0x0300,
    TIMER_CR1__ARPE = 0x0080,
    TIMER_CR1__CMS_MASK = 0x0060,
    TIMER_CR1__DIR = 0x0010,
    TIMER_CR1__OPM = 0x0008,
    TIMER_CR1__URS = 0x0004,
    TIMER_CR1__UDIS = 0x0002,
    TIMER_CR1__CEN = 0x0001,
};

enum
{
    TIMER_DIER__TDE = 0x4000,
    TIMER_DIER__CC4DE = 0x1000,
    TIMER_DIER__CC3DE = 0x0800,
    TIMER_DIER__CC2DE = 0x0400,
    TIMER_DIER__CC1DE = 0x0200,
    TIMER_DIER__UDE = 0x0100,
    TIMER_DIER__TIE = 0x0040,
    TIMER_DIER__CC4IE = 0x0010,
    TIMER_DIER__CC3IE = 0x0008,
    TIMER_DIER__CC2IE = 0x0004,
    TIMER_DIER__CC1IE = 0x0002,
    TIMER_DIER__UIE = 0x0001,

    TIMER_DIER__TDE_bit = 14,
    TIMER_DIER__CC4DE_bit = 12,
    TIMER_DIER__CC3DE_bit = 11,
    TIMER_DIER__CC2DE_bit = 10,
    TIMER_DIER__CC1DE_bit = 9,
    TIMER_DIER__UDE_bit = 8,
    TIMER_DIER__TIE_bit = 5,
    TIMER_DIER__CC4IE_bit = 4,
    TIMER_DIER__CC3IE_bit = 3,
    TIMER_DIER__CC2IE_bit = 2,
    TIMER_DIER__CC1IE_bit = 1,
    TIMER_DIER__UIE_bit = 0,
};

enum
{
    TIMER_SR__TIF = 0x0040,
    TIMER_SR__CC4IF = 0x0010,
    TIMER_SR__CC3IF = 0x0008,
    TIMER_SR__CC2IF = 0x0004,
    TIMER_SR__CC1IF = 0x0002,
    TIMER_SR__UIF = 0x0001,

    TIMER_SR__TIF_bit = 5,
    TIMER_SR__CC4IF_bit = 4,
    TIMER_SR__CC3IF_bit = 3,
    TIMER_SR__CC2IF_bit = 2,
    TIMER_SR__CC1IF_bit = 1,
    TIMER_SR__UIF_bit = 0,
};

enum
{
    TIMER_SMCR__ETP = 0x8000,
    TIMER_SMCR__ECE = 0x4000,
    TIMER_SMCR__ETPS_MASK = 0x3000,
    TIMER_SMCR__ETF_MASK = 0x0F00,
    TIMER_SMCR__MSM = 0x0080,
    TIMER_SMCR__TS_MASK = 0x0070,

    TIMER_SMCR__SMS_MASK = 0x0007,
    TIMER_SMCR__SMS_OFF = 0x0000,
    TIMER_SMCR__SMS_ENCODER_MODE_1 = 0x0001,
    TIMER_SMCR__SMS_ENCODER_MODE_2 = 0x0002,
    TIMER_SMCR__SMS_ENCODER_MODE_3 = 0x0003,
    TIMER_SMCR__SMS_RESET_MODE = 0x0004,
    TIMER_SMCR__SMS_GATED_MODE = 0x0005,
    TIMER_SMCR__SMS_TRIGGER_MODE = 0x0006,
    TIMER_SMCR__SMS_EXTERNAL_CLOCK_MODE_1 = 0x0007,
};

enum
{
    // Mode selection
    TIMER_CCMRx__CCxS__MASK = 0x03,
    TIMER_CCMRx__CCxS__OUTPUT = 0x00,
    TIMER_CCMRx__CCxS__INPUT1 = 0x01,
    TIMER_CCMRx__CCxS__INPUT2 = 0x02,
    TIMER_CCMRx__CCxS__INPUT3 = 0x03,

    // Output mode
    TIMER_CCMRx__OCxFE = 0x04,
    TIMER_CCMRx__OCxPE = 0x08,
    TIMER_CCMRx__OCxM__MASK = 0x70,
    TIMER_CCMRx__OCxM__FROZEN = 0x00,
    TIMER_CCMRx__OCxM__SET_ACTIVE = 0x10,
    TIMER_CCMRx__OCxM__SET_INACTIVE = 0x20,
    TIMER_CCMRx__OCxM__TOGGLE = 0x30,
    TIMER_CCMRx__OCxM__FORCE_INACTIVE = 0x40,
    TIMER_CCMRx__OCxM__FORCE_ACTIVE = 0x50,
    TIMER_CCMRx__OCxM__PWM_MODE1 = 0x60,
    TIMER_CCMRx__OCxM__PWM_MODE2 = 0x70,
    TIMER_CCMRx__OCxCE = 0x80,

    // Input mode
    TIMER_CCMRx__ICxPSC__MASK = 0x0C,
    TIMER_CCMRx__ICxF__MASK = 0xF0,
};

enum
{
    TIMER_CCER__CC1NP = 0x8,
    TIMER_CCER__CC1P = 0x2,
    TIMER_CCER__CC1E = 0x1,

    TIMER_CCER__CC1_MASK = 0xF,
};

enum
{
    TIMER_EGR__UG = 0x01,
    TIMER_EGR__CC1G = 0x02,
    TIMER_EGR__CC2G = 0x04,
    TIMER_EGR__CC3G = 0x08,
    TIMER_EGR__CC4G = 0x10,
    TIMER_EGR__TG = 0x40,
};

enum
{
    TIMER_BDTR__MOE = 0x8000
};

#endif /* TIMER_REGISTERS_H_ */
