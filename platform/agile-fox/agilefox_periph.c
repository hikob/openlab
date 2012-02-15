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
#include "rf2xx/rf2xx_.h"
#include "l3g4200d/l3g4200d_.h"
#include "lsm303dlhc/lsm303dlhc_.h"

/** Radio initialization procedure */
static void radio_setup();

/* Radio instantiation */
static _rf2xx_t _rf231;
/* Radio declaration */
rf2xx_t rf231 = &_rf231;

/** Gyro initialization procedure */
static void gyro_setup();

/* Gyro instantiation */
static _l3g4200d_t _l3g4200d;
/* Gyro declaration */
l3g4200d_t l3g4200d = &_l3g4200d;

/** Accelero/magneto initialization procedure */
static void accmag_setup();

/* Accelero/magneto instantiation */
static _lsm303dlhc_t _lsm303dlhc;
/* Accelero/magneto declaration */
lsm303dlhc_t lsm303dlhc = &_lsm303dlhc;

/** Pressure sensor initialization procedure */
static void pres_setup();


void platform_periph_setup()
{
    radio_setup();
    gyro_setup();
    accmag_setup();
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
    l3g4200d_config(l3g4200d, i2c1);
}

static void accmag_setup()
{
    lsm303dlhc_config(lsm303dlhc, i2c1);
}


