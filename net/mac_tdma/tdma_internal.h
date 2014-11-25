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
 * Copyright (C) 2013 HiKoB.
 */

/*
 * tdma_internal.h
 *
 * \date Jan 09, 2013
 * \author: Damien Hedde <damien.hedde.at.hikob.com>
 */

#ifndef TDMA_INTERNAL_H_
#define TDMA_INTERNAL_H_

#include "FreeRTOS.h"

#include "tdma_types.h"

extern struct tdma_global tdma_data;

/*
 * Get the lock
 */
void tdma_get (void);

/*
 * Release the lock
 */
void tdma_release (void);

/*
 * Init the frame storage
 */
void tdma_frame_init (void);

/*
 * Allocate a frame
 */
tdma_frame_t * tdma_frame_alloc (unsigned rem);

/*
 * Free a frame
 */
void tdma_frame_free (tdma_frame_t *frame);

/*
 * Send a frame
 */
int tdma_frame_send (tdma_frame_t *frame);

/*
 * Print a frame
 */
void tdma_frame_print(tdma_frame_t *frame);

/*
 * Init the slots-frame module
 */
void tdma_slot_init (void);

/*
 * Start a slots-frame
 */
int tdma_slot_start (uint32_t start_time);

/*
 * Stop the slots-frame
 */
void tdma_slot_stop (void);

/*
 * Configure a slot owner
 */
void tdma_slot_configure(uint8_t id, uint16_t owner);

/*
 * Print the slots-frame
 */
void tdma_slot_print (void);

/*
 * Update slots-frame timing
 */
void tdma_slot_update_frame_start (uint32_t time);

/*
 * Scan for beacon
 */
void slot_scan(tdma_frame_t *frame, uint8_t channel);

/*
 * Handler for RX data packet
 */
void tdma_data_rx_handler(tdma_frame_t *frame);

/*
 * Handler for TX data packet
 */
void tdma_data_tx_handler(tdma_frame_t *frame);

#endif /* MAC_TDMA_INTERNAL_H_ */
