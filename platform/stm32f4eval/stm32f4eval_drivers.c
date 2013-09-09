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
 * stm32f4eval_drivers.c
 *
 *  Created on: Sep 17, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "stm32f4eval.h"

#include "rcc.h"
#include "nvic_.h"

#include "uart_.h"
#include "timer_.h"
#include "ethmac_.h"

static void gpio_drivers_setup();
static void uart_drivers_setup();
static void timer_drivers_setup();
static void ethmac_drivers_setup();

void platform_drivers_setup()
{
    gpio_drivers_setup();
    uart_drivers_setup();
    timer_drivers_setup();

    ethmac_drivers_setup();
}

static void gpio_drivers_setup()
{
    // Set base address and AHB bit for all GPIO ports
    gpio_enable(GPIO_A);
    gpio_enable(GPIO_B);
    gpio_enable(GPIO_C);
    gpio_enable(GPIO_D);
    gpio_enable(GPIO_E);
    gpio_enable(GPIO_F);
    gpio_enable(GPIO_G);
    gpio_enable(GPIO_H);
    gpio_enable(GPIO_I);
}

/* UART declaration */
uart_t uart_print = UART_3;

static void uart_drivers_setup()
{
    // Enable the print uart
    gpio_set_uart_tx(GPIO_C, GPIO_PIN_10);
    gpio_set_uart_rx(GPIO_C, GPIO_PIN_11);
    uart_enable(uart_print, 500000);
}

void usart3_isr()
{
    uart_handle_interrupt(UART_3);
}

static void timer_drivers_setup()
{
    // Configure the General Purpose Timers
    timer_enable(TIM_2);
    timer_enable(TIM_3);
    timer_enable(TIM_4);

    // Select the clocks for all timers
    timer_select_internal_clock(TIM_2, 0);
    timer_select_internal_clock(TIM_3, 0);
    timer_select_internal_clock(TIM_4, 0);

    // Start others timers as 32kHz clock
    timer_select_internal_clock(TIM_2,
            (rcc_sysclk_get_clock_frequency(RCC_SYSCLK_CLOCK_PCLK1_TIM) / 32768)
                    - 1);
    timer_select_internal_clock(TIM_3,
            (rcc_sysclk_get_clock_frequency(RCC_SYSCLK_CLOCK_PCLK1_TIM) / 32768)
                    - 1);
    timer_select_internal_clock(TIM_4,
            (rcc_sysclk_get_clock_frequency(RCC_SYSCLK_CLOCK_PCLK1_TIM) / 32768)
                    - 1);

    // Start ALL PWM and other timers
    timer_start(TIM_2, 0xFFFF, NULL, NULL);
    timer_start(TIM_3, 0xFFFF, NULL, NULL);
    timer_start(TIM_4, 0xFFFF, NULL, NULL);
}

void tim2_isr()
{
    timer_handle_interrupt(TIM_2);
}
void tim3_isr()
{
    timer_handle_interrupt(TIM_3);
}
void tim4_isr()
{
    timer_handle_interrupt(TIM_4);
}

static void ethmac_drivers_setup()
{
    int i;

    // Set all GPIO AF11
    uint8_t A[] =
    { 1, 2, 7 };
    for (i = 0; i < sizeof(A); i++)
    {
        gpio_set_alternate_function(GPIO_A, A[i], GPIO_AF_11);
        gpio_config_output_type(GPIO_A, A[i], GPIO_TYPE_PUSH_PULL);
        gpio_set_speed(GPIO_A, A[i], GPIO_SPEED_HIGH);
    }

    uint8_t B[] =
    { 5, 8};
    for (i = 0; i < sizeof(B); i++)
    {
        gpio_set_alternate_function(GPIO_B, B[i], GPIO_AF_11);
        gpio_config_output_type(GPIO_B, B[i], GPIO_TYPE_PUSH_PULL);
        gpio_set_speed(GPIO_B, B[i], GPIO_SPEED_HIGH);
    }

    uint8_t C[] =
    { 1, 2, 3, 4, 5 };
    for (i = 0; i < sizeof(C); i++)
    {
        gpio_set_alternate_function(GPIO_C, C[i], GPIO_AF_11);
        gpio_config_output_type(GPIO_C, C[i], GPIO_TYPE_PUSH_PULL);
        gpio_set_speed(GPIO_C, C[i], GPIO_SPEED_HIGH);
    }

    gpio_set_alternate_function(GPIO_E, GPIO_PIN_2, GPIO_AF_11);

    uint8_t G[] =
    { 11, 13, 14 };
    for (i = 0; i < sizeof(G); i++)
    {
        gpio_set_alternate_function(GPIO_G, G[i], GPIO_AF_11);
        gpio_config_output_type(GPIO_G, G[i], GPIO_TYPE_PUSH_PULL);
        gpio_set_speed(GPIO_G, G[i], GPIO_SPEED_HIGH);
    }

    uint8_t H[] =
    { 2, 3, 6, 7 };
    for (i = 0; i < sizeof(H); i++)
    {
        gpio_set_alternate_function(GPIO_H, H[i], GPIO_AF_11);
        gpio_config_output_type(GPIO_H, H[i], GPIO_TYPE_PUSH_PULL);
        gpio_set_speed(GPIO_H, H[i], GPIO_SPEED_HIGH);
    }

    uint8_t I[] =
    { 10 };
    for (i = 0; i < sizeof(I); i++)
    {
        gpio_set_alternate_function(GPIO_I, I[i], GPIO_AF_11);
        gpio_config_output_type(GPIO_H, I[i], GPIO_TYPE_PUSH_PULL);
        gpio_set_speed(GPIO_I, I[i], GPIO_SPEED_HIGH);
    }

    ethmac_init(ETHMAC_MODE_MII, 0);
}
void eth_isr()
{
    ethmac_handle_interrupt();
}
