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
 * agilefox_periph.c
 *
 *  Created on: Jul 13, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "platform.h"
#include "agilefox.h"

#include "nvic.h"
#include "rcc.h"
#include "afio.h"
#include "gpio_.h"
#include "nvic_.h"
#include "usb_.h"
#include "rf2xx/rf2xx_.h"
#include "l3g4200d/l3g4200d_.h"
#include "lsm303dlhc/lsm303dlhc_.h"
#include "lps331ap/lps331ap_.h"


/** Radio initialization procedure */
static void radio_setup();

/** Gyro initialization procedure */
static void gyro_setup();

/** Accelero/magneto initialization procedure */
static void accmag_setup();

/** Pressure sensor initialization procedure */
static void pres_setup();

void platform_periph_setup()
{
    radio_setup();
    gyro_setup();
    accmag_setup();
    pres_setup();
}

/* Radio instantiation */
static const _rf2xx_config_t _rf231_config =
{
        /** SPI2 */
        .spi = SPI_2,

        /** CSN: PA1 */
        .csn_gpio = GPIO_A, .csn_pin = GPIO_PIN_1,

        /** RSTN: PC1 */
        .rstn_gpio = GPIO_C, .rstn_pin = GPIO_PIN_1,

        /** SPL_TR: PA0 */
        .slp_tr_gpio = GPIO_A, .slp_tr_pin = GPIO_PIN_0,

        /** IRQ: EXTI 2 */
        .irq_exti_line = EXTI_LINE_Px2,

        /** DIG2: TIM3CH4 */
        .dig2_timer = TIM_3, .dig2_channel = TIMER_CHANNEL_4,

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
    // Set IRQ (PC2) as input IRQ
    rcc_apb_enable(RCC_APB_BUS_AFIO, RCC_APB_BIT_AFIO);
    gpio_enable(GPIO_C);
    afio_select_exti_pin(EXTI_LINE_Px2, AFIO_PORT_C);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI2);

    // Set DIG2 (PB1) as Timer 3 channel 4 input capture
    // Configure the Timer Capture pins
    gpio_enable(GPIO_B);
    gpio_set_input(GPIO_B, GPIO_PIN_1);

    // Initialize the radio to put it in Sleep State
    rf2xx_init(rf231);
}

static void gyro_setup()
{
    afio_select_exti_pin(EXTI_LINE_Px8, AFIO_PORT_B);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI9_5);

    l3g4200d_config(I2C_1, EXTI_LINE_Px8, GPIO_B, GPIO_PIN_8);
}

static void accmag_setup()
{
    afio_select_exti_pin(EXTI_LINE_Px9, AFIO_PORT_A);
    afio_select_exti_pin(EXTI_LINE_Px5, AFIO_PORT_B);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI9_5);
    lsm303dlhc_config(I2C_1,
                      /* Mag Drdy */EXTI_LINE_Px9, GPIO_A, GPIO_PIN_9,
                      /* Acc Int1 */EXTI_LINE_Px9, GPIO_B, GPIO_PIN_9,
                      /* Acc Int2 */EXTI_LINE_Px5, GPIO_B, GPIO_PIN_5);

    // special fox config which uses a timer as irq source
    // because Acc Int1 and Mag Drdy are in conflict
    timer_enable(TIM_4);
    lsm303dlhc_config_acc_int1_uses_timer(TIM_4, TIMER_CHANNEL_4);
}

static void pres_setup()
{
    lps331ap_config(I2C_1, 0);
}

void platform_usb_enable()
{
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_USB);
    // agile-fox specific USB Enable code
    gpio_enable(GPIO_C);
    gpio_set_output(GPIO_C, GPIO_PIN_7);
    gpio_config_output_type(GPIO_C, GPIO_PIN_7, GPIO_TYPE_PUSH_PULL);
    gpio_pin_set(GPIO_C, GPIO_PIN_7);
}

/* ************** OPTIONAL PERIPERALS ********************* */
#include "ublox_max6/max6x_.h"

const max6x_config_t max6x_config =
{
    .i2c = I2C_1,

    .reg_gpio = GPIO_C, .reg_pin = GPIO_PIN_6,

    .rst_gpio = GPIO_C, .rst_pin = GPIO_PIN_5,
    .rst_high = false,
    .en_gpio = GPIO_A, .en_pin = GPIO_PIN_4,

    .ready_gpio = GPIO_A, .ready_pin = GPIO_PIN_6, .ready_exti_line = EXTI_LINE_Px6,
    .ready_afio_port = AFIO_PORT_A,
    .ready_nvic_line = NVIC_IRQ_LINE_EXTI9_5,

    .pulse_gpio = GPIO_A, .pulse_pin = GPIO_PIN_7,

    .antsel_gpio = GPIO_B, .antsel_pin = GPIO_PIN_0,
};

