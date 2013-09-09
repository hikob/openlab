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
 * adc_registers.h
 *
 *  Created on: Nov 2, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef ADC_REGISTERS_H_
#define ADC_REGISTERS_H_

#include "memmap.h"
#include "adc_.h"

/*
 * All registers must be accessed by words (32bit)
 */

static inline volatile uint32_t *adc_get_SR(const _adc_t *_adc)
{
    return mem_get_reg32(_adc->base_address + ADCx_SR_OFFSET);
}
static inline volatile uint32_t *adc_get_CR1(const _adc_t *_adc)
{
    return mem_get_reg32(_adc->base_address + ADCx_CR1_OFFSET);
}
static inline volatile uint32_t *adc_get_CR2(const _adc_t *_adc)
{
    return mem_get_reg32(_adc->base_address + ADCx_CR2_OFFSET);
}
static inline volatile uint32_t *adc_get_SMPRx(const _adc_t *_adc, uint8_t smpr)
{
    return mem_get_reg32(_adc->base_address + ADCx_SMPR1_OFFSET + 4
                         * (smpr - 1));
}
static inline volatile uint32_t *adc_get_JOFRx(const _adc_t *_adc, uint8_t jofr)
{
    return mem_get_reg32(_adc->base_address + ADCx_JOFR1_OFFSET + 4
                         * (jofr - 1));
}
static inline volatile uint32_t *adc_get_HTR(const _adc_t *_adc)
{
    return mem_get_reg32(_adc->base_address + ADCx_HTR_OFFSET);
}
static inline volatile uint32_t *adc_get_LTR(const _adc_t *_adc)
{
    return mem_get_reg32(_adc->base_address + ADCx_LTR_OFFSET);
}
static inline volatile uint32_t *adc_get_SQR1(const _adc_t *_adc)
{
    return mem_get_reg32(_adc->base_address + ADCx_SQR1_OFFSET);
}
static inline volatile uint32_t *adc_get_SQRx(const _adc_t *_adc, uint8_t sqr)
{
    return mem_get_reg32(_adc->base_address + ADCx_SQR1_OFFSET + 4 * (sqr - 1));
}
static inline volatile uint32_t *adc_get_JSQR(const _adc_t *_adc)
{
    return mem_get_reg32(_adc->base_address + ADCx_JSQR_OFFSET);
}
static inline volatile uint32_t *adc_get_JDRx(const _adc_t *_adc, uint8_t jdr)
{
    return mem_get_reg32(_adc->base_address + ADCx_JDR1_OFFSET + 4 * (jdr - 1));
}
static inline volatile uint32_t *adc_get_DR(const _adc_t *_adc)
{
    return mem_get_reg32(_adc->base_address + ADCx_DR_OFFSET);
}

enum
{
    ADCx_SR__AWD = 0x0001,
    ADCx_SR__EOC = 0x0002,
    ADCx_SR__JEOC = 0x0004,
    ADCx_SR__JSTRT = 0x0008,
    ADCx_SR__STRT = 0x0010,
};

enum
{
    ADCx_CR1__AWDCH_MASK = 0x0000001F,
    ADCx_CR1__EOCIE = 0x00000020,
    ADCx_CR1__AWDIE = 0x00000040,
    ADCx_CR1__JEOCIE = 0x00000080,
    ADCx_CR1__SCAN = 0x00000100,
    ADCx_CR1__AWDSGL = 0x00000200,
    ADCx_CR1__JAUTO = 0x00000400,
    ADCx_CR1__DISCEN = 0x00000800,
    ADCx_CR1__JDISCEN = 0x00001000,
    ADCx_CR1__DISCNUM_MASK = 0x0000E000,
    ADCx_CR1__DUALMOD_MASK = 0x000F0000,

    ADCx_CR1_JAWDEN = 0x00400000,
    ADCx_CR1__AWDEN = 0x00800000,
};

enum
{
    ADCx_CR2__ADON = 0x00000001,
    ADCx_CR2__CONT = 0x00000002,
    ADCx_CR2__CAL = 0x00000004,
    ADCx_CR2__RSTCAL = 0x00000008,

    ADCx_CR2__DMA = 0x00000100,

    ADCx_CR2__ALIGN = 0x00000800,

    ADCx_CR2__JEXTSEL_MASK = 0x00007000,
    ADCx_CR2__JEXTTRIG = 0x00008000,

    ADCx_CR2__EXTSEL_MASK = 0x000E0000,
    ADCx_CR2__EXTTRIG = 0x00100000,
    ADCx_CR2__JSWSTART = 0x00200000,
    ADCx_CR2__SWSTART = 0x00400000,
    ADCx_CR2__TSVREFE = 0x00800000,
};

enum
{
    ADCx_SMPRx__SMP0_MASK = 0x00000007,
    ADCx_SMPRx__SMP1_MASK = 0x00000038,
    ADCx_SMPRx__SMP2_MASK = 0x000001C0,
    ADCx_SMPRx__SMP3_MASK = 0x00000E00,
    ADCx_SMPRx__SMP4_MASK = 0x00007000,
    ADCx_SMPRx__SMP5_MASK = 0x00038000,
    ADCx_SMPRx__SMP6_MASK = 0x001C0000,
    ADCx_SMPRx__SMP7_MASK = 0x00E00000,
    ADCx_SMPRx__SMP8_MASK = 0x07000000,
    ADCx_SMPRx__SMP9_MASK = 0x38000000,
};

enum
{
    ADCx_HTR__HT_MASK = 0x00000FFF
};
enum
{
    ADCx_LTR__LT_MASK = 0x00000FFF
};

enum
{
    ADCx_SQR1__SQ13_MASK = 0x0000001F,
    ADCx_SQR1__SQ14_MASK = 0x000003E0,
    ADCx_SQR1__SQ15_MASK = 0x00007C00,
    ADCx_SQR1__SQ16_MASK = 0x000F8000,

    ADCx_SQR1__L_MASK = 0x00F00000,
};

enum
{
    ADCx_SQR2__SQ7_MASK = 0x0000001F,
    ADCx_SQR2__SQ8_MASK = 0x000003E0,
    ADCx_SQR2__SQ9_MASK = 0x00007C00,
    ADCx_SQR2__SQ10_MASK = 0x000F8000,
    ADCx_SQR2__SQ11_MASK = 0x01F00000,
    ADCx_SQR2__SQ12_MASK = 0x3E000000,
};
enum
{
    ADCx_SQR3__SQ1_MASK = 0x0000001F,
    ADCx_SQR3__SQ2_MASK = 0x000003E0,
    ADCx_SQR3__SQ3_MASK = 0x00007C00,
    ADCx_SQR3__SQ4_MASK = 0x000F8000,
    ADCx_SQR3__SQ5_MASK = 0x01F00000,
    ADCx_SQR3__SQ6_MASK = 0x3E000000,
};
enum
{
    ADCx_JSQR__JSQ1_MASK = 0x0000001F,
    ADCx_JSQR__JSQ2_MASK = 0x000003E0,
    ADCx_JSQR__JSQ3_MASK = 0x00007C00,
    ADCx_JSQR__JSQ4_MASK = 0x000F8000,
    ADCx_JSQR__JL_MASK = 0x00300000,
};
enum
{
    ADCx_JDRx__DATA_MASK = 0x0000FFFF
};
enum
{
    ADCx_DR__DATA_MASK = 0x0000FFFF
};
enum
{
    ADCx_CSR__AWD1 = 0x0001,
    ADCx_CSR__EOC1 = 0x0002,
    ADCx_CSR__JEOC1 = 0x0004,
    ADCx_CSR__JSTRT1 = 0x0008,
    ADCx_CSR__STRT1 = 0x0010,
    ADCx_CSR__OVR1 = 0x0020,
};
enum
{
    ADCx_CCR__ADCPRE_MASK = 0x00030000,
    ADCx_CCR__TSVREFE = 0x00800000,
};

#endif /* ADC_REGISTERS_H_ */
