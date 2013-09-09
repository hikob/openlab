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
 * fiteco-a8_periph.c
 *
 *  Created on: Jul 10, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "platform.h"
#include "fiteco-a8.h"

#include "rcc.h"
#include "nvic_.h"
#include "afio.h"
#include "rf2xx/rf2xx_.h"
#include "l3g4200d/l3g4200d_.h"
#include "lsm303dlhc/lsm303dlhc_.h"

/** Radio initialization procedure */
static void radio_setup();

/** Gyro initialization procedure */
static void gyro_setup();

/** Accelero/magneto initialization procedure */
static void accmag_setup();

void platform_periph_setup()
{
    // Configure the radio
    radio_setup();
    // Configure the IMU
    gyro_setup();
    accmag_setup();
}

/* Radio instantiation */
/**
 * Radio1: (RF 231)
 *   CSn: PA4
 *   RSTn: PC1
 *   SLP_TR: PA2
 *   IRQ: EXTI4
 *   DIG2: TIM3CH3
 */
static const _rf2xx_config_t _rf231_config =
{
        /** SPI2 */
        .spi = SPI_2,

        /** CSN: PA4 */
        .csn_gpio = GPIO_A, .csn_pin = GPIO_PIN_4,

        /** RSTN: PC1 */
        .rstn_gpio = GPIO_C, .rstn_pin = GPIO_PIN_1,

        /** SPL_TR: PA2 */
        .slp_tr_gpio = GPIO_A, .slp_tr_pin = GPIO_PIN_2,

        /** IRQ: EXTI 4 */
        .irq_exti_line = EXTI_LINE_Px4,

        /** DIG2: TIM3CH3 */
        .dig2_timer = TIM_3, .dig2_channel = TIMER_CHANNEL_3,

        /** External PA: none */
        .pa_enable_gpio = NULL, .pa_enable_pin = 0,

        /** Type: 2.4GHz */
        .type = RF2XX_TYPE_2_4GHz
};
static _rf2xx_t _rf231 = {.config = &_rf231_config};
rf2xx_t rf231 = &_rf231;

static void radio_setup()
{
    // RF 231
    // Set IRQ (PC4) as input IRQ
    gpio_set_input(GPIO_C, GPIO_PIN_4);
    afio_select_exti_pin(EXTI_LINE_Px4, AFIO_PORT_C);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI4);

    // Set DIG2 (PB0) as Timer 3 channel 3 input capture
    // Configure the Timer Capture pins
    gpio_set_input(GPIO_B, GPIO_PIN_0);

    // Initialize the radio to put it in Sleep State
    rf2xx_init(rf231);
}

static void gyro_setup()
{
    // Set PB9 input for GYRO DRDY
    gpio_set_input(GPIO_C, GPIO_PIN_9);
    afio_select_exti_pin(EXTI_LINE_Px9, AFIO_PORT_C);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI9_5);
    l3g4200d_config(I2C_1, EXTI_LINE_Px9, GPIO_C, GPIO_PIN_9);
}

static void accmag_setup()
{
    // Set PA11 input for MAG DRDY
    gpio_set_input(GPIO_A, GPIO_PIN_11);
    afio_select_exti_pin(EXTI_LINE_Px11, AFIO_PORT_A);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI15_10);

    // Set PB12 for ACC INT1
    gpio_set_input(GPIO_B, GPIO_PIN_12);
    afio_select_exti_pin(EXTI_LINE_Px12, AFIO_PORT_B);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI15_10);

    // Set PB2 for ACC INT2
    gpio_set_input(GPIO_B, GPIO_PIN_2);
    afio_select_exti_pin(EXTI_LINE_Px2, AFIO_PORT_B);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI2);

    // Configure LSM303DLHC
    lsm303dlhc_config(I2C_1,
                      /* Mag Drdy */EXTI_LINE_Px11, GPIO_A, GPIO_PIN_11,
                      /* Acc Int1 */EXTI_LINE_Px12, GPIO_B, GPIO_PIN_12,
                      /* Acc Int2 */EXTI_LINE_Px2, GPIO_B, GPIO_PIN_2);
}

