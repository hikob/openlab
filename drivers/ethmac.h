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
 * ethmac.h
 *
 *  Created on: Sep 20, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef ETHMAC_H_
#define ETHMAC_H_

#include <stdint.h>

typedef struct
{
    uint32_t offset, size;
    void* real_des;
} ethmac_tx_t;

typedef struct
{
    uint32_t offset, size;
    void* real_des;
} ethmac_rx_t;

/**
 * Enable the Ethernet-MAC driver
 */
void ethmac_enable();

/**
 * Disable the Ethernet-MAC driver
 */
void ethmac_disable();

/**
 * Start the Ethernet MAC.
 *
 * \param address the address to start with
 */
void ethmac_start(const uint8_t* address);

/**
 * Get the link status
 * \return 1 if link up, 0 if down
 */
int ethmac_get_link_status();

/**
 * Initialize the next transmit descriptor
 *
 * \param tx a pointer to a structure provided by the caller.
 * \return 1 on success, 0 if none available
 */
int32_t ethmac_tx_init(ethmac_tx_t *tx);

/**
 * Write data to the current write descriptor.
 *
 * \param tx a pointer to a structure provided by the caller
 * \param buffer the buffer to copy from
 * \param len the length of the copy
 */
void ethmac_tx_write(ethmac_tx_t *tx, const uint8_t* buffer, uint32_t len);

/**
 * Send the current transmit descriptor.
 */
void ethmac_tx_send(ethmac_tx_t *tx);

/**
 * Initialize the next receive descriptor
 *
 * \return 1 on success, 0 if none available
 */
int32_t ethmac_rx_init(ethmac_rx_t *rx);

/**
 * Get the length of the received data in the current descriptor.
 *
 * \return the length in bytes of the received data.
 */
uint32_t ethmac_rx_get_len(ethmac_rx_t *rx);

/**
 * Read a bunch of data from the received descriptor to a buffer.
 *
 * \param buffer the buffer to store the data o
 * \param len the length of the copy
 */
void ethmac_rx_read(ethmac_rx_t *rx,uint8_t *buffer, uint32_t len);

/**
 * Release the current received descriptor.
 *
 * This should be called after having read all its data.
 */
void ethmac_rx_release(ethmac_rx_t *rx);

/**
 *  Function called when a received frame interrupt happened
 *
 *  The application should call \ref ethmac_process_received_frame to process.
 */
extern void ethmac_frame_received_isr();

/**
 *  Function called when a received frame interrupt happened
 *
 *  The application should call \ref ethmac_process_sent_frame to process.
 */
extern void ethmac_frame_sent_isr();


/** Print all registers */
void ethmac_debug();
#endif /* ETHMAC_H_ */
