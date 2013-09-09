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
 * dnewt_periph.c
 *
 *  Created on: Mar 28, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "platform.h"
#include "dangerousnewt.h"

#include "nvic.h"
#include "rcc.h"
#include "afio.h"
#include "gpio_.h"
#include "nvic_.h"
#include "exti.h"
#include "nvic.h"

#include "pca9685/pca9685_.h"
#include "lsm303dlhc/lsm303dlhc_.h"
#include "rf2xx.h"
#include "rf2xx/rf2xx_.h"
#include "adg759/adg759_.h"
#include "pga308/pga308_.h"

void pca_setup();
void accelmag_setup();
void radio_setup();
void amp_setup();

void platform_periph_setup()
{
    amp_setup();

    pca_setup();

    accelmag_setup();

    radio_setup();
}

static _pca9685_t pcas[4];
void pca_setup()
{
    // Enable the LEDs OUTPUT (set OEn low)
    gpio_set_output(GPIO_B, GPIO_PIN_5);
    gpio_pin_clear(GPIO_B, GPIO_PIN_5);

    // Configure the PCA9685
    pca9685_config(pcas + 0, I2C_2, 0x86);
    pca9685_config(pcas + 1, I2C_2, 0x80);
    pca9685_config(pcas + 2, I2C_2, 0x84);
    pca9685_config(pcas + 3, I2C_2, 0x82);

    // Initialize the module
    pca9685_init(pcas, 4);

    // Stop all LEDs
    uint32_t i;

    for (i = 0; i < 4; i++)
    {
        pca9685_enable(pca9685_get(i));
        pca9685_set_all_leds(pca9685_get(i), 0);
    }
}

void accelmag_setup()
{
    // Configure the LSM303DLHC
    lsm303dlhc_config(I2C_2,
                      /* Mag Drdy */EXTI_LINE_Px4, GPIO_A, GPIO_PIN_4,
                      /* Acc Int1 */EXTI_LINE_Px1, GPIO_C, GPIO_PIN_1,
                      /* Acc Int2 */EXTI_LINE_Px0, GPIO_C, GPIO_PIN_0);

    // Set the pins as input
    gpio_set_input(GPIO_A, GPIO_PIN_4);
    gpio_set_input(GPIO_C, GPIO_PIN_1);
    gpio_set_input(GPIO_C, GPIO_PIN_0);

    // Disable the LSM
    lsm303dlhc_powerdown();
}

/* Radio instantiation */
/**
 * Radio1: (RF 231)
 *   DIG2: TIM3CH4
 *   IRQ: EXTI9
 *   CSn: PC6
 *   SLP_TR: PA0
 *   RSTn: PA11
 */
static const _rf2xx_config_t _rf231_config =
{
        /** SPI2 */
        .spi = SPI_2,

        /** CSN: PC6 */
        .csn_gpio = GPIO_C, .csn_pin = GPIO_PIN_6,

        /** RSTN: PA11 */
        .rstn_gpio = GPIO_A, .rstn_pin = GPIO_PIN_11,

        /** SPL_TR: PA0 */
        .slp_tr_gpio = GPIO_A, .slp_tr_pin = GPIO_PIN_0,

        /** IRQ: EXTI 9 */
        .irq_exti_line = EXTI_LINE_Px9,

        /** DIG2: TIM3CH4 */
        .dig2_timer = TIM_3, .dig2_channel = TIMER_CHANNEL_4,

        /** External PA: none */
        .pa_enable_gpio = NULL, .pa_enable_pin = 0,

        /** Type: 2.4GHz */
        .type = RF2XX_TYPE_2_4GHz
};
static _rf2xx_t _rf231 = {.config = &_rf231_config};
rf2xx_t rf231 = &_rf231;

void radio_setup()
{
    // Configure the RF231
    // Set IRQ (PC9) as input IRQ
    rcc_apb_enable(RCC_APB2, RCC_APB_BIT_AFIO);
    gpio_enable(GPIO_C);
    afio_select_exti_pin(EXTI_LINE_Px9, AFIO_PORT_C);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI9_5);

    // Set DIG2 (PB1) as Timer 3 channel 4 input capture
    // Configure the Timer Capture pins
    gpio_enable(GPIO_B);
    gpio_set_input(GPIO_B, GPIO_PIN_1);

    // Initialize
    rf2xx_init(rf231);
}

static _adg759_t adg759 =
{
        .enable_gpio = NULL, .enable_pin = 0,
        .select_gpio_A0 = GPIO_A, .select_pin_A0 = GPIO_PIN_6,
        .select_gpio_A1 = GPIO_A, .select_pin_A1 = GPIO_PIN_7
};
adg759_t pga308_mux = &adg759;

void amp_setup()
{
    // Configure the multiplexer, no enable
    adg759_config(&adg759, GPIO_TYPE_OPEN_DRAIN);

    // Disable it
    adg759_disable(&adg759);

    // Configure the amplifier, ADC channel is 8 and disable it
    pga308_init(UART_1, GPIO_B, GPIO_PIN_2, ADC_1, 8);
    pga308_disable();

    // Set analog input on PB0 for ADC input
    gpio_set_analog(GPIO_B, GPIO_PIN_0);

    // Enable PGA
    pga308_enable();
}
