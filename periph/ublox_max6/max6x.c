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

/**
 * max6x.c
 *
 *  Created on: Feb 27, 2012
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 *      Author: Guillaume Roche <guillaume.roche.at.hikob.com>
 */

#include "platform.h"
#include "stm32f1xx/afio.h"
#include "nvic.h"
#include "exti.h"
#include "soft_timer.h"
#include "max6x_.h"
#include "max6x.h"
#include "ublox_class.h"

#define LOG_LEVEL LOG_LEVEL_DEBUG
#include "printf.h"
#include "debug.h"

typedef enum
{
    IDLE,
    HEADER1,
    HEADER2,
    CLASS,
    ID,
    LENGTH1,
    LENGTH2,
    PAYLOAD,
    CK_A
} state_t;

static void max6x_read(handler_arg_t dummy);

static max6x_handler_t nav_hdl = NULL;
static soft_timer_t timer;

static bool max6x_configure_ddc()
{
    // Message to deactivate UART 1
    uint8_t msg1[20] = {1, 0, 0, 0, 0xC0, 0x08, 0, 0, 38400 & 0xFF, 38400 >> 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    // Payload:
    //  portID: 0 (DDC port)
    //  res0: 0x00 (reserved)
    //  txready: 0x017 (enabled, active low, GPIO5 (TXD1), no threshold)
    //  mode: 0x00000084 (default address)
    //  res2: 0x00000000 (reserved)
    //  inProtoMask: 0x0001 (only UBX input protocol is active)
    //  outProtoMask: 0x0001 (only UBX output protocol is active)
    //  flags: 0x0000 (reserved)
    //  pad: 0x0000 (reserved)
    uint8_t msg2[20] = {0, 0, 0x17, 0, MAX6X_BASE_ADD, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0};

    return max6x_send_msg(MAX6x_CLASS_CFG, MAX6x_ID_PRT, sizeof(msg1), msg1) ||
           max6x_send_msg(MAX6x_CLASS_CFG, MAX6x_ID_PRT, sizeof(msg2), msg2);
}

// PA.4 (O) : EN_GPS*
// PA.6 (I) : DATA_READY
// PA.7 (I) : PULSE
// PB.0 (O) : ANTENNA_SELECTION
// PC.5 (O) : RESET_GPS
bool max6x_init()
{
    // Disable I2C first as GPS module will generate errors
    i2c_disable(max6x_config.i2c);

    // Default to PCB antenna
    if (max6x_config.antsel_gpio)
    {
        log_info("GPS Antenna selection is configured");
        gpio_set_output(max6x_config.antsel_gpio, max6x_config.antsel_pin);
        max6x_select_antenna(MAX6X_ANTENNA_PATCH);
    }

    // Set regulator to 500mA
    gpio_set_output(max6x_config.reg_gpio, max6x_config.reg_pin);
    gpio_pin_set(max6x_config.reg_gpio, max6x_config.reg_pin);

    // Enable GPS by setting EN_GPS* to 0
    gpio_set_output(max6x_config.en_gpio, max6x_config.en_pin);
    gpio_pin_clear(max6x_config.en_gpio, max6x_config.en_pin);

    // Configure RESET_GPS pin and to a 500ms pulse to reset the GPS
    gpio_set_output(max6x_config.rst_gpio, max6x_config.rst_pin);
    if (max6x_config.rst_high)
    {
        gpio_pin_clear(max6x_config.rst_gpio, max6x_config.rst_pin);
        soft_timer_delay_ms(500);
        gpio_pin_set(max6x_config.rst_gpio, max6x_config.rst_pin);
        soft_timer_delay_ms(500);
    }
    else
    {
        gpio_pin_set(max6x_config.rst_gpio, max6x_config.rst_pin);
        soft_timer_delay_ms(500);
        gpio_pin_clear(max6x_config.rst_gpio, max6x_config.rst_pin);
        soft_timer_delay_ms(500);
    }

    // Configure the DATA_READY pin as interruptible
    gpio_set_input(max6x_config.ready_gpio, max6x_config.ready_pin);
    afio_select_exti_pin(max6x_config.ready_exti_line, max6x_config.ready_afio_port);
    nvic_enable_interrupt_line(max6x_config.ready_nvic_line);

    //! \todo make this pin EXTI
    gpio_set_input(max6x_config.pulse_gpio, max6x_config.pulse_pin); // PULSE

    // Create tx ready polling timer
    soft_timer_set_handler(&timer, max6x_read, NULL);

    // Enable I2C when GPS module has started up
    i2c_enable(max6x_config.i2c, I2C_CLOCK_MODE_STANDARD);

    // Configure GPS module to send only on I2C bus
    if (max6x_configure_ddc())
    {
        i2c_disable(max6x_config.i2c);
        i2c_enable(max6x_config.i2c, I2C_CLOCK_MODE_FAST);
        return true;
    }
    return false;
}

void max6x_select_antenna(max6x_antenna_t ant)
{
    if(max6x_config.antsel_gpio)
    {
        switch (ant)
        {
            case MAX6X_ANTENNA_PATCH:
                gpio_pin_set(max6x_config.antsel_gpio, max6x_config.antsel_pin);
                break;
            case MAX6X_ANTENNA_CONNECTOR:
                gpio_pin_clear(max6x_config.antsel_gpio, max6x_config.antsel_pin);
                break;
        }
    }
}

inline static bool max6x_is_tx_ready()
{
    uint32_t r = gpio_pin_read(max6x_config.ready_gpio, max6x_config.ready_pin);

    return r == 0;
}

bool max6x_reset()
{
    // Payload:
    //  navBbrMask: 0xFFFF (Coldstart)
    //  resetMode: 0x00 (Hardware reset immediately)
    //  reserved: 0x00 ()
    uint8_t msg[4] = {0xFF, 0xFF, 0x00, 0x00};

    return max6x_send_msg(MAX6x_CLASS_CFG, MAX6x_ID_RST, sizeof(msg), msg);
}

bool max6x_coldstart()
{
    // Payload:
    //  navBbrMask: 0xFFFF (Coldstart)
    //  resetMode: 0x09 (Controlled GPS start)
    //  reserved: 0x00
    uint8_t msg[4] = {0xFF, 0xFF, 0x09, 0x00};

    return max6x_send_msg(MAX6x_CLASS_CFG, MAX6x_ID_RST, sizeof(msg), msg);
}

bool max6x_hotstart()
{
    // Payload:
    //  navBbrMask: 0x0000 (Hotstart)
    //  resetMode: 0x09 (Controlled GPS start)
    //  reserved: 0x00
    uint8_t msg[4] = {0x00, 0x00, 0x09, 0x00};

    return max6x_send_msg(MAX6x_CLASS_CFG, MAX6x_ID_RST, sizeof(msg), msg);
}

bool max6x_set_ubx_protocol()
{
    // Payload:
    //  protocolID: 0 (UBX Protocol)
    //  res0: 0x00 (reserved)
    //  res1: 0x0000 (reserved)
    //  infMsgMask: 0x00 * 6 (no information message)
    uint8_t msg[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    return max6x_send_msg(MAX6x_CLASS_CFG, MAX6x_ID_INF, sizeof(msg), msg);
}

bool max6x_send_periodically(uint8_t class, uint8_t id)
{
    // Payload:
    //  msgClass: class
    //  msgID: id
    //  rate: 1
    uint8_t msg[3] = {class, id, 1};

    return max6x_send_msg(MAX6x_CLASS_CFG, MAX6x_ID_MSG, sizeof(msg), msg);
}

bool max6x_set_rate(uint16_t rate)
{
    // Payload:
    //  measRate: rate (in ms)
    //  navRate: 1 (cannot be anything else)
    //  timeRef: 1 (GPS time)
    uint8_t msg[6] = {rate & 0xFF, rate >> 8, 1, 0, 1, 0};

    return max6x_send_msg(MAX6x_CLASS_CFG, MAX6x_ID_RATE, sizeof(msg), msg);
}

bool max6x_configure_power_management()
{
    // Payload:
    //  reserved: 0x00 (reserved)
    //  lpMode: 0x01 (Power Save Mode)
    uint8_t msg1[2] = {0x00, 0x01};
    // Payload:
    //  version: 0x00 (current version: 0)
    //      res1: 0x00 (reserved)
    //      res2: 0x00 (reserved)
    //      res3: 0x00 (reserved)
    //  flags: 0x00000364
    //  updatePeriod: 0x00002710 (10000ms)
    //  searchPeriod: 0x00002710 (10000ms)
    //  gridOffset: 0x00000000 (0ms)
    //  onTime: 0x0002 (2s)
    //  minAcqTime: 0x0000 (0s)
    uint8_t msg2[24] = {0, 0, 0, 0, 0x64, 0x03, 0, 0, 0x10, 0x27, 0, 0, 0x10, 0x27, 0, 0, 0, 0, 0, 0, 0x02, 0, 0, 0};

    return max6x_send_msg(MAX6x_CLASS_CFG, MAX6x_ID_RXM, sizeof(msg1), msg1) ||
           max6x_send_msg(MAX6x_CLASS_CFG, MAX6x_ID_PM, sizeof(msg2), msg2);
}

/**
UBX Message :
_________________________________________________________________________________
|      |      |           |        |            |                 |             |
| 0xB5 | 0x62 | Class (1) | ID (1) | Length (2) | Message(length) | Checksum (2)|
|______|______|___________|________|____________|_________________|_____________|
**/

bool max6x_send_msg(uint8_t class, uint8_t id, uint16_t len, uint8_t *msg)
{
    uint8_t ck_a = 0, ck_b = 0, buf[255];
    uint16_t i;

    buf[0] = 0xB5;
    buf[1] = 0x62;
    buf[2] = class;
    buf[3] = id;

    buf[4] = (uint8_t)(len & 0xFF);
    buf[5] = (uint8_t)(len >> 8);

    for (i = 0; i < len; i++)
    {
        buf[6 + i] = msg[i];
    }

    for (i = 2; i < 6 + len; i++)
    {
        ck_a += buf[i];
        ck_b += ck_a;
    }

    buf[6 + len] = ck_a;
    buf[7 + len] = ck_b;

    return i2c_tx(max6x_config.i2c, MAX6X_BASE_ADD, buf, len + 8);
}

static state_t state = IDLE;

void max6x_decoder_reset()
{
    state = IDLE;
}

// GPS state machine iteration
static void max6x_parse(uint8_t c, max6x_handler_t hdl)
{
    static uint16_t read_bytes = 0, length = 0, i;
    static uint8_t buf[1024] __attribute__((aligned(4)));
    static uint8_t ck_a = 0, ck_b = 0;

    switch (state)
    {
        case IDLE:

            if (c == 0xB5)
            {
                state = HEADER1;
            }

            break;

        case HEADER1:

            if (c == 0x62)
            {
                ck_a = 0;
                ck_b = 0;
                state = HEADER2;
            }
            else
            {
                state = IDLE;
            }

            break;

        case HEADER2:
            buf[0] = c;

            ck_a += c;
            ck_b += ck_a;

            state = CLASS;
            break;

        case CLASS:
            buf[1] = c;

            ck_a += c;
            ck_b += ck_a;

            state = ID;
            break;

        case ID:
            length = buf[2] = c;

            ck_a += c;
            ck_b += ck_a;

            state = LENGTH1;
            break;

        case LENGTH1:
            buf[3] = c;

            ck_a += c;
            ck_b += ck_a;

            length |= ((uint16_t)c) << 8;
            state = LENGTH2;
            read_bytes = 0;
            break;

        case LENGTH2:
            read_bytes++;
            buf[3 + read_bytes] = c;

            ck_a += c;
            ck_b += ck_a;

            if (read_bytes == length)
            {
                state = PAYLOAD;
            }

            break;

        case PAYLOAD:

            if (ck_a == c)
            {
                state = CK_A;
            }
            else
            {
                log_error("Wrong CRC: CK_A does not match (0x%02X should be 0x%02X)", c, ck_a);
                state = IDLE;
                for(i = 0; i < length + 3; i++)
                {
                    log_printf("%02X ", buf[i]);
                }
            }

            break;

        case CK_A:

            if (ck_b == c)
            {
                if (buf[0] == MAX6x_CLASS_NAV)
                {
                    if(hdl != NULL)
                    {
                        hdl(buf, length + 3);
                    }
                }
                else
                {
                    log_info("Packet class %d not handled", buf[0]);
                }
            }
            else
            {
                log_error("Wrong CRC: CK_B does not match (0x%02X should be 0x%02X)", c, ck_b);
            }

            state = IDLE;
            break;
    }
}

static void max6x_read(handler_arg_t dummy)
{
    static uint8_t reg = 0xFD;
    static uint8_t buf[1024];
    static uint16_t i, size;

    log_debug("TOP");

    if(max6x_is_tx_ready())
    {
        i2c_tx_rx(max6x_config.i2c, MAX6X_BASE_ADD, &reg, 1, buf, 2);
       
        size = (buf[0] << 8) | buf[1];
        if(size > sizeof(buf)) size = sizeof(buf);

        log_debug("DATA: %u", size);
        if(size != 0)
        {
            i2c_rx(max6x_config.i2c, MAX6X_BASE_ADD, buf, size);

            for(i = 0; i < sizeof(buf); i++)
            {
                max6x_parse(buf[i], nav_hdl);
            }
        }
    }
}

void max6x_start()
{
    soft_timer_start(&timer, soft_timer_ms_to_ticks(100), true);
//    exti_set_handler(EXTI_LINE_Px6, max6x_data_available, NULL);
//    exti_enable_interrupt_line(EXTI_LINE_Px6, EXTI_TRIGGER_FALLING);
}

void max6x_register_nav_handler(max6x_handler_t hdl)
{
    nav_hdl = hdl;
}

bool max6x_is_rdy()
{
    return max6x_is_tx_ready();
}

void max6x_set_rdy_handler(handler_t handler, handler_arg_t arg)
{
    exti_disable_interrupt_line(max6x_config.ready_exti_line);
    if (handler != NULL)
    {
        exti_set_handler(max6x_config.ready_exti_line, handler, arg);
        exti_enable_interrupt_line(max6x_config.ready_exti_line, EXTI_TRIGGER_FALLING);
    }
}

uint32_t max6x_data_count(uint16_t *count, result_handler_t handler, handler_arg_t arg)
{
    uint8_t reg = 0xFD;

    if (handler)
    {
        i2c_tx_rx_async(max6x_config.i2c, MAX6X_BASE_ADD, &reg, 1, (uint8_t *) count, 2, handler, arg);
        return 0;
    }
    return i2c_tx_rx(max6x_config.i2c, MAX6X_BASE_ADD, &reg, 1, (uint8_t *) count, 2);
}

uint32_t max6x_data_read(uint8_t *buf, uint16_t size, result_handler_t handler, handler_arg_t arg)
{
    // read data
    // do not send register address
    // we suppose a max6x_data_count has been done before
    if (handler)
    {
        i2c_tx_rx_async(max6x_config.i2c, MAX6X_BASE_ADD, NULL, 0, buf, size, handler, arg);
        return 0;
    }
    return i2c_tx_rx(max6x_config.i2c, MAX6X_BASE_ADD, NULL, 0, buf, size);
}

void max6x_data_decode(uint8_t *buf, uint16_t size, max6x_handler_t hdl)
{
    int i;
    for(i = 0; i < size; i++)
    {
        max6x_parse(buf[i], hdl);
    }
}
