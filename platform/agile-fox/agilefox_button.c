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
 * agilefox_button.c
 *
 *  Created on: Oct 21, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include "platform.h"
#include "agilefox.h"

#include "gpio_.h"
#include "afio.h"
#include "exti.h"
#include "nvic_.h"

#define BUTTON_GPIO_PORT   GPIO_C
#define BUTTON_GPIO_PIN    GPIO_PIN_13
#define BUTTON_AFIO_PORT   AFIO_PORT_C

#define BUTTON_EXTI_LINE   EXTI_LINE_Px13
#define BUTTON_NVIC_LINE   NVIC_IRQ_LINE_EXTI15_10

void platform_button_setup()
{
    // Set button as input interruptible
    rcc_apb_enable(RCC_APB2, RCC_APB_BIT_AFIO);
    gpio_enable(BUTTON_GPIO_PORT);
    afio_select_exti_pin(BUTTON_EXTI_LINE, BUTTON_AFIO_PORT);
    nvic_enable_interrupt_line(BUTTON_NVIC_LINE);
}

uint32_t button_state()
{
    return gpio_pin_read(BUTTON_GPIO_PORT, BUTTON_GPIO_PIN);
}

void button_set_handler(handler_t handler, handler_arg_t handler_arg)
{
    if (handler)
    {
        exti_set_handler(BUTTON_EXTI_LINE, handler, handler_arg);
        exti_enable_interrupt_line(BUTTON_EXTI_LINE, EXTI_TRIGGER_FALLING);
    }
    else
    {
        exti_disable_interrupt_line(BUTTON_EXTI_LINE);
    }
}
