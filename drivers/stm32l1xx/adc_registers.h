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
 * adc_registers.h
 *
 *  Created on: Oct 11, 2011
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
    return mem_get_reg32(_adc->base_address + ADC_SR_OFFSET);
}
static inline volatile uint32_t *adc_get_CR1(const _adc_t *_adc)
{
    return mem_get_reg32(_adc->base_address + ADC_CR1_OFFSET);
}
static inline volatile uint32_t *adc_get_CR2(const _adc_t *_adc)
{
    return mem_get_reg32(_adc->base_address + ADC_CR2_OFFSET);
}
static inline volatile uint32_t *adc_get_SMPRx(const _adc_t *_adc, uint8_t smpr)
{
    return mem_get_reg32(_adc->base_address + ADC_SMPR1_OFFSET + 4 * (smpr - 1));
}
static inline volatile uint32_t *adc_get_JOFRx(const _adc_t *_adc, uint8_t jofr)
{
    return mem_get_reg32(_adc->base_address + ADC_JOFR1_OFFSET + 4 * (jofr - 1));
}
static inline volatile uint32_t *adc_get_HTR(const _adc_t *_adc)
{
    return mem_get_reg32(_adc->base_address + ADC_HTR_OFFSET);
}
static inline volatile uint32_t *adc_get_LTR(const _adc_t *_adc)
{
    return mem_get_reg32(_adc->base_address + ADC_LTR_OFFSET);
}
static inline volatile uint32_t *adc_get_SQR1(const _adc_t *_adc)
{
    return mem_get_reg32(_adc->base_address + ADC_SQR1_OFFSET);
}
static inline volatile uint32_t *adc_get_SQRx(const _adc_t *_adc, uint8_t sqr)
{
    return mem_get_reg32(_adc->base_address + ADC_SQR1_OFFSET + 4 * (sqr - 1));
}
static inline volatile uint32_t *adc_get_JSQR(const _adc_t *_adc)
{
    return mem_get_reg32(_adc->base_address + ADC_JSQR_OFFSET);
}
static inline volatile uint32_t *adc_get_JDRx(const _adc_t *_adc, uint8_t jdr)
{
    return mem_get_reg32(_adc->base_address + ADC_JDR1_OFFSET + 4 * (jdr - 1));
}
static inline volatile uint32_t *adc_get_DR(const _adc_t *_adc)
{
    return mem_get_reg32(_adc->base_address + ADC_DR_OFFSET);
}
static inline volatile uint32_t *adc_get_CSR()
{
    return mem_get_reg32(ADC_BASE_ADDRESS + ADC_CSR_OFFSET);
}
static inline volatile uint32_t *adc_get_CCR()
{
    return mem_get_reg32(ADC_BASE_ADDRESS + ADC_CCR_OFFSET);
}

static inline volatile uint16_t* adc_get_VREFINT_FACTORY()
{
    return mem_get_reg16(0x1FF80078);
}

enum
{
    ADC_SR__AWD = 0x0001,
    ADC_SR__EOC = 0x0002,
    ADC_SR__JEOC = 0x0004,
    ADC_SR__JSTRT = 0x0008,
    ADC_SR__STRT = 0x0010,
    ADC_SR__OVR = 0x0020,
    ADC_SR__ADONS = 0x0040,
    ADC_SR__RCNR = 0x0100,
    ADC_SR__JCNR = 0x0200
};

enum
{
    ADC_CR1__AWDCH_MASK = 0x0000001F,
    ADC_CR1__EOCIE = 0x00000020,
    ADC_CR1__AWDIE = 0x00000040,
    ADC_CR1__JEOCIE = 0x00000080,
    ADC_CR1__SCAN = 0x00000100,
    ADC_CR1__AWDSGL = 0x00000200,
    ADC_CR1__JAUTO = 0x00000400,
    ADC_CR1__DISCEN = 0x00000800,
    ADC_CR1__JDISCEN = 0x00001000,
    ADC_CR1__DISCNUM_MASK = 0x0000E000,

    ADC_CR1__PDD = 0x00010000,
    ADC_CR1__PDI = 0x00020000,
    ADC_CR1_JAWDEN = 0x00200000,
    ADC_CR1__AWDEN = 0x00400000,
    ADC_CR1__RES_MASK = 0x01800000,
    ADC_CR1__OVRIE = 0x08000000
};

enum
{
    ADC_CR2__ADON = 0x00000001,
    ADC_CR2__CONT = 0x00000002,
    ADC_CR2__DELS_MASK = 0x00000070,
    ADC_CR2__DMA = 0x00000100,
    ADC_CR2__DDS = 0x00000200,
    ADC_CR2__EOCS = 0x00000400,
    ADC_CR2__ALIGN = 0x00000800,

    ADC_CR2__JEXTSEL_MASK = 0x000F0000,
    ADC_CR2__JEXTEN_MASK = 0x00300000,
    ADC_CR2__JSWSTART = 0x00400000,
    ADC_CR2__EXTSEL_MASK = 0x0F000000,
    ADC_CR2__EXTEN_MASK = 0x30000000,
    ADC_CR2__SWSTART = 0x40000000
};

enum
{
    ADC_SMPRx__SMP0_MASK = 0x00000007,
    ADC_SMPRx__SMP1_MASK = 0x00000038,
    ADC_SMPRx__SMP2_MASK = 0x000001C0,
    ADC_SMPRx__SMP3_MASK = 0x00000E00,
    ADC_SMPRx__SMP4_MASK = 0x00007000,
    ADC_SMPRx__SMP5_MASK = 0x00038000,
    ADC_SMPRx__SMP6_MASK = 0x001C0000,
    ADC_SMPRx__SMP7_MASK = 0x00E00000,
    ADC_SMPRx__SMP8_MASK = 0x07000000,
    ADC_SMPRx__SMP9_MASK = 0x38000000,
};

enum
{
    ADC_HTR__HT_MASK = 0x00000FFF
};
enum
{
    ADC_LTR__LT_MASK = 0x00000FFF
};

enum
{
    ADC_SQR1__SQ25_MASK = 0x0000001F,
    ADC_SQR1__SQ26_MASK = 0x000003E0,
    ADC_SQR1__SQ27_MASK = 0x00007C00,

    ADC_SQR1__L_MASK = 0x01F00000,
};

enum
{
    ADC_SQR2__SQ19_MASK = 0x0000001F,
    ADC_SQR2__SQ20_MASK = 0x000003E0,
    ADC_SQR2__SQ21_MASK = 0x00007C00,
    ADC_SQR2__SQ22_MASK = 0x000F8000,
    ADC_SQR2__SQ23_MASK = 0x01F00000,
    ADC_SQR2__SQ24_MASK = 0x3E000000,
};
enum
{
    ADC_SQR3__SQ13_MASK = 0x0000001F,
    ADC_SQR3__SQ14_MASK = 0x000003E0,
    ADC_SQR3__SQ15_MASK = 0x00007C00,
    ADC_SQR3__SQ16_MASK = 0x000F8000,
    ADC_SQR3__SQ17_MASK = 0x01F00000,
    ADC_SQR3__SQ18_MASK = 0x3E000000,
};
enum
{
    ADC_SQR4__SQ7_MASK = 0x0000001F,
    ADC_SQR4__SQ8_MASK = 0x000003E0,
    ADC_SQR4__SQ9_MASK = 0x00007C00,
    ADC_SQR4__SQ10_MASK = 0x000F8000,
    ADC_SQR4__SQ11_MASK = 0x01F00000,
    ADC_SQR4__SQ12_MASK = 0x3E000000,
};
enum
{
    ADC_SQR5__SQ1_MASK = 0x0000001F,
    ADC_SQR5__SQ2_MASK = 0x000003E0,
    ADC_SQR5__SQ3_MASK = 0x00007C00,
    ADC_SQR5__SQ4_MASK = 0x000F8000,
    ADC_SQR5__SQ5_MASK = 0x01F00000,
    ADC_SQR5__SQ6_MASK = 0x3E000000,
};
enum
{
    ADC_JSQR__JSQ1_MASK = 0x0000001F,
    ADC_JSQR__JSQ2_MASK = 0x000003E0,
    ADC_JSQR__JSQ3_MASK = 0x00007C00,
    ADC_JSQR__JSQ4_MASK = 0x000F8000,
    ADC_JSQR__JL_MASK = 0x00300000,
};
enum
{
    ADC_JDRx__DATA_MASK = 0x0000FFFF
};
enum
{
    ADC_DR__DATA_MASK = 0x0000FFFF
};
enum
{
    ADC_CSR__AWD1 = 0x0001,
    ADC_CSR__EOC1 = 0x0002,
    ADC_CSR__JEOC1 = 0x0004,
    ADC_CSR__JSTRT1 = 0x0008,
    ADC_CSR__STRT1 = 0x0010,
    ADC_CSR__OVR1 = 0x0020,
    ADC_CSR__ADONS1 = 0x0040,
};
enum
{
    ADC_CCR__ADCPRE_MASK = 0x00030000,
    ADC_CCR__TSVREFE = 0x00800000,
};

#endif /* ADC_REGISTERS_H_ */
