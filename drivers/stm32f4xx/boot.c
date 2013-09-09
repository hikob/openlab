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
 * boot.c
 *
 *  Created on: Oct 22, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include "boot.h"

DEBUG_HANDLER(wwdg_isr)
DEBUG_HANDLER(pvd_isr)
DEBUG_HANDLER(tamper_stamp_isr)
DEBUG_HANDLER(rtc_wkup_isr)
DEBUG_HANDLER(flash_isr)
DEBUG_HANDLER(rcc_isr)
DEBUG_HANDLER(exti0_isr)
DEBUG_HANDLER(exti1_isr)
DEBUG_HANDLER(exti2_isr)
DEBUG_HANDLER(exti3_isr)
DEBUG_HANDLER(exti4_isr)
DEBUG_HANDLER(dma1_stream0_isr)
DEBUG_HANDLER(dma1_stream1_isr)
DEBUG_HANDLER(dma1_stream2_isr)
DEBUG_HANDLER(dma1_stream3_isr)
DEBUG_HANDLER(dma1_stream4_isr)
DEBUG_HANDLER(dma1_stream5_isr)
DEBUG_HANDLER(dma1_stream6_isr)
DEBUG_HANDLER(adc_isr)
DEBUG_HANDLER(can1_tx_isr)
DEBUG_HANDLER(can1_rx0_isr)
DEBUG_HANDLER(can1_rx1_isr)
DEBUG_HANDLER(can1_sce_isr)
DEBUG_HANDLER(exti9_5_isr)
DEBUG_HANDLER(tim1_brk_tim9_isr)
DEBUG_HANDLER(tim1_up_tim10_isr)
DEBUG_HANDLER(tim1_trg_com_tim11_isr)
DEBUG_HANDLER(tim1_cc_isr)
DEBUG_HANDLER(tim2_isr)
DEBUG_HANDLER(tim3_isr)
DEBUG_HANDLER(tim4_isr)
DEBUG_HANDLER(i2c1_ev_isr)
DEBUG_HANDLER(i2c1_er_isr)
DEBUG_HANDLER(i2c2_ev_isr)
DEBUG_HANDLER(i2c2_er_isr)
DEBUG_HANDLER(spi1_isr)
DEBUG_HANDLER(spi2_isr)
DEBUG_HANDLER(usart1_isr)
DEBUG_HANDLER(usart2_isr)
DEBUG_HANDLER(usart3_isr)
DEBUG_HANDLER(exti15_10_isr)
DEBUG_HANDLER(rtc_alarm_isr)
DEBUG_HANDLER(otg_fs_wkup_isr)
DEBUG_HANDLER(tim8_brk_tim12_isr)
DEBUG_HANDLER(tim8_up_tim13_isr)
DEBUG_HANDLER(tim8_trg_com_tim14_isr)
DEBUG_HANDLER(tim8_cc_isr)
DEBUG_HANDLER(dma1_stream7_isr)
DEBUG_HANDLER(fsmc_isr)
DEBUG_HANDLER(sdio_isr)
DEBUG_HANDLER(tim5_isr)
DEBUG_HANDLER(spi3_isr)
DEBUG_HANDLER(uart4_isr)
DEBUG_HANDLER(uart5_isr)
DEBUG_HANDLER(tim6_dac_isr)
DEBUG_HANDLER(tim7_isr)
DEBUG_HANDLER(dma2_stream0_isr)
DEBUG_HANDLER(dma2_stream1_isr)
DEBUG_HANDLER(dma2_stream2_isr)
DEBUG_HANDLER(dma2_stream3_isr)
DEBUG_HANDLER(dma2_stream4_isr)
DEBUG_HANDLER(eth_isr)
DEBUG_HANDLER(eth_wkup_isr)
DEBUG_HANDLER(can2_tx_isr)
DEBUG_HANDLER(can2_rx0_isr)
DEBUG_HANDLER(can2_rx1_isr)
DEBUG_HANDLER(can2_sce_isr)
DEBUG_HANDLER(otg_fs_isr)
DEBUG_HANDLER(dma2_stream5_isr)
DEBUG_HANDLER(dma2_stream6_isr)
DEBUG_HANDLER(dma2_stream7_isr)
DEBUG_HANDLER(usart6_isr)
DEBUG_HANDLER(i2c3_ev_isr)
DEBUG_HANDLER(i2c3_er_isr)
DEBUG_HANDLER(otg_hs_ep1_out_isr)
DEBUG_HANDLER(otg_hs_ep1_in_isr)
DEBUG_HANDLER(otg_hs_wkup_isr)
DEBUG_HANDLER(otg_hs_isr)
DEBUG_HANDLER(dcmi_isr)
DEBUG_HANDLER(cryp_isr)
DEBUG_HANDLER(hash_rng_isr)
DEBUG_HANDLER(fpu_isr)

/*
 * Place the stm32-specific vector handlers pointers in order at the beginning
 * of the .usr_vectors section (just after .vectors section)
 */
__attribute__((section(".usr_vectors")))  const vector_t usr_vectors[] =
{
        // 0x40
        wwdg_isr,
        pvd_isr,
        tamper_stamp_isr,
        rtc_wkup_isr,
        // 0x50
        flash_isr,
        rcc_isr,
        exti0_isr,
        exti1_isr,
        // 0x60
        exti2_isr,
        exti3_isr,
        exti4_isr,
        dma1_stream0_isr,
        // 0x70
        dma1_stream1_isr,
        dma1_stream2_isr,
        dma1_stream3_isr,
        dma1_stream4_isr,
        // 0x80
        dma1_stream5_isr,
        dma1_stream6_isr,
        adc_isr,
        can1_tx_isr,
        // 0x90
        can1_rx0_isr,
        can1_rx1_isr,
        can1_sce_isr,
        exti9_5_isr,
        // 0xA0
        tim1_brk_tim9_isr,
        tim1_up_tim10_isr,
        tim1_trg_com_tim11_isr,
        tim1_cc_isr,
        // 0xB0
        tim2_isr,
        tim3_isr,
        tim4_isr,
        i2c1_ev_isr,
        // 0xC0
        i2c1_er_isr,
        i2c2_ev_isr,
        i2c2_er_isr,
        spi1_isr,
        // 0xD0
        spi2_isr,
        usart1_isr,
        usart2_isr,
        usart3_isr,
        // 0xE0
        exti15_10_isr,
        rtc_alarm_isr,
        otg_fs_wkup_isr,
        tim8_brk_tim12_isr,
        // 0xF0
        tim8_up_tim13_isr,
        tim8_trg_com_tim14_isr,
        tim8_cc_isr,
        dma1_stream7_isr,
        // 0x100
        fsmc_isr,
        sdio_isr,
        tim5_isr,
        spi3_isr,
        // 0x110
        uart4_isr,
        uart5_isr,
        tim6_dac_isr,
        tim7_isr,
        // 0x120
        dma2_stream0_isr,
        dma2_stream1_isr,
        dma2_stream2_isr,
        dma2_stream3_isr,
        // 0x130
        dma2_stream4_isr,
        eth_isr,
        eth_wkup_isr,
        can2_tx_isr,
        // 0x140
        can2_rx0_isr,
        can2_rx1_isr,
        can2_sce_isr,
        otg_fs_isr,
        // 0x150
        dma2_stream5_isr,
        dma2_stream6_isr,
        dma2_stream7_isr,
        usart6_isr,
        // 0x160
        i2c3_ev_isr,
        i2c3_er_isr,
        otg_hs_ep1_out_isr,
        otg_hs_ep1_in_isr,
        // 0x170
        otg_hs_wkup_isr,
        otg_hs_isr,
        dcmi_isr,
        cryp_isr,
        // 0x180
        hash_rng_isr,
        fpu_isr
};
