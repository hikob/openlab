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
 * boot.c
 *
 *  Created on: Jul 12, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "boot.h"

DEBUG_HANDLER(wwdg_isr);
DEBUG_HANDLER(pvd_isr);
DEBUG_HANDLER(tamper_stamp_isr);
DEBUG_HANDLER(rtc_isr);
DEBUG_HANDLER(flash_isr);
DEBUG_HANDLER(rcc_isr);
DEBUG_HANDLER(exti0_isr);
DEBUG_HANDLER(exti1_isr);
DEBUG_HANDLER(exti2_isr);
DEBUG_HANDLER(exti3_isr);
DEBUG_HANDLER(exti4_isr);
DEBUG_HANDLER(dma1_channel1_isr);
DEBUG_HANDLER(dma1_channel2_isr);
DEBUG_HANDLER(dma1_channel3_isr);
DEBUG_HANDLER(dma1_channel4_isr);
DEBUG_HANDLER(dma1_channel5_isr);
DEBUG_HANDLER(dma1_channel6_isr);
DEBUG_HANDLER(dma1_channel7_isr);
DEBUG_HANDLER(adc1_2_isr);
DEBUG_HANDLER(usb_hp_can1_tx_isr);
DEBUG_HANDLER(usb_lp_can1_rx0_isr);
DEBUG_HANDLER(can1_rx1_isr);
DEBUG_HANDLER(can1_sce_isr);
DEBUG_HANDLER(exti9_5_isr);
DEBUG_HANDLER(tim1_brk_isr);
DEBUG_HANDLER(tim1_up_isr);
DEBUG_HANDLER(tim1_trg_com_isr);
DEBUG_HANDLER(tim1_cc_isr);
DEBUG_HANDLER(tim2_isr);
DEBUG_HANDLER(tim3_isr);
DEBUG_HANDLER(tim4_isr);
DEBUG_HANDLER(i2c1_ev_isr);
DEBUG_HANDLER(i2c1_er_isr);
DEBUG_HANDLER(i2c2_ev_isr);
DEBUG_HANDLER(i2c2_er_isr);
DEBUG_HANDLER(spi1_isr);
DEBUG_HANDLER(spi2_isr);
DEBUG_HANDLER(usart1_isr);
DEBUG_HANDLER(usart2_isr);
DEBUG_HANDLER(usart3_isr);
DEBUG_HANDLER(exti15_10_isr);
DEBUG_HANDLER(rtc_alarm_isr);
DEBUG_HANDLER(usb_fs_wkup_isr);
DEBUG_HANDLER(tim8_brk_isr);
DEBUG_HANDLER(tim8_up_isr);
DEBUG_HANDLER(tim8_trg_com_isr);
DEBUG_HANDLER(tim8_cc_isr);
DEBUG_HANDLER(adc3_isr);
DEBUG_HANDLER(fsmc_isr);
DEBUG_HANDLER(sdio_isr);
DEBUG_HANDLER(tim5_isr);
DEBUG_HANDLER(spi3_isr);
DEBUG_HANDLER(uart4_isr);
DEBUG_HANDLER(uart5_isr);
DEBUG_HANDLER(tim6_isr);
DEBUG_HANDLER(tim7_isr);
DEBUG_HANDLER(dma2_ch1_isr);
DEBUG_HANDLER(dma2_ch2_isr);
DEBUG_HANDLER(dma2_ch3_isr);
DEBUG_HANDLER(dma2_ch4_5_isr);

/*
 * Place the stm32-specific vector handlers pointers in order at the beginning
 * of the .usr_vectors section (just after .vectors section)
 */
__attribute__((section(".usr_vectors"))) const vector_t usr_vectors[] =
{
    // 0x40
    wwdg_isr, pvd_isr, tamper_stamp_isr, rtc_isr, flash_isr, rcc_isr,
    exti0_isr, exti1_isr, exti2_isr, exti3_isr, exti4_isr,
    dma1_channel1_isr, dma1_channel2_isr, dma1_channel3_isr,
    dma1_channel4_isr, dma1_channel5_isr, dma1_channel6_isr,
    dma1_channel7_isr, adc1_2_isr, usb_hp_can1_tx_isr, usb_lp_can1_rx0_isr,
    can1_rx1_isr, can1_sce_isr, exti9_5_isr, tim1_brk_isr, tim1_up_isr,
    tim1_trg_com_isr, tim1_cc_isr, tim2_isr, tim3_isr, tim4_isr,
    i2c1_ev_isr, i2c1_er_isr, i2c2_ev_isr, i2c2_er_isr, spi1_isr,
    spi2_isr, usart1_isr, usart2_isr, usart3_isr, exti15_10_isr, rtc_alarm_isr,
    usb_fs_wkup_isr, tim8_brk_isr, tim8_up_isr, tim8_trg_com_isr, tim8_cc_isr,
    adc3_isr, fsmc_isr, sdio_isr, tim5_isr, spi3_isr, uart4_isr, uart5_isr,
    tim6_isr, tim7_isr, dma2_ch1_isr, dma2_ch2_isr, dma2_ch3_isr, dma2_ch4_5_isr,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    // Address 0x1E0: This is for boot in RAM mode for STM32F10x High Density devices.
    (vector_t)0xF1E0F85F
};
