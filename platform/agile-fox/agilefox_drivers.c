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
#include "usb_.h"

#include "printf.h"

/* GPIO instantiations */
_gpio_t _gpioA, _gpioB, _gpioC, _gpioD, _gpioE, _gpioF, _gpioG;

_timer_t _tim2, _tim3, _tim4, _tim6, _tim7;

/* Timer channels instantiation */
static timer_handler_t tim2_handlers[4], tim3_handlers[4], tim4_handlers[4];
static handler_arg_t tim2_args[4], tim3_args[4], tim4_args[4];

/* UART instantiation */
_uart_t _uart2;
/* UART declaration */
uart_t uart_print = &_uart2;

/* SPI instantiation */
_spi_t _spi2;

/* I2C instantiation */
_i2c_t _i2c1;

/* SDIO instantiation */
_sdio_t _sdio;
/* SDIO declaration */
sdio_t sdio = &_sdio;

/* DMA instantiation */
static _dma_t _dma1_ch4, _dma1_ch5;
static _dma_t _dma2_ch4;

void platform_drivers_setup()
{
    gpio_t sdio_gpio[6] =
    { gpioC, gpioC, gpioC, gpioC, gpioC, gpioD };
    gpio_pin_t
    sdio_pin[6] =
    {
        GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12,
        GPIO_PIN_2
    };

    // Set base address and AHB bit for all GPIO ports
    gpio_init(gpioA, GPIO_BASE_ADDRESS + GPIOA_OFFSET, RCC_APB_BIT_GPIOA);
    gpio_init(gpioB, GPIO_BASE_ADDRESS + GPIOB_OFFSET, RCC_APB_BIT_GPIOB);
    gpio_init(gpioC, GPIO_BASE_ADDRESS + GPIOC_OFFSET, RCC_APB_BIT_GPIOC);
    gpio_init(gpioD, GPIO_BASE_ADDRESS + GPIOD_OFFSET, RCC_APB_BIT_GPIOD);
    gpio_init(gpioE, GPIO_BASE_ADDRESS + GPIOE_OFFSET, RCC_APB_BIT_GPIOE);
    gpio_init(gpioF, GPIO_BASE_ADDRESS + GPIOF_OFFSET, RCC_APB_BIT_GPIOF);
    gpio_init(gpioG, GPIO_BASE_ADDRESS + GPIOG_OFFSET, RCC_APB_BIT_GPIOG);
    gpio_enable(gpioA);
    gpio_enable(gpioB);
    gpio_enable(gpioC);
    gpio_enable(gpioD);
    gpio_enable(gpioE);
    gpio_enable(gpioF);
    gpio_enable(gpioG);


    // Enable the AFIO
    rcc_apb_enable(RCC_APB2, RCC_APB_BIT_AFIO);

    // Configure the General Purpose Timers
    timer_init_general(tim2, TIM2_BASE_ADDRESS, RCC_APB_BUS_TIM2,
                       RCC_APB_BIT_TIM2, NVIC_IRQ_LINE_TIM2, tim2_handlers, tim2_args);
    timer_init_general(tim3, TIM3_BASE_ADDRESS, RCC_APB_BUS_TIM3,
                       RCC_APB_BIT_TIM3, NVIC_IRQ_LINE_TIM3, tim3_handlers, tim3_args);
    timer_init_general(tim4, TIM4_BASE_ADDRESS, RCC_APB_BUS_TIM4,
                       RCC_APB_BIT_TIM4, NVIC_IRQ_LINE_TIM4, tim4_handlers, tim4_args);

    // Configure the Basic Timers
    timer_init_basic(tim6, TIM6_BASE_ADDRESS, RCC_APB_BUS_TIM6,
                     RCC_APB_BIT_TIM6, NVIC_IRQ_LINE_TIM6);

    timer_init_basic(tim7, TIM7_BASE_ADDRESS, RCC_APB_BUS_TIM7,
                     RCC_APB_BIT_TIM7, NVIC_IRQ_LINE_TIM7);

    // Start the TIM3 at ~32kHz
    timer_enable(tim3);
    timer_select_internal_clock(tim3, (rcc_sysclk_get_clock_frequency(
                                           RCC_SYSCLK_CLOCK_PCLK1_TIM) / 32768) - 1);
    timer_start(tim3, 0xFFFF, NULL, NULL);

    // Configure the UART
    uart_init(uart_print, USART2_BASE_ADDRESS, RCC_APB_BUS_USART2,
              RCC_APB_BIT_USART2, NVIC_IRQ_LINE_USART2, gpioA, GPIO_PIN_3, // RX pin
              GPIO_PIN_2, // TX pin
              NULL // no DMA yet
             );

    // Enable the print uart
    uart_enable(uart_print, 500000);

    // Configure DMA1 Channel 4 (SPI2 RX)
    dma_init(&_dma1_ch4, DMA1_BASE_ADDRESS, RCC_AHB_BIT_DMA1, DMA_CHANNEL_4,
             NVIC_IRQ_LINE_DMA1_CH4);
    // Configure DMA1 Channel 5 (SPI2 TX)
    dma_init(&_dma1_ch5, DMA1_BASE_ADDRESS, RCC_AHB_BIT_DMA1, DMA_CHANNEL_5,
             NVIC_IRQ_LINE_DMA1_CH5);
    dma_enable(&_dma1_ch4);
    dma_enable(&_dma1_ch5);

    // Configure the SPI 2 with DMA
    spi_init(spi2, SPI2_BASE_ADDRESS, RCC_APB_BUS_SPI2, RCC_APB_BIT_SPI2,
             NVIC_IRQ_LINE_SPI2, gpioB, GPIO_PIN_13, GPIO_PIN_15, GPIO_PIN_14,
             &_dma1_ch4, &_dma1_ch5);
    spi_enable(spi2, 4000000, SPI_CLOCK_MODE_IDLE_LOW_RISING);

    // Configure the I2C 1
    i2c_init(i2c1, I2C1_BASE_ADDRESS, RCC_APB_BUS_I2C1, RCC_APB_BIT_I2C1,
             NVIC_IRQ_LINE_I2C1_EV, NVIC_IRQ_LINE_I2C1_ER, gpioB, GPIO_PIN_6,
             GPIO_PIN_7);
    i2c_enable(i2c1, I2C_CLOCK_MODE_STANDARD);

    // Configure DMA2 Channel 4 (for SDIO)
    dma_init(&_dma2_ch4, DMA2_BASE_ADDRESS, RCC_AHB_BIT_DMA2, DMA_CHANNEL_4,
             NVIC_IRQ_LINE_DMA2_CH4_5);

    // Configure SDIO
    sdio_init(sdio, sdio_gpio, sdio_pin, gpioC, GPIO_PIN_6, &_dma2_ch4);

    // Enable SD card TODO: place it in enhanced driver
    gpio_set_output(gpioB, GPIO_PIN_2);
    gpio_pin_clear(gpioB, GPIO_PIN_2);
}
void platform_drivers_restart_timers()
{
    timer_restart(tim3);
}
void platform_disable_uart()
{
    // Set pins analog and disable the print UART
    gpio_set_analog(gpioA, GPIO_PIN_2);
    gpio_set_analog(gpioA, GPIO_PIN_3);
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
    timer_handle_interrupt(tim2);
}

void tim3_isr()
{
    timer_handle_interrupt(tim3);
}

void tim4_isr()
{
    timer_handle_interrupt(tim4);
}

void tim6_isr()
{
    timer_handle_interrupt(tim6);
}

void tim7_isr()
{
    timer_handle_interrupt(tim7);
}

void usart2_isr()
{
    uart_handle_interrupt(uart_print);
}

void spi2_isr()
{
    spi_handle_interrupt(spi2);
}

void i2c1_ev_isr()
{
    i2c_handle_ev_interrupt(i2c1);
}

void i2c1_er_isr()
{
    i2c_handle_er_interrupt(i2c1);
}

void dma1_channel4_isr()
{
    dma_handle_interrupt(&_dma1_ch4);
}

void dma1_channel5_isr()
{
    dma_handle_interrupt(&_dma1_ch5);
}

void dma2_ch4_5_isr()
{
    dma_handle_interrupt(&_dma2_ch4);
}

void sdio_isr()
{
    sdio_handle_interrupt(sdio);
}

void usb_lp_can1_rx0_isr()
{
    usb_handle_interrupt();
}
