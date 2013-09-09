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
 * packet.h
 *
 *  Created on: Sep 14, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef PACKET_H_
#define PACKET_H_

#include <stdint.h>

enum
{
    PACKET_MAX_SIZE = 128,
};

typedef struct packet
{
    uint8_t raw_data[PACKET_MAX_SIZE];

    /** Pointer to the beginning of the data of interest in raw_data */
    uint8_t *data;

    /** Length of the data of interest */
    uint16_t length;

    /** Pointer to struct packet for chaining */
    struct packet *next;
} packet_t;

typedef enum
{
    PACKET_SUCCESS = 0,
    PACKET_CANT_MOVE
} packet_status_t;

/**
 * Initialize the packet management module.
 * Should be called before any call to packet_alloc.
 */
void packet_init();

/**
 * Allocate a packet.
 * This picks a packet up from the packet pool and return its address.
 *
 * \param offset the offset of the data pointer in the packet, to provide
 * space for future headers;
 * \return the allocated packet address, or 0x0 if there is no free packet.
 */
packet_t *packet_alloc(uint8_t offset);

/**
 * Return the number of available packet_t in the packet pool.
 */
uint32_t packet_available();

/**
 * Free a packet.
 * This releases a packet from the pool.
 * \param packet the packet' address to free.
 */
void packet_free(packet_t *packet);

/**
 * Move the data to the right (to insert a header) if possible
 *
 * \param packet the packet to move;
 * \param shift the number of bytes to move;
 * \return the status of the operation
 *
 */
packet_status_t packet_move_data_right(packet_t *packet, uint16_t shift);

/**
 * Append a packet to a packet FIFO.
 * \param fifo a pointer to the FIFO;
 * \param packet the packet to append;
 */
void packet_fifo_append(packet_t **fifo, packet_t *packet);

/**
 * Get a packet from a packet FIFO.
 * \param fifo a pointer to the FIFO;
 * \return the first packet, or NULL;
 */
packet_t *packet_fifo_get(packet_t **fifo);

/**
 * Structure defining the complete storage for the packet library
 */
typedef struct
{
    packet_t *packet_buffer;
    uint32_t *packet_flags;
    uint32_t packet_number;
} packet_storage_t;

/**
 * This macro is used to simply define all the storage required for the TSCH
 *
 * \param num_packets the number of packets available
 */
#define PACKET_STORAGE_INIT(num_packets) \
        \
packet_t packet_storage_packet_buffer[num_packets]; \
uint32_t packet_storage_packet_flags[(num_packets - 1) / 32 + 1]; \
\
packet_storage_t packet_storage = \
{ \
    .packet_buffer = packet_storage_packet_buffer, \
    .packet_flags = packet_storage_packet_flags, \
    .packet_number = num_packets \
}

/** Complete storage that may be provided by an application */
extern packet_storage_t packet_storage;

#endif /* PACKET_H_ */
