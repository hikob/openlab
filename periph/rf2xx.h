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
 * rf2xx.h
 *
 *  Created on: Jul 8, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef RF2XX_H_
#define RF2XX_H_

#include <stdint.h>

#include "rf2xx/rf2xx_timing.h"
#include "rf2xx/rf2xx_regs.h"
#include "handler.h"
#include "timer.h"

/**
 * This type defines a radio chip
 */
typedef void *rf2xx_t;

/* Initialization and reset */
void rf2xx_init(rf2xx_t radio);
void rf2xx_reset(rf2xx_t radio);

/* Register, FIFO, SRAM access */
uint8_t rf2xx_reg_read(rf2xx_t radio, uint8_t addr);
void rf2xx_reg_write(rf2xx_t radio, uint8_t addr, uint8_t value);

void rf2xx_fifo_read(rf2xx_t radio, uint8_t *buffer, uint16_t length);
void rf2xx_fifo_read_async(rf2xx_t radio, uint8_t *buffer, uint16_t length,
                           handler_t handler, handler_arg_t arg);

uint8_t rf2xx_fifo_read_first(rf2xx_t radio);
void rf2xx_fifo_read_remaining(rf2xx_t radio, uint8_t *buffer, uint16_t length);
void rf2xx_fifo_read_remaining_async(rf2xx_t radio, uint8_t *buffer,
                                     uint16_t length, handler_t handler, handler_arg_t arg);

void rf2xx_fifo_write(rf2xx_t radio, const uint8_t *buffer, uint16_t length);
void rf2xx_fifo_write_async(rf2xx_t radio, const uint8_t *buffer,
                            uint16_t length, handler_t handler, handler_arg_t arg);

void rf2xx_fifo_write_first(rf2xx_t radio, uint8_t first);
void rf2xx_fifo_write_remaining(rf2xx_t radio, const uint8_t *buffer,
                                uint16_t length);
void rf2xx_fifo_write_remaining_async(rf2xx_t radio, const uint8_t *buffer,
                                      uint16_t length, handler_t handler, handler_arg_t arg);

/** Cancel any asynchronous access */
void rf2xx_fifo_access_cancel(rf2xx_t radio);

void rf2xx_sram_read(rf2xx_t radio, uint8_t addr, uint8_t *buffer,
                     uint16_t length);
void rf2xx_sram_write(rf2xx_t radio, uint8_t addr, const uint8_t *buffer,
                      uint16_t length);

/* IRQ/DIG2 configuration */
void rf2xx_irq_configure(rf2xx_t radio, handler_t handler, handler_arg_t arg);
void rf2xx_irq_enable(rf2xx_t radio);
void rf2xx_irq_disable(rf2xx_t radio);
void rf2xx_dig2_configure(rf2xx_t radio, timer_handler_t handler,
                          handler_arg_t arg);
void rf2xx_dig2_enable(rf2xx_t radio);
void rf2xx_dig2_disable(rf2xx_t radio);

/* SLP_TR command */
void rf2xx_slp_tr_set(rf2xx_t radio);
void rf2xx_slp_tr_clear(rf2xx_t radio);

/* Handy functions */
static inline void rf2xx_set_state(rf2xx_t radio, uint8_t state)
{
    rf2xx_reg_write(radio, RF2XX_REG__TRX_STATE, state);
}
static inline uint8_t rf2xx_get_status(rf2xx_t radio)
{
    return rf2xx_reg_read(radio, RF2XX_REG__TRX_STATUS)
           & RF2XX_TRX_STATUS_MASK__TRX_STATUS;
}
void rf2xx_sleep(rf2xx_t radio);
void rf2xx_wakeup(rf2xx_t radio);

#endif /* RF2XX_H_ */
