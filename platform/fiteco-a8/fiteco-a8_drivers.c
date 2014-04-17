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
 * fiteco-a8_drivers.c
 *
 *  Created on: Jul 10, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */


#include "platform.h"
#include "fiteco-a8.h"

#include "memmap.h"
#include "nvic_.h"
#include "rcc.h"
#include "dma_.h"
#include "exti.h"

/* UART declaration */
uart_t uart_print = UART_1;

/* I2C instantiation */
i2c_t i2c_external = I2C_1;

void platform_drivers_setup()
{
    // Set base address and AHB bit for all GPIO ports
    gpio_enable(GPIO_A);
    gpio_enable(GPIO_B);
    gpio_enable(GPIO_C);
    gpio_enable(GPIO_D);

    // Enable the AFIO
    rcc_apb_enable(RCC_APB_BUS_AFIO, RCC_APB_BIT_AFIO);

    // Start the TIM3 at ~32kHz
    timer_enable(TIM_3);
    timer_select_internal_clock(TIM_3, (rcc_sysclk_get_clock_frequency(
                                           RCC_SYSCLK_CLOCK_PCLK1_TIM) / 32768) - 1);
    timer_start(TIM_3, 0xFFFF, NULL, NULL);

    // Enable the print uart
    uart_enable(UART_1, PLATFORM_UART_PRINT_BAUDRATE);
    gpio_set_uart_tx(GPIO_A, GPIO_PIN_9);
    gpio_set_uart_rx(GPIO_A, GPIO_PIN_10);

    // Configure the DMA for the SPIs
    dma_enable(DMA_1_CH4);
    dma_enable(DMA_1_CH5);

    // Configure the SPI 2
    gpio_set_spi_clk(GPIO_B, GPIO_PIN_13);
    gpio_set_spi_miso(GPIO_B, GPIO_PIN_14);
    gpio_set_spi_mosi(GPIO_B, GPIO_PIN_15);
    spi_set_dma(SPI_2, DMA_1_CH4, DMA_1_CH5);
    spi_enable(SPI_2, 4000000, SPI_CLOCK_MODE_IDLE_LOW_RISING);

    // Configure the I2C 1
    gpio_set_i2c_scl(GPIO_B, GPIO_PIN_6);
    gpio_set_i2c_sda(GPIO_B, GPIO_PIN_7);
    i2c_enable(I2C_1, I2C_CLOCK_MODE_FAST);

    // Force inclusion of EXTI
    exti_set_handler(EXTI_LINE_Px0, NULL, NULL);
}

void platform_disable_uart()
{
    // Set pins analog and disable the print UART
    gpio_set_analog(GPIO_A, GPIO_PIN_9);
    gpio_set_analog(GPIO_A, GPIO_PIN_10);
    uart_disable(uart_print);

}
void platform_start_freertos_tick(uint16_t frequency, handler_t handler,
                                  handler_arg_t arg)
{
    // For now, until the RTC works, start the systick.
    nvic_enable_systick(frequency, handler, arg);
}

/* ISR handlers */
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

void tim6_isr()
{
    timer_handle_interrupt(TIM_6);
}

void tim7_isr()
{
    timer_handle_interrupt(TIM_7);
}

void usart1_isr()
{
    uart_handle_interrupt(UART_1);
}

void spi2_isr()
{
    spi_handle_interrupt(SPI_2);
}

void i2c1_ev_isr()
{
    i2c_handle_ev_interrupt(I2C_1);
}

void i2c1_er_isr()
{
    i2c_handle_er_interrupt(I2C_1);
}

void dma1_channel4_isr()
{
    dma_handle_interrupt(DMA_1_CH4);
}

void dma1_channel5_isr()
{
    dma_handle_interrupt(DMA_1_CH5);
}
