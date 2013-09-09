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
 * azurelion_periph.c
 *
 *  Created on: Jul 8, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "azurelion.h"

#include "syscfg.h"
#include "rcc_sysclk.h"
#include "gpio_.h"
#include "timer.h"
#include "nvic_.h"
#include "usb_.h"
#include "rf2xx/rf2xx_.h"
#include "batfb/batfb_.h"
#include "adg759/adg759_.h"
#include "pga308/pga308_.h"
#include "gpio_registers.h"

#include "debug.h"

/** Flag indicating if the SLPTR pin is bound to PC6 (tim3ch1) */
int32_t al04_has_slptr_on_timer = 0;

/** Radio initialization procedure */
static void radio_setup();
/** Battery Feedback ADC init */
static void vbat_setup();
/** Amplifier initialization procedure */
static void amp_setup();

void platform_periph_setup()
{
    // Test PC8, if 0 then SLP_TR is bound
    gpio_set_input(GPIO_C, GPIO_PIN_8); // Red
    gpio_config_pull_up_down(GPIO_C, GPIO_PIN_8, GPIO_PULL_UP);

    if (gpio_pin_read(GPIO_C, GPIO_PIN_8) == 0)
    {
        log_info("SLP_TR to timer effective!");
        al04_has_slptr_on_timer = 1;
    }

    // Disable pull up
    gpio_set_analog(GPIO_C, GPIO_PIN_8);
    gpio_config_pull_up_down(GPIO_C, GPIO_PIN_8, GPIO_PULL_DISABLED);

    // Initialize the radio chip
    radio_setup();

    // Setup the feedback battery
    vbat_setup();

    // Initialize the amplifier
    amp_setup();
}

/* Radio instantiation */
/**
 * Radio1: (RF 231)
 *   DIG2: PA6, TIM10CH1
 *   IRQ: PB0, EXTI0
 *   CSn: PC9
 *   SLP_TR: PC10
 *   RSTn: PC11
 */
static _rf2xx_config_t _rf231_config =
{
        /** SPI2 */
        .spi = SPI_2,

        /** CSN: PC9 */
        .csn_gpio = GPIO_C, .csn_pin = GPIO_PIN_9,

        /** RSTN: PC11 */
        .rstn_gpio = GPIO_C, .rstn_pin = GPIO_PIN_11,

        /** SPL_TR: PC10 as default, PC6 if patched */
        .slp_tr_gpio = GPIO_C, .slp_tr_pin = GPIO_PIN_10,

        /** IRQ: EXTI 0 */
        .irq_exti_line = EXTI_LINE_Px0,

        /** DIG2: TIM10CH1 */
        .dig2_timer = TIM_10, .dig2_channel = TIMER_CHANNEL_1,

        /** External PA: none */
        .pa_enable_gpio = NULL, .pa_enable_pin = 0,

        /** Type: 2.4GHz */
        .type = RF2XX_TYPE_2_4GHz
};
static _rf2xx_t _rf231 = {.config = &_rf231_config};
rf2xx_t rf231 = &_rf231;

/**
 * Radio1: (RF 212)
 *   DIG2: PA7, TIM11CH1
 *   IRQ: PB1, EXTI1
 *   CSn: PB2
 *   SLP_TR: PC5
 *   RSTn: PC4
 */
static const _rf2xx_config_t _rf212_config =
{
        /** SPI2 */
        .spi = SPI_2,

        /** CSN: PB2 */
        .csn_gpio = GPIO_B, .csn_pin = GPIO_PIN_2,

        /** RSTN: PC4 */
        .rstn_gpio = GPIO_C, .rstn_pin = GPIO_PIN_4,

        /** SPL_TR: PC5 */
        .slp_tr_gpio = GPIO_C, .slp_tr_pin = GPIO_PIN_5,

        /** IRQ: EXTI 1 */
        .irq_exti_line = EXTI_LINE_Px1,

        /** DIG2: none */
        .dig2_timer = NULL, .dig2_channel = 0,

        /** External PA: none */
        .pa_enable_gpio = NULL, .pa_enable_pin = 0,

        /** Type: 868MHz */
        .type = RF2XX_TYPE_868MHz
};
static _rf2xx_t _rf212 = {.config = &_rf212_config};
rf2xx_t rf212 = &_rf212;

static void radio_setup()
{
    // Update RF231 config if board is patched
    if (al04_has_slptr_on_timer)
    {
        _rf231_config.slp_tr_gpio = GPIO_C;
        _rf231_config.slp_tr_pin = GPIO_PIN_6;
        _rf231_config.slp_tr_af = GPIO_AF_2;
    }

    // Configure the radio chips
    // Set IRQ as input IRQ
    syscfg_select_exti_pin(EXTI_LINE_Px0, SYSCFG_PORT_B);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI0);

    // Set DIG2 as Timer input capture
    gpio_set_alternate_function(GPIO_A, GPIO_PIN_6, GPIO_AF_3);

    // Initialize the radio to put it in Sleep State
    rf2xx_init(rf231);

    // Set IRQ as input IRQ
    syscfg_select_exti_pin(EXTI_LINE_Px1, SYSCFG_PORT_B);
    nvic_enable_interrupt_line(NVIC_IRQ_LINE_EXTI1);

    // Set DIG2 as Timer input capture
    gpio_set_alternate_function(GPIO_A, GPIO_PIN_7, GPIO_AF_3);

    // Initialize the radio to put it in Sleep State
    rf2xx_init(rf212);
}

/** Battery Feedback configuration */
static const batfb_config_t batfb_cfg =
{
    .adc = ADC_1,
    .adc_channel = 10,
    .mode = BATFB_MODE_TRANSISTORS,
    .enable_gpio = GPIO_B,
    .enable_pin = GPIO_PIN_6,
    .type = BATFB_TYPE_PRIMARY,
};

static void vbat_setup()
{
    // Register the configuration
    batfb_config(&batfb_cfg);

    // Initialize it
    batfb_init();

    // Set analog for PC0
    gpio_set_analog(GPIO_C, GPIO_PIN_0);
}

/** PGA308 amplifier configuration */
static const _adg759_t adg759 =
{
        .enable_gpio = GPIO_A, .enable_pin = GPIO_PIN_1,
        .select_gpio_A0 = GPIO_C, .select_pin_A0 = GPIO_PIN_3,
        .select_gpio_A1 = GPIO_C, .select_pin_A1 = GPIO_PIN_2,
};
adg759_t pga308_mux = &adg759;

static void amp_setup()
{
    // Configure the multiplexer
    adg759_config(&adg759, GPIO_TYPE_PUSH_PULL);

    // Configure the amplifier, ADC channel is 5 and disable it
    pga308_init(UART_3, GPIO_C, GPIO_PIN_1, ADC_1, 5);

    // Set analog input on PA5 for ADC input
    gpio_set_analog(GPIO_A, GPIO_PIN_5);
}

void platform_usb_enable()
{
    // gpio_enable(GPIO_A);
    // gpio_set_alternate_function(GPIO_A, GPIO_PIN_11, GPIO_AF_10);
    // gpio_set_alternate_function(GPIO_A, GPIO_PIN_12, GPIO_AF_10);

    // external pull-up
    gpio_enable(GPIO_B);
    gpio_set_output(GPIO_B, GPIO_PIN_5);
    gpio_pin_set(GPIO_B, GPIO_PIN_5);
    // internal pull-up

    //syscfg_pmc_config(SYSCFG_PMC_USB, 1);

    nvic_enable_interrupt_line(NVIC_IRQ_LINE_USB_LP);
}
