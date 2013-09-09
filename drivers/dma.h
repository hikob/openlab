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

/**
 * \file dma.h
 *
 * \date Aug 19, 2011
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef DMA_H_
#define DMA_H_

/**
 * \addtogroup drivers
 * @{
 */

/**
 * \defgroup DMA DMA driver
 *
 * This driver provides the functions used to execute Direct Memory Accesses,
 * i.e. transferring data either from a peripheral to memory or from memory to
 * a peripheral.
 *
 *@{
 */

#include <stdint.h>
#include "handler.h"

/**
 * Abstract representation of a DMA driver
 */
typedef const void *dma_t;

/**
 * The DMA channels
 */
typedef enum
{
    DMA_CHANNEL_1 = 0,
    DMA_CHANNEL_2 = 1,
    DMA_CHANNEL_3 = 2,
    DMA_CHANNEL_4 = 3,
    DMA_CHANNEL_5 = 4,
    DMA_CHANNEL_6 = 5,
    DMA_CHANNEL_7 = 6
} dma_channel_t;

/**
 * The possible transfer size
 */
typedef enum
{
    DMA_SIZE_8bit  = 0,
    DMA_SIZE_16bit = 1,
    DMA_SIZE_32bit = 2,
} dma_size_t;

/**
 * Transfer direction
 */
typedef enum
{
    DMA_DIRECTION_FROM_PERIPHERAL = 0,
    DMA_DIRECTION_TO_PERIPHERAL = 1,
} dma_direction_t;

/**
 * DMA Action
 */
typedef enum
{
    DMA_INCREMENT_ON = 1,
    DMA_INCREMENT_OFF = 0
} dma_increment_t;

/**
 * Enable the DMA capability
 * This has to be called only once if DMA is required.
 */
void dma_enable(dma_t dma);

/**
 * Disable the DMA capability
 * This has to be called only once if DMA is required.
 */
void dma_disable(dma_t dma);

/**
 * Configure a DMA channel, specifying all the parameters.
 *
 * \param dma the DMA channel object;
 * \param peripheral_address: the register address of the peripheral data
 *      tranfer location;
 * \param memory_address: the memory address to store/read the data from/to the
 *      peripheral;
 * \param transfer_number: the number of transfers to operate;
 * \param tranfer_size: the size of each transfer;
 * \param direction: either to or from the peripheral;
 * \param memory_increment: select if the memory address is to be incremented
 *      at each transfer or not.
 */
void dma_config(dma_t dma, uint32_t peripheral_address,
                uint32_t memory_address, uint16_t transfer_number,
                dma_size_t transfer_size, dma_direction_t direction,
                dma_increment_t memory_increment);

/**
 * Start a DMA transfer, which must have been configured.
 *
 * \param dma the DMA to start;
 * \param done_handler the function to be called when transfer is completed;
 * \param handler_arg optional argument for the handler;
 **/
void dma_start(dma_t dma, handler_t done_handler, handler_arg_t handler_arg);

/**
 * Cancel a DMA transfer.
 *
 * \param dma the DMA to cancel.
 * \return 1 if the DMA transfer was aborted, 0 if it was already finished.
 */
int32_t dma_cancel(dma_t dma);

/**
 * @}
 * @}
 */

#endif /* DMA_H_ */
