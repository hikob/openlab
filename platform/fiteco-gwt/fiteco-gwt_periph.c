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
 * fiteco-gwt_periph.c
 *
 *  Created on: Jul 10, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "platform.h"
#include "fiteco-gwt.h"

#include "rcc.h"
#include "nvic_.h"
#include "afio.h"
#include "rf2xx/rf2xx_.h"
#include "adg759/adg759_.h"
#include "ina226/ina226_.h"

#include "fiteco/fiteco_lib_gwt_.h"

#include "printf.h"
#include "debug.h"

/** Radio initialization procedure */
static void radio_setup();
/** Power measurement initialization */
static void power_setup();

void platform_periph_setup()
{
    // Configure the radio
    radio_setup();

    // Configure the power measurement
    power_setup();

    // Cut power PA11, PB2, PB12
    gpio_set_output(GPIO_A, GPIO_PIN_11);
    gpio_pin_clear(GPIO_A, GPIO_PIN_11); // Battery

    gpio_set_output(GPIO_B, GPIO_PIN_2);
    gpio_pin_clear(GPIO_B, GPIO_PIN_2); // CHARGE

    gpio_set_output(GPIO_B, GPIO_PIN_12);
    gpio_pin_clear(GPIO_B, GPIO_PIN_12); // 5V
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

static _adg759_t ina_mux =
{
        .enable_gpio = GPIO_C, .enable_pin = GPIO_PIN_6,
        .select_gpio_A0 = GPIO_C, .select_pin_A0 = GPIO_PIN_9,
        .select_gpio_A1 = GPIO_C, .select_pin_A1 = GPIO_PIN_10
};

static fiteco_lib_gwt_config_t gwt_config = {
        .main_power_gpio = GPIO_B, .main_power_pin = GPIO_PIN_12,
        .battery_power_gpio = GPIO_A, .battery_power_pin = GPIO_PIN_11,
        .battery_charge_gpio = GPIO_B, .battery_charge_pin = GPIO_PIN_2,

        .current_mux = &ina_mux
};

static void power_setup()
{
    /*
     * Configure the multiplexer:
     *      EN: PC6
     *      A0: PC9
     *      A1: PC10
     */
    adg759_config(&ina_mux, GPIO_TYPE_PUSH_PULL);
    adg759_enable(&ina_mux);
    adg759_select(&ina_mux, ADG759_INPUT_1);

    log_printf("Configuring INA226...\n");

    // Select PC0 as input interrupt (ALERT)
    gpio_set_input(GPIO_C, GPIO_PIN_0);
    afio_select_exti_pin(EXTI_LINE_Px0, AFIO_PORT_C);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI0);

    // Configure the INA226, then initialize
    ina226_init(I2C_1, 0x80, EXTI_LINE_Px0);

    // Initial calibration
    ina226_calibrate(1, 0.1);

    // Configure the fiteco lib
    fiteco_lib_gwt_set_config(&gwt_config);
}
