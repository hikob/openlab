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
 * fiteco-m3_periph.c
 *
 *  Created on: Jul 10, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "platform.h"
#include "fiteco-m3.h"

#include "rcc.h"
#include "nvic_.h"
#include "afio.h"
#include "rf2xx/rf2xx_.h"
#include "l3g4200d/l3g4200d_.h"
#include "lsm303dlhc/lsm303dlhc_.h"
#include "lps331ap/lps331ap_.h"
#include "isl29020/isl29020_.h"
#include "n25xxx/n25xxx_.h"

#include "debug.h"

/** Radio initialization procedure */
static void radio_setup();

/** Gyro initialization procedure */
static void gyro_setup();

/** Accelero/magneto initialization procedure */
static void accmag_setup();

/** Pressure sensor initialization procedure */
static void pres_setup();

/** Light sensor initialization procedure */
static void light_setup();

/** SPI flash initialization procedure */
static void storage_setup();

void platform_periph_setup()
{
    // Configure the radio
    radio_setup();
    // Configure the IMU
    gyro_setup();
    accmag_setup();
    pres_setup();
    // Configure the light sensor
    light_setup();
    // Configure the storage
    storage_setup();
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
/** SPI1 */
.spi = SPI_1,

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
.type = RF2XX_TYPE_2_4GHz };
static _rf2xx_t _rf231 =
{ .config = &_rf231_config };
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
    gpio_set_input(GPIO_C, GPIO_PIN_0);
    afio_select_exti_pin(EXTI_LINE_Px0, AFIO_PORT_C);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI0);
    l3g4200d_config(I2C_1, EXTI_LINE_Px0, GPIO_C, GPIO_PIN_0);
}

static void accmag_setup()
{
    afio_select_exti_pin(EXTI_LINE_Px1, AFIO_PORT_B);
    afio_select_exti_pin(EXTI_LINE_Px2, AFIO_PORT_B);
    afio_select_exti_pin(EXTI_LINE_Px12, AFIO_PORT_B);

    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI1);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI2);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI15_10);
    
    gpio_set_input(GPIO_B, GPIO_PIN_2);
    gpio_set_input(GPIO_B, GPIO_PIN_12);
    gpio_set_input(GPIO_B, GPIO_PIN_1);
    
    lsm303dlhc_config(I2C_1,
                      /* Mag Drdy */EXTI_LINE_Px2, GPIO_B, GPIO_PIN_2,
                      /* Acc Int1 */EXTI_LINE_Px12, GPIO_B, GPIO_PIN_12,
                      /* Acc Int2 */EXTI_LINE_Px1, GPIO_B, GPIO_PIN_1);
}

static void pres_setup()
{
    lps331ap_config(I2C_1, 0);
}

static void light_setup()
{
    isl29020_config(I2C_1, 0);
    isl29020_powerdown();
}

static void storage_setup()
{
    // Configure the flash chips
    n25xxx_config(SPI_2, GPIO_A, GPIO_PIN_11, GPIO_C, GPIO_PIN_6, GPIO_C,
            GPIO_PIN_9);

    n25xxx_init();
}
