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
 * memmap.h
 *
 *  Created on: Jul 12, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef MEMMAP_H_
#define MEMMAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

    /* Useful macros */
#ifndef BV
#define BV(x) (1 << (x))
#endif

    static inline volatile uint32_t *mem_get_reg32(uint32_t addr)
    {
        return (volatile uint32_t *) addr;
    }

    static inline volatile uint16_t *mem_get_reg16(uint32_t addr)
    {
        return (volatile uint16_t *) addr;
    }

    static inline volatile uint8_t *mem_get_reg8(uint32_t addr)
    {
        return (volatile uint8_t *) addr;
    }

    /* RCC section */
#define RCC_BASE_ADDRESS        0x40021000

#define RCC_CR_OFFSET           0x00
#define RCC_CFGR_OFFSET         0x04
#define RCC_CIR_OFFSET          0x08
#define RCC_APB2RSTR_OFFSET     0x0C
#define RCC_APB1RSTR_OFFSET     0x10
#define RCC_AHBENR_OFFSET       0x14
#define RCC_APB2ENR_OFFSET      0x18
#define RCC_APB1ENR_OFFSET      0x1C
#define RCC_BDCR_OFFSET         0x20
#define RCC_CSR_OFFSET          0x24
#define RCC_AHBRSTR_OFFSET              0x28
#define RCC_CFGR2_OFFSET        0x2C

    /* AFIO section */
#define AFIO_BASE_ADDRESS               0x40010000
#define AFIO_EVCR_OFFSET                0x00
#define AFIO_MAPR_OFFSET                0x04
#define AFIO_EXTICR1_OFFSET             0x08
#define AFIO_EXTICR2_OFFSET             0x0C
#define AFIO_EXTICR3_OFFSET             0x10
#define AFIO_EXTICR4_OFFSET             0x14
#define AFIO_MAPR2_OFFSET               0x1C

    /* GPIO section */
#define GPIO_BASE_ADDRESS       0x40010800
#define GPIOx_OFFSET            0x0400
#define GPIOA_OFFSET            0x0000
#define GPIOB_OFFSET            0x0400
#define GPIOC_OFFSET            0x0800
#define GPIOD_OFFSET            0x0C00
#define GPIOE_OFFSET            0x1000
#define GPIOF_OFFSET            0x1400
#define GPIOG_OFFSET            0x1800

    // Offsets
#define GPIOx_CRL_OFFSET                0x00
#define GPIOx_CRH_OFFSET                0x04
#define GPIOx_IDR_OFFSET                0x08
#define GPIOx_ODR_OFFSET                0x0C
#define GPIOx_BSRR_OFFSET               0x10

    /* TIMERs section */
#define TIM1_BASE_ADDRESS               0x40012C00
#define TIM2_BASE_ADDRESS               0x40000000
#define TIM3_BASE_ADDRESS               0x40000400
#define TIM4_BASE_ADDRESS               0x40000800
#define TIM5_BASE_ADDRESS               0x40000C00
#define TIM6_BASE_ADDRESS               0x40001000
#define TIM7_BASE_ADDRESS               0x40001400
#define TIM8_BASE_ADDRESS               0x40013400

    // Offsets
#define TIMx_CR1_OFFSET                 0x00
#define TIMx_CR2_OFFSET                 0x04
#define TIMx_SMCR_OFFSET                0x08
#define TIMx_DIER_OFFSET                0x0C
#define TIMx_SR_OFFSET                  0x10
#define TIMx_EGR_OFFSET                 0x14
#define TIMx_CCMR1_OFFSET               0x18
#define TIMx_CCMR2_OFFSET               0x1C
#define TIMx_CCER_OFFSET                0x20
#define TIMx_CNT_OFFSET                 0x24
#define TIMx_PSC_OFFSET                 0x28
#define TIMx_ARR_OFFSET                 0x2C
#define TIMx_CCR1_OFFSET                0x34
#define TIMx_CCR2_OFFSET                0x38
#define TIMx_CCR3_OFFSET                0x3C
#define TIMx_CCR4_OFFSET                0x40
#define TIMx_DCR_OFFSET                 0x48
#define TIMx_DMAR_OFFSET                0x4C
#define TIMx_OR_OFFSET                  0x50

    /* USART section */
#define USART1_BASE_ADDRESS     0x40013800
#define USART2_BASE_ADDRESS     0x40004400
#define USART3_BASE_ADDRESS     0x40004800
#define USART4_BASE_ADDRESS     0x40004C00
#define USART5_BASE_ADDRESS     0x40005000

    // Offsets
#define USARTx_SR_OFFSET        0x00
#define USARTx_DR_OFFSET        0x04
#define USARTx_BRR_OFFSET       0x08
#define USARTx_CR1_OFFSET       0x0C
#define USARTx_CR2_OFFSET       0x10
#define USARTx_CR3_OFFSET       0x14
#define USARTx_GTPR_OFFSET      0x18

    /* SPI section */
#define SPI1_BASE_ADDRESS       0x40013000
#define SPI2_BASE_ADDRESS       0x40003800
#define SPI3_BASE_ADDRESS       0x40003C00

    // Offsets
#define SPIx_CR1_OFFSET         0x00
#define SPIx_CR2_OFFSET         0x04
#define SPIx_SR_OFFSET          0x08
#define SPIx_DR_OFFSET          0x0C
#define SPIx_CRCPR_OFFSET       0x10
#define SPIx_RXCRCR_OFFSET      0x14
#define SPIx_TXCRCR_OFFSET      0x18

    /* I2C section */
#define I2C1_BASE_ADDRESS       0x40005400
#define I2C2_BASE_ADDRESS       0x40005800

    // Offsets
#define I2Cx_CR1_OFFSET         0x00
#define I2Cx_CR2_OFFSET         0x04
#define I2Cx_OAR1_OFFSET        0x08
#define I2Cx_OAR2_OFFSET        0x0C
#define I2Cx_DR_OFFSET          0x10
#define I2Cx_SR1_OFFSET         0x14
#define I2Cx_SR2_OFFSET         0x18
#define I2Cx_CCR_OFFSET         0x1C
#define I2Cx_TRISE_OFFSET       0x20

    /* DMA section */
#define DMA1_BASE_ADDRESS       0x40020000
#define DMA2_BASE_ADDRESS       0x40020400

    // Offsets
#define DMA_ISR_OFFSET          0x0
#define DMA_IFCR_OFFSET         0x4
#define DMA_CHANNEL1_OFFSET     0x8
#define DMA_CHANNEL_OFFSET      0x14
#define DMA_CCRx_OFFSET         0x0
#define DMA_CNDTRx_OFFSET       0x4
#define DMA_CPARx_OFFSET        0x8
#define DMA_CMARx_OFFSET        0xC

    /* RTC section */
    //#define RTC_BASE_ADDRESS      0x40002800

    // Offsets
    //#define RTC_TR_OFFSET         0x0
    //#define RTC_DR_OFFSET         0x4
    //#define RTC_CR_OFFSET         0x8
    //#define RTC_ISR_OFFSET            0xC
    //#define RTC_PRER_OFFSET           0x10
    //#define RTC_WUTR_OFFSET           0x14
    //#define RTC_CALIBR_OFFSET     0x18
    //#define RTC_ALRMAR_OFFSET     0x1C
    //#define RTC_ALRMBR_OFFSET     0x20
    //#define RTC_WPR_OFFSET            0x24
    //#define RTC_TSTR_OFFSET           0x30
    //#define RTC_TSDR_OFFSET           0x34
    //#define RTC_TAFCR_OFFSET      0x40
    //#define RTC_BK0R_OFFSET           0x50

    /* SDIO section */
#define SDIO_BASE_ADDRESS               0x40018000

    // Offsets
#define SDIO_POWER_OFFSET               0x00
#define SDIO_CLKCR_OFFSET               0x04
#define SDIO_ARG_OFFSET                 0x08
#define SDIO_CMD_OFFSET                 0x0C
#define SDIO_RESP1_OFFSET               0x14
#define SDIO_RESP2_OFFSET               0x18
#define SDIO_RESP3_OFFSET               0x1C
#define SDIO_RESP4_OFFSET               0x20
#define SDIO_DTIMER_OFFSET              0x24
#define SDIO_DLEN_OFFSET                0x28
#define SDIO_DCTRL_OFFSET               0x2C
#define SDIO_STA_OFFSET                 0x34
#define SDIO_ICR_OFFSET                 0x38
#define SDIO_MASK_OFFSET                0x3C
#define SDIO_FIFO_OFFSET                0x80

    /* USB section */
#define USB_BASE_ADDRESS                0x40005C00
#define USB_PMA_BASE_ADDRESS            0x40006000

    // Offsets
#define USB_EPnR_OFFSET                 0x00
#define USB_CNTR_OFFSET                 0x40
#define USB_ISTR_OFFSET                 0x44
#define USB_FNR_OFFSET                  0x48
#define USB_DADDR_OFFSET                0x4C
#define USB_BTABLE_OFFSET               0x50

#define USB_ADDRn_TX_OFFSET             0x00
#define USB_COUNTn_TX_OFFSET            0x04
#define USB_ADDRn_RX_OFFSET             0x08
#define USB_COUNTn_RX_OFFSET            0x0C

    /* EXTI section */
#define EXTI_BASE_ADDRESS               0x40010400

    // Offsets
#define EXTI_IMR_OFFSET                 0x0
#define EXTI_EMR_OFFSET                 0x4
#define EXTI_RTSR_OFFSET                0x8
#define EXTI_FTSR_OFFSET                0xC
#define EXTI_SWIER_OFFSET               0x10
#define EXTI_PR_OFFSET                  0x14

    /* PWR section */
    //#define PWR_BASE_ADDRESS      0x40007000

    // Offsets
    //#define PWR_CR_OFFSET         0x0
    //#define PWR_CSR_OFFSET            0x4

    /* Flash Section */
#define FLASH_BASE_ADDRESS              0x40022000

    // Offsets
#define FLASH_ACR_OFFSET                0x00
#define FLASH_KEYR_OFFSET               0x04
#define FLASH_OPTKEYR_OFFSET            0x08
#define FLASH_SR_OFFSET                 0x0C
#define FLASH_CR_OFFSET                 0x10
#define FLASH_AR_OFFSET                 0x14
#define FLASH_OBR_OFFSET                0x1C
#define FLASH_WRPR_OFFSET               0x20

    /* CRC Section */
#define CRC_BASE_ADDRESS              0x40023000

    // Offsets
#define CRC_DR_OFFSET                0x00
#define CRC_IDR_OFFSET               0x04
#define CRC_CR_OFFSET                0x08

    /* Unique 96bit ID section */
#define UNIQUE_ID_BASE_ADDRESS          0x1FFFF7E8

#ifdef __cplusplus
}
#endif

#endif /* MEMMAP_H_ */
