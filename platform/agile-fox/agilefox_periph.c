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
#include "lps331/lps331_.h"


/** Radio initialization procedure */
static void radio_setup();

/* Radio instantiation */
static _rf2xx_t _rf231;
/* Radio declaration */
rf2xx_t rf231 = &_rf231;

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

static void radio_setup()
{
    // RF 231
    // Set IRQ (PC2) as input IRQ
    rcc_apb_enable(RCC_APB2, RCC_APB_BIT_AFIO);
    gpio_enable(gpioC);
    afio_select_exti_pin(EXTI_LINE_Px2, AFIO_PORT_C);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI2);

    // Set DIG2 (PB1) as Timer 3 channel 4 input capture
    // Configure the Timer Capture pins
    gpio_enable(gpioB);
    gpio_set_input(gpioB, GPIO_PIN_1);

    rf2xx_config(rf231, spi2,
                 // CSN
                 gpioA, GPIO_PIN_1,
                 // RSTN
                 gpioC, GPIO_PIN_1,
                 // SLP_TR
                 gpioA, GPIO_PIN_0,
                 // IRQ
                 EXTI_LINE_Px2,
                 // DIG 2 timer
                 tim3, TIMER_CHANNEL_4);

    // Initialize the radio to put it in Sleep State
    rf2xx_init(rf231);
}

static void gyro_setup()
{
//    afio_select_exti_pin(EXTI_LINE_Px8, AFIO_PORT_B);
//    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI9_5);
    l3g4200d_config(i2c1);
//    l3g4200d_enable_drdy(EXTI_LINE_Px8);
}

static void accmag_setup()
{
    afio_select_exti_pin(EXTI_LINE_Px9, AFIO_PORT_A);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI9_5);
    lsm303dlhc_config(i2c1,
                      /* Mag DRDY INT*/EXTI_LINE_Px9,
                      /* Acc INT1 & INT2 (unused now) */
                      EXTI_LINE_Px9, EXTI_LINE_Px5);
}

static void pres_setup()
{
    lps331_config(i2c1, 0);
}

void platform_usb_enable()
{
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_USB);
    // agile-fox specific USB Enable code
    gpio_enable(gpioC);
    gpio_set_output(gpioC, GPIO_PIN_7);
    gpio_config_output_type(gpioC, GPIO_PIN_7, GPIO_TYPE_PUSH_PULL);
    gpio_pin_set(gpioC, GPIO_PIN_7);
}


