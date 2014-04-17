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
 * agilefox_drivers.c
 *
 *  Created on: Jul 13, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "platform.h"
#include "agilefox.h"

#include "memmap.h"
#include "nvic.h"
#include "nvic_.h"
#include "rcc.h"
#include "nvic_.h"
#include "gpio_.h"
#include "timer_.h"
#include "uart_.h"
#include "spi_.h"
#include "i2c_.h"
#include "sdio_.h"
#include "dma_.h"
#include "adc_.h"

/* UART instantiation */
uart_t uart_print = UART_2;

/* SDIO instantiation */
_sdio_t _sdio;
/* SDIO declaration */
sdio_t sdio = &_sdio;

/* I2C instantiation */
i2c_t i2c_external = I2C_1;

void platform_drivers_setup()
{
    gpio_t sdio_gpio[6] =
    { GPIO_C, GPIO_C, GPIO_C, GPIO_C, GPIO_C, GPIO_D };
    gpio_pin_t
    sdio_pin[6] =
    {
        GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12,
        GPIO_PIN_2
    };

    // Set base address and AHB bit for all GPIO ports
    gpio_enable(GPIO_A);
    gpio_enable(GPIO_B);
    gpio_enable(GPIO_C);
    gpio_enable(GPIO_D);
    gpio_enable(GPIO_E);
    gpio_enable(GPIO_F);
    gpio_enable(GPIO_G);

    // Enable the AFIO
    rcc_apb_enable(RCC_APB2, RCC_APB_BIT_AFIO);

    // Start the TIM3 at ~32kHz
    timer_enable(TIM_3);
    timer_select_internal_clock(TIM_3, (rcc_sysclk_get_clock_frequency(
                                           RCC_SYSCLK_CLOCK_PCLK1_TIM) / 32768) - 1);
    timer_start(TIM_3, 0xFFFF, NULL, NULL);

    // Enable the print uart
    uart_enable(uart_print, PLATFORM_UART_PRINT_BAUDRATE);
    gpio_set_uart_tx(GPIO_A, GPIO_PIN_2);
    gpio_set_uart_rx(GPIO_A, GPIO_PIN_3);

    // Configure DMA1 Channel 4 (SPI2 RX) and DMA1 Channel 5 (SPI2 TX)
    dma_enable(DMA_1_CH4);
    dma_enable(DMA_1_CH5);

    // Configure the SPI 2 with DMA
    gpio_set_spi_clk(GPIO_B, GPIO_PIN_13);
    gpio_set_spi_miso(GPIO_B, GPIO_PIN_14);
    gpio_set_spi_mosi(GPIO_B, GPIO_PIN_15);
    spi_set_dma(SPI_2, DMA_1_CH4, DMA_1_CH5);
    spi_enable(SPI_2, 4000000, SPI_CLOCK_MODE_IDLE_LOW_RISING);

    // Configure the I2C 1
    gpio_set_i2c_scl(GPIO_B, GPIO_PIN_6);
    gpio_set_i2c_sda(GPIO_B, GPIO_PIN_7);
    i2c_enable(I2C_1, I2C_CLOCK_MODE_FAST);

    // Configure DMA2 Channel 4 (for SDIO)
    dma_enable(DMA_2_CH4);

    // Configure SDIO
    sdio_init(sdio, sdio_gpio, sdio_pin, GPIO_C, GPIO_PIN_6, DMA_2_CH4);

    /** Enable SD card
     * \todo place it in enhanced driver
     */
    gpio_set_output(GPIO_B, GPIO_PIN_2);
    gpio_pin_clear(GPIO_B, GPIO_PIN_2);
}
void platform_drivers_restart_timers()
{
    timer_restart(TIM_3);
}
void platform_disable_uart()
{
    // Set pins analog and disable the print UART
    gpio_set_analog(GPIO_A, GPIO_PIN_2);
    gpio_set_analog(GPIO_A, GPIO_PIN_3);
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

void usart2_isr()
{
    uart_handle_interrupt(UART_2);
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

void dma2_ch4_5_isr()
{
    dma_handle_interrupt(DMA_2_CH4);
}

void sdio_isr()
{
    sdio_handle_interrupt(sdio);
}

void adc1_2_isr()
{
    adc_handle_interrupt(ADC_1);
}
