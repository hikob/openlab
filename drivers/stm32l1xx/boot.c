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
 * boot.c
 *
 *  Created on: Jul 25, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include "boot.h"

DEBUG_HANDLER(wwdg_isr);
DEBUG_HANDLER(pvd_isr);
DEBUG_HANDLER(tamper_stamp_isr);
DEBUG_HANDLER(rtc_wkup_isr);
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
DEBUG_HANDLER(adc1_isr);
DEBUG_HANDLER(usb_hp_isr);
DEBUG_HANDLER(usb_lp_isr);
DEBUG_HANDLER(dac_isr);
DEBUG_HANDLER(comp_isr);
DEBUG_HANDLER(exti9_5_isr);
DEBUG_HANDLER(lcd_isr);
DEBUG_HANDLER(tim9_isr);
DEBUG_HANDLER(tim10_isr);
DEBUG_HANDLER(tim11_isr);
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
DEBUG_HANDLER(tim6_isr);
DEBUG_HANDLER(tim7_isr);

/*
 * Place the stm32-specific vector handlers pointers in order at the beginning
 * of the .usr_vectors section (just after .vectors section).
 * The vector table is define in RM0038 page 148
 */
__attribute__((section(".usr_vectors"))) const vector_t usr_vectors[] =
{
    // 0x40
    wwdg_isr, pvd_isr, tamper_stamp_isr, rtc_wkup_isr, flash_isr, rcc_isr,
    exti0_isr, exti1_isr, exti2_isr, exti3_isr, exti4_isr,
    dma1_channel1_isr, dma1_channel2_isr, dma1_channel3_isr,
    dma1_channel4_isr, dma1_channel5_isr, dma1_channel6_isr,
    dma1_channel7_isr, adc1_isr, usb_hp_isr, usb_lp_isr, dac_isr, comp_isr,
    exti9_5_isr, lcd_isr, tim9_isr, tim10_isr, tim11_isr, tim2_isr,
    tim3_isr, tim4_isr, i2c1_ev_isr, i2c1_er_isr, i2c2_ev_isr, i2c2_er_isr,
    spi1_isr, spi2_isr, usart1_isr, usart2_isr, usart3_isr, exti15_10_isr,
    rtc_alarm_isr, usb_fs_wkup_isr, tim6_isr, tim7_isr
};


