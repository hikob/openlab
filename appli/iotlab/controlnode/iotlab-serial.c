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
 * iotlab-serial.c
 *
 *  Created on: Aug 12, 2013
 *      Author: burindes
 */
#include "platform.h"
#include "iotlab-serial.h"

#include "debug.h"
#include "event.h"

#if defined(RELEASE) && RELEASE
#define ASYNCHRONOUS 1
#else // ASYNCHRONOUS
#define ASYNCHRONOUS 0
#endif // ASYNCHRONOUS
enum
{
    SYNC_BYTE = 0x80,

    ACK = 0x0A,
    NACK = 0x02
};

/** Handler for IDLE check */
static int32_t check_uart(handler_arg_t arg);
/** Handler for character received */
static void char_rx(handler_arg_t arg, uint8_t c);

static void allocate_packet(handler_arg_t arg);
static void packet_received(handler_arg_t arg);
static void send_now(handler_arg_t arg);
#if ASYNCHRONOUS
/** Function called at the end of a UART TX transfer */
static void tx_done_isr(handler_arg_t arg);
#endif // ASYNCHRONOUS
/** */
static void handle_packet_sent(handler_arg_t arg);

static struct
{
    iotlab_serial_handler_t *first_handler;

    /** Structure holding the TX information */
    struct
    {
        /** The FIFO of packets to send */
        packet_t *fifo;

        /** The packet in TX */
        packet_t *pkt;

        /** Flag indicating TX ongoing */
        volatile uint32_t busy;

        /** Flag to indicate end of asynchronous TX */
        volatile uint32_t irq_triggered;
    } tx;

    /** Structure holding RX information */
    struct
    {
        /** The packet being received */
        packet_t * volatile tmp_pkt;

        /** The complete received packet */
        packet_t * volatile ready_pkt;
    } rx;
} ser;

static inline int is_idle()
{
    return (ser.tx.pkt == NULL ) && (ser.tx.busy == 0);
}

static inline int is_sending()
{
    return (ser.tx.busy != 0);
}

void iotlab_serial_start(uint32_t baudrate)
{
    packet_init();
    uart_enable(uart_external, baudrate);

    // Clear the first handler
    ser.first_handler = NULL;

    // Clear RX/TX structures
    ser.tx.fifo = NULL;
    ser.tx.pkt = NULL;
    ser.tx.busy = 0;

    ser.rx.tmp_pkt = NULL;
    ser.rx.ready_pkt = NULL;

    // Configure serial port
    uart_set_rx_handler(uart_external, char_rx, NULL );

    // Set the UART priority higher than FreeRTOS limit, to receive all chars.
    // But be careful, the interrupt handler CANNOT use FreeRTOS or event functions
    // Therefore we use the platform IDLE handler to check for input
    uart_set_irq_priority(uart_external, 0x10);
    platform_set_idle_handler(check_uart, NULL );

    // allocate first packet
    allocate_packet(NULL );
}

void iotlab_serial_register_handler(iotlab_serial_handler_t *handler)
{
    // Insert last
    handler->next = NULL;

    if (ser.first_handler == NULL )
    {
        ser.first_handler = handler;
    }
    else
    {
        iotlab_serial_handler_t *h;
        for (h = ser.first_handler; h->next != NULL ; h = h->next)
        {
        }
        h->next = handler;
    }
}

int32_t iotlab_serial_send_frame(uint8_t type, packet_t *pkt)
{
    // Check length:
    if (pkt->length > PACKET_MAX_SIZE - IOTLAB_SERIAL_PACKET_OFFSET)
    {
        log_error("Serial Packet TX length too big: %u", pkt->length);
        return 0;
    }

    // We need to insert the header
    if (pkt->data - pkt->raw_data < IOTLAB_SERIAL_PACKET_OFFSET)
    {
        // We need to move the data
        if (packet_move_data_right(pkt,
                IOTLAB_SERIAL_PACKET_OFFSET - (pkt->data - pkt->raw_data))
                != PACKET_SUCCESS)
        {
            log_error("Serial packet too big to add header");
            return 0;
        }
    }

    // Shift the data pointer a header on the left
    pkt->data -= 3;

    // Set header
    pkt->data[0] = SYNC_BYTE;
    pkt->data[1] = pkt->length + 1;
    pkt->data[2] = type;
    pkt->length += 3;

    // Append to FIFO
    packet_fifo_append(&ser.tx.fifo, pkt);

    // Send if idle
    if (is_idle())
    {
        // Try to send now
        send_now(NULL );
    }

    // Success, return OK
    return 1;
}

static void char_rx(handler_arg_t arg, uint8_t c)
{
    /*
     * HIGH PRIORITY Interrupt
     *
     * Do not use FreeRTOS or Event library functions!!!
     */
    static uint16_t rx_index = 0;
    static uint32_t last_start_time = 0;

    // Check for ready buffer
    if (ser.rx.tmp_pkt == NULL )
    {
        // Request allocation
        rx_index = 0;
        return;
    }

    // Check if packet started too long ago
    if (last_start_time
            && (soft_timer_time() - last_start_time
                    > soft_timer_ms_to_ticks(100)))
    {
        // Reset index
        rx_index = 0;
        last_start_time = 0;
    }

    // A char is received, switch index
    switch (rx_index)
    {
        case 0:
            // the received char should be a start
            if (c != SYNC_BYTE)
            {
                // Abort
                return;
            }
            // Store time
            last_start_time = soft_timer_time();
            break;
        case 1:
            // length byte
            ser.rx.tmp_pkt->length = 2 + c;
            break;
        default:
            // Proceed
            break;
    }

    // Save byte
    ser.rx.tmp_pkt->data[rx_index] = c;

    // Increment
    rx_index++;

    if (rx_index < 2)
    {
        return;
    }

    // Check length
    if (rx_index == ser.rx.tmp_pkt->length)
    {
        // Reset index
        rx_index = 0;
        last_start_time = 0;

        // Switch buffers
        ser.rx.ready_pkt = ser.rx.tmp_pkt;
        ser.rx.tmp_pkt = NULL;
    }
}
static int32_t check_uart(handler_arg_t arg)
{
    if (ser.rx.ready_pkt)
    {
        event_post(EVENT_QUEUE_APPLI, packet_received, NULL );
        return 1;
    }
    else if (ser.rx.tmp_pkt == NULL )
    {
        event_post(EVENT_QUEUE_APPLI, allocate_packet, NULL );
        return 1;
    }
    if (ser.tx.irq_triggered)
    {
        event_post(EVENT_QUEUE_APPLI, handle_packet_sent, NULL );
        return 1;
    }

    return 0;
}
static void allocate_packet(handler_arg_t arg)
{
    if (ser.rx.tmp_pkt == NULL )
    {
        // Allocate a new packet for RX
        ser.rx.tmp_pkt = packet_alloc(IOTLAB_SERIAL_PACKET_OFFSET);
    }
}
static void packet_received(handler_arg_t arg)
{
    packet_t *rx_pkt;

    // Allocate a new packet for RX
    allocate_packet(NULL );

    // Get the ready packet
    rx_pkt = ser.rx.ready_pkt;

    // Prepare packets for next RX
    ser.rx.ready_pkt = NULL;

    // Get the command type header
    uint8_t cmd_type = rx_pkt->data[2];

    // Loop over the registered handlers to find a match
    iotlab_serial_handler_t *handler = ser.first_handler;

    int32_t result = 0;

    // Remove header
    rx_pkt->data += 3;
    rx_pkt->length -= 3;

    int found = 0;
    while (handler != NULL )
    {
        if (handler->cmd_type == cmd_type)
        {
            // Found! process
            found = 1;
            result = handler->handler(cmd_type, rx_pkt);
            break;
        }

        // Increment
        handler = handler->next;
    }
    if (!found)
    {
        log_warning("No Handler found for command %x", cmd_type);
    }

    if (!result)
    {
        rx_pkt->length = 0;
    }

    // Reset header for response
    rx_pkt->data -= 4;
    rx_pkt->data[0] = SYNC_BYTE;
    rx_pkt->data[1] = rx_pkt->length + 2; // Length
    rx_pkt->data[2] = cmd_type; // Type
    rx_pkt->data[3] = result ? ACK : NACK;
    rx_pkt->length += 4;

    // Append to FIFO
    packet_fifo_append(&ser.tx.fifo, rx_pkt);

    // Send if idle
    if (is_idle())
    {
        // Try to send now
        send_now(NULL );
    }
}

static void send_now(handler_arg_t arg)
{
    // Check if busy
    if (is_sending())
    {
        log_error("Serial can't send because already Sending");
        return;
    }

    if (ser.tx.pkt != NULL )
    {
        log_error("Serial error: already a TX packet");
        packet_free(ser.tx.pkt);
        ser.tx.pkt = NULL;
    }

    // Set busy
    ser.tx.busy = 1;

    // Try to get a frame from the FIFO
    ser.tx.pkt = packet_fifo_get(&ser.tx.fifo);
    if (ser.tx.pkt == NULL )
    {
        // Nothing to send, set IDLE
        ser.tx.busy = 0;
        return;
    }

    ser.tx.irq_triggered = 0;

    // Start sending the packet
#if ASYNCHRONOUS
    uart_transfer_async(uart_external, ser.tx.pkt->data, ser.tx.pkt->length,
            tx_done_isr, NULL );
#else // ASYNCHRONOUS
    uart_transfer(uart_external, ser.tx.pkt->data, ser.tx.pkt->length);
    event_post(EVENT_QUEUE_APPLI, handle_packet_sent, NULL );
#endif // ASYNCHRONOUS
}

#if ASYNCHRONOUS
static void tx_done_isr(handler_arg_t arg)
{
    ser.tx.irq_triggered = 1;
}
#endif // ASYNCHRONOUS
static void handle_packet_sent(handler_arg_t arg)
{
    // Check there is a packet being sent
    if (ser.tx.pkt == NULL )
    {
        log_error("Packet sent but no packet!");
        return;
    }

    // Free the packet
    packet_free(ser.tx.pkt);
    ser.tx.pkt = NULL;

    // Clear the TX busy flag
    ser.tx.busy = 0;
    ser.tx.irq_triggered = 0;

    // Test if there is a packet to send
    if (ser.tx.fifo != NULL )
    {
        event_post(EVENT_QUEUE_APPLI, send_now, NULL );
    }
}
