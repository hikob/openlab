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
 * packet.c
 *
 *  Created on: Sep 14, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */
#include <stdint.h>
#include "packet.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "printf.h"
#include "debug.h"

static inline void packet_flag_set(uint32_t index);
static inline void packet_flag_clear(uint32_t index);
static inline uint32_t packet_flag_test(uint32_t index);

/** The mutex for accessing the shared variables */
static xSemaphoreHandle mutex = NULL;

void packet_init()
{
    // Create the mutex if not created yet
    if (mutex == NULL)
    {
        mutex = xSemaphoreCreateMutex();

        uint32_t i;

        // Clear all the flags
        for (i = 0; i < (packet_storage.packet_number - 1) / 32 + 1; i++)
        {
            packet_storage.packet_flags[i] = 0;
        }
    }
}
packet_t *packet_alloc(uint8_t offset)
{
    uint32_t i;

    // Take the mutex
    xSemaphoreTake(mutex, portMAX_DELAY);

    // Loop over the buffers
    for (i = 0; i < packet_storage.packet_number; i++)
    {
        // Check if corresponding flag is clear
        if (packet_flag_test(i) == 0)
        {
            // Yes, set the flag
            packet_flag_set(i);

            // Give the mutex
            xSemaphoreGive(mutex);

            // Prepare the packet
            packet_t *packet = packet_storage.packet_buffer + i;

            // Set data to point to the beginning of raw data plus the offset
            packet->data = packet->raw_data + offset;
            packet->length = 0;
            packet->next = NULL;

            // Return the pointer
            return packet;
        }
    }

    // Not found!

    // Give the mutex
    xSemaphoreGive(mutex);

    // Return NULL
    return NULL;
}
void packet_free(packet_t *packet)
{
    uint32_t i;

    // Take the mutex
    xSemaphoreTake(mutex, portMAX_DELAY);

    // Find the index of the buffer from its address
    i = (packet - packet_storage.packet_buffer);

    // Make sure index is coherent
    if (i < packet_storage.packet_number)
    {
        if (packet_flag_test(i))
        {
            // Clear flag
            packet_flag_clear(i);
        }
        else
        {
            log_warning("Freeing already freed packet");
        }
    }
    else
    {
        log_error("Freeing invalid packet %x", packet);
    }

    // Give the mutex
    xSemaphoreGive(mutex);
}
packet_status_t packet_move_data_right(packet_t *packet, uint16_t shift)
{
    // Check if there is enough space
    if ((packet->data + packet->length + shift)
            > packet->raw_data + PACKET_MAX_SIZE)
    {
        // Can't move the data
        log_warning("Can't move packet data");
        return PACKET_CANT_MOVE;
    }

    // Move the data
    int32_t i;

    for (i = 1; i <= packet->length; i++)
    {
        // Copy the (length - i)th byte
        packet->data[shift + packet->length - i] = packet->data[packet->length
                - i];
    }

    // Update the data pointer
    packet->data += shift;

    return PACKET_SUCCESS;
}

uint32_t packet_available()
{
    uint32_t i, count = 0;

    // Take the mutex
    xSemaphoreTake(mutex, portMAX_DELAY);

    // Count the number of unused packet
    for (i = 0; i < packet_storage.packet_number; i++)
    {
        if (!packet_flag_test(i))
        {
            count++;
        }
    }

    // Give the mutex
    xSemaphoreGive(mutex);

    // Return the counted value
    return count;
}

void packet_fifo_append(packet_t **fifo, packet_t *packet)
{
    // Take the mutex
    xSemaphoreTake(mutex, portMAX_DELAY);

    // Insert first if first is NULL
    if (*fifo == NULL)
    {
        *fifo = packet;
    }
    else
    {
        packet_t *tmp_pkt = *fifo;

        // Loop until next is NULL
        for (tmp_pkt = *fifo; tmp_pkt->next != NULL; tmp_pkt = tmp_pkt->next)
        {
        }

        // Append
        tmp_pkt->next = packet;
    }

    // Packet is inserted, hence its next is NULL
    packet->next = NULL;

    // Give the mutex
    xSemaphoreGive(mutex);
}

packet_t *packet_fifo_get(packet_t **fifo)
{
    // Take the mutex
    xSemaphoreTake(mutex, portMAX_DELAY);

    packet_t *pkt = NULL;

    // if fifo is null, return null
    if (*fifo == NULL)
    {
        pkt = NULL;
    }
    else
    {
        // Take first
        pkt = *fifo;
        *fifo = pkt->next;
    }

    // Give the mutex
    xSemaphoreGive(mutex);
    return pkt;
}

static inline void packet_flag_set(uint32_t index)
{
    packet_storage.packet_flags[index / 32] |= (1 << (index & 0x1f));
}
static inline void packet_flag_clear(uint32_t index)
{
    packet_storage.packet_flags[index / 32] &= ~(1 << (index & 0x1f));
}
static inline uint32_t packet_flag_test(uint32_t index)
{
    return packet_storage.packet_flags[index / 32] & (1 << (index & 0x1f));
}
