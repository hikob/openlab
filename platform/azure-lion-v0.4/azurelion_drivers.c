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
 * azurelion_drivers.c
 *
 *  Created on: Jul 8, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "azurelion.h"

#include "memmap.h"
#include "rcc.h"
#include "rcc_sysclk.h"
#include "rtc.h"
#include "nvic_.h"
#include "gpio_.h"
#include "dma_.h"
#include "timer_.h"
#include "exti_.h"
#include "uart_.h"
#include "spi_.h"
#include "adc_.h"
#include "exti.h"

#include "printf.h"
#include "debug.h"

// UART 1 for debug, 2 for gateway, 3 for PGA
uart_t uart_print = UART_1;

void platform_drivers_setup()
{
    // Enable SYSCFG
    rcc_apb_enable(RCC_APB_BUS_SYSCFG, RCC_APB_BIT_SYSCFG);

    // Set base address and AHB bit for all GPIO ports
    gpio_enable(GPIO_A);
    gpio_enable(GPIO_B);
    gpio_enable(GPIO_C);
    gpio_enable(GPIO_D);
    gpio_enable(GPIO_H);

    // Set LSE on MCO output on pin PA.8
    gpio_set_alternate_function(GPIO_A, GPIO_PIN_8, GPIO_AF_0);
    rcc_sysclk_set_mco(RCC_SYSCLK_MCO_CLOCK_LSE, RCC_SYSCLK_MCO_PRE_1);

    // Set MCO as clock input for TIM2 (PA0) and TIM3 (PD2)
    gpio_set_alternate_function(GPIO_A, GPIO_PIN_0, GPIO_AF_1);
    gpio_set_alternate_function(GPIO_D, GPIO_PIN_2, GPIO_AF_2);

    // Start the timers
    timer_enable(TIM_3);
    timer_enable(TIM_9);
    timer_enable(TIM_10);
    timer_enable(TIM_11);
    timer_select_external_clock(TIM_3, 0);
    timer_select_external_clock(TIM_9, 0);
    timer_select_external_clock(TIM_10, 0);
    timer_select_external_clock(TIM_11, 0);
    timer_start(TIM_3, 0xFFFF, NULL, NULL);
    timer_start(TIM_9, 0xFFFF, NULL, NULL);
    timer_start(TIM_10, 0xFFFF, NULL, NULL);
    timer_start(TIM_11, 0xFFFF, NULL, NULL);

    // Enable the print uart
    gpio_set_uart_tx(GPIO_A, GPIO_PIN_9);
    gpio_set_uart_rx(GPIO_A, GPIO_PIN_10);
    uart_enable(UART_1, PLATFORM_UART_PRINT_BAUDRATE);

    // Configure the UART2 (for gateway)
    gpio_set_uart_tx(GPIO_A, GPIO_PIN_2);
    gpio_set_uart_rx(GPIO_A, GPIO_PIN_3);
    uart_set_dma(UART_2, DMA_1_CH7);

    // Configure the UART3 (for PGA308)
    gpio_set_uart_tx(GPIO_B, GPIO_PIN_10);
    gpio_set_uart_rx(GPIO_B, GPIO_PIN_11);

    // XXX uart pins forced disabled
    gpio_set_analog(GPIO_B, GPIO_PIN_11);
    gpio_set_analog(GPIO_B, GPIO_PIN_10);

    // Configure the SPI2, with the DMA objects (CH4: RX, CH5: TX)
    gpio_set_spi_clk(GPIO_B, GPIO_PIN_13);
    gpio_set_spi_miso(GPIO_B, GPIO_PIN_14);
    gpio_set_spi_mosi(GPIO_B, GPIO_PIN_15);
    dma_enable(DMA_1_CH4);
    dma_enable(DMA_1_CH5);
    spi_set_dma(SPI_2, DMA_1_CH4, DMA_1_CH5);
    spi_enable(SPI_2, 4000000, SPI_CLOCK_MODE_IDLE_LOW_RISING);

    // Configure the ADC1
    adc_enable(ADC_1);

    // Force inclusion of EXTI
    exti_set_handler(EXTI_LINE_Px0, NULL, NULL);
}

void platform_drivers_restart_timers()
{
    timer_restart(TIM_3);
    timer_restart(TIM_9);
    timer_restart(TIM_10);
    timer_restart(TIM_11);
}
void platform_drivers_check_timers()
{
    const openlab_timer_t timers[] =
    { TIM_3, TIM_9, TIM_10 };
    const openlab_timer_t timer_ref = TIM_11;

    uint32_t i;

    for (i = 0; i < sizeof(timers) / sizeof(openlab_timer_t); i++)
    {
        uint16_t ta1, ta2, tb;
        ta1 = timer_time(timers[i]);
        tb = timer_time(timer_ref);
        ta2 = timer_time(timers[i]);

        int32_t dt;

        if ((dt = (int16_t)(tb - ta1)) < -1)
        {
            timer_tick_update(timers[i], dt);
            log_printf("\tTIM#%u updated %d\n", i, dt);
        }
        else if ((dt = (int16_t)(ta2 - tb)) < -1)
        {
            timer_tick_update(timers[i], -dt);
            log_printf("\tTIM#%u updated %d\n", i, dt);
        }
    }
}

void platform_start_freertos_tick(uint16_t frequency, handler_t handler,
                                  handler_arg_t arg)
{
    // Check if FreeRTOS ticks are used
#if (configUSE_PREEMPTION)
    rtc_set_periodic_wakeup(RTC_WAKEUP_CLOCK_16384Hz, 16384 / frequency - 1, handler, arg);
#endif
}

void platform_disable_uart()
{
    // Disable the UART1
    uart_disable(UART_1);
    gpio_set_analog(GPIO_A, GPIO_PIN_9);
    gpio_set_analog(GPIO_A, GPIO_PIN_10);
}

/* ISR handlers */
void dma1_channel4_isr()
{
    dma_handle_interrupt(DMA_1_CH4);
}
void dma1_channel5_isr()
{
    dma_handle_interrupt(DMA_1_CH5);
}
void tim3_isr()
{
    timer_handle_interrupt(TIM_3);
}
void tim9_isr()
{
    timer_handle_interrupt(TIM_9);
}
void tim10_isr()
{
    timer_handle_interrupt(TIM_10);
}
void tim11_isr()
{
    timer_handle_interrupt(TIM_11);
}
void dma1_channel7_isr()
{
    dma_handle_interrupt(DMA_1_CH7);
}
void usart2_isr()
{
    uart_handle_interrupt(UART_2);
}
void usart1_isr()
{
    uart_handle_interrupt(UART_1);
}
void usart3_isr()
{
    uart_handle_interrupt(UART_3);
}

void spi2_isr()
{
    spi_handle_interrupt(SPI_2);
}

void adc1_isr()
{
    adc_handle_interrupt(ADC_1);
}

/* The EXTI isr */
void rtc_wkup_isr()
{
    exti_handle_interrupt(EXTI_LINE_RTC_WAKEUP);
}
void exti0_isr()
{
    exti_handle_interrupt(EXTI_LINE_Px0);
}
void exti1_isr()
{
    exti_handle_interrupt(EXTI_LINE_Px1);
}
void exti2_isr()
{
    exti_handle_interrupt(EXTI_LINE_Px2);
}
void exti3_isr()
{
    exti_handle_interrupt(EXTI_LINE_Px3);
}
void exti4_isr()
{
    exti_handle_interrupt(EXTI_LINE_Px4);
}
void exti9_5_isr()
{
    exti_handle_9_5_interrupt();
}
void exti15_10_isr()
{
    exti_handle_15_10_interrupt();
}
