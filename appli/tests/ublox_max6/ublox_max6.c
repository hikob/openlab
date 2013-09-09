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
 * ublox_max6.c
 *
 *  Created on: Feb 27, 2012
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */
#include <stdint.h>
#include "platform.h"

#include "ublox_max6/max6x.h"
#include "ublox_max6/packet.h"
#include "ublox_max6/max6x_.h"

#include "printf.h"
#include "ansi.h"
#include "debug.h"

static max6x_antenna_t max6x_ant;

void hdl(uint8_t *buf, uint16_t length)
{
    static bool fix = false;
    static uint8_t cpt = 0, i, t;
    static uint8_t *b;
    max6x_nav_posllh_t *p1;
    max6x_nav_sol_t *p2;
    max6x_nav_svinfo_header_t *p3;
    max6x_nav_svinfo_channel_t *c;

    (void) fix;
    (void) t;

    p1 = (max6x_nav_posllh_t *)buf;
    p2 = (max6x_nav_sol_t *)buf;
    p3 = (max6x_nav_svinfo_header_t *)buf;

    if (p3->id == MAX6x_ID_SVINFO)
    {
        t = 0;

        for (i = 0; i < p3->numCh; i++)
        {
            b = buf + sizeof(max6x_nav_svinfo_header_t) + i * sizeof(max6x_nav_svinfo_channel_t);
            c = (max6x_nav_svinfo_channel_t *)b;

            ansi_goto(1 + 49 * (i % 2), 14 + i / 2);

            if (c->flags & 0x01)
            {
                if (c->flags & 0x10)
                {
                    ansi_set_text_color(ANSI_YELLOW);
                }
                else
                {
                    ansi_set_text_color(ANSI_GREEN);
                }
            }
            else
            {
                ansi_set_text_color(ANSI_RED);
            }

            printf("Sat #%u(%u) %03udbHz (%d°, %d°) qual: %u   %c%c       ", c->svid, c->channel, c->cno, c->elev, c->azim, c->quality, c->flags & 0x04 ? '+' : '-', c->flags & 0x08 ? '+' : '-');
        }
    }
    else if (p1->id == MAX6x_ID_POSLLH)
    {
        leds_toggle(LED_0);

        ansi_set_text_color(ANSI_WHITE);
        ansi_goto(1, 6);
        printf("Lon: %f°      ", p1->lon * 1e-7);
        ansi_goto(25, 6);
        printf("Lat: %f°      ", p1->lat * 1e-7);
        ansi_goto(50, 6);
        printf("Height: %fm      ", p1->height * 1e-3);

        ansi_goto(0, 11);
        printf("Horizontal accuracy: %f       ", p1->hAcc * 1e-3);
        ansi_goto(0, 12);
        printf("Vertical accuracy:   %f       ", p1->vAcc * 1e-3);
    }
    else if (p2->id == MAX6x_ID_SOL)
    {
        switch (p2->gpsFix)
        {
            case 0:
                fix = false;
                leds_off(LED_1);
                ansi_set_text_color(ANSI_RED);
                break;

            case 5:
                fix = false;
                leds_off(LED_1);
                ansi_set_text_color(ANSI_YELLOW);
                break;

            default:
                fix = true;
                leds_on(LED_1);
                ansi_set_text_color(ANSI_GREEN);
        }
        ansi_goto(1, 1);
        printf("GPS Fix: 0x%02X numSV: %d", p2->gpsFix, p2->numSV);
        ansi_goto(1, 2);
        printf("Antenna: %s", (max6x_ant == MAX6X_ANTENNA_PATCH) ? "PATCH      " : "CONNECTOR ");

        ansi_set_text_color(ANSI_WHITE);
        ansi_goto(1, 4);
        printf("Time of week: %fs       ", p2->iTOW * 1e-3);
        ansi_goto(50, 4);
        printf("Week: %d       ", p2->week);
        ansi_goto(1, 8);
        printf("X:   %fm       ", p2->ecefX * 1e-2);
        ansi_goto(25, 8);
        printf("Y:   %fm       ", p2->ecefY * 1e-2);
        ansi_goto(50, 8);
        printf("Z:      %fm       ", p2->ecefZ * 1e-2);
        ansi_goto(1, 9);
        printf("VX:  %fm/s       ", p2->ecefVX * 1e-2);
        ansi_goto(25, 9);
        printf("VY:  %fm/s       ", p2->ecefVY * 1e-2);
        ansi_goto(50, 9);
        printf("VZ:     %fm/s       ", p2->ecefVZ * 1e-2);
    }

    cpt++;
    ansi_goto(50, 0);
    ansi_set_text_color(ANSI_CYAN);

    switch ((cpt >> 1) & 0x3)
    {
        case 0:
            printf("-");
            break;
        case 1:
            printf("\\");
            break;
        case 2:
            printf("|");
            break;
        case 3:
            printf("/");
            break;
    }
}

static void button_handler(handler_arg_t arg)
{
    switch (max6x_ant)
    {
        case MAX6X_ANTENNA_PATCH:
            max6x_ant = MAX6X_ANTENNA_CONNECTOR;
            break;
        case MAX6X_ANTENNA_CONNECTOR:
            max6x_ant = MAX6X_ANTENNA_PATCH;
            break;
    }
    max6x_select_antenna(max6x_ant);
}

static void uart_char_recv (handler_arg_t arg, uint8_t c)
{
    if (c == 'a' || c == 'A')
    {
        switch (max6x_ant)
        {
            case MAX6X_ANTENNA_PATCH:
                max6x_ant = MAX6X_ANTENNA_CONNECTOR;
                break;
            case MAX6X_ANTENNA_CONNECTOR:
                max6x_ant = MAX6X_ANTENNA_PATCH;
                break;
        }
        max6x_select_antenna(max6x_ant);
    }
}

int main()
{
    // Initialize the platform
    platform_init();

    soft_timer_init();
    asm("cpsie i\n"
            "mov r0, #0\n"
            "msr basepri, r0\n");

    log_info("U-Blox MAX6 GPS test");

    if (!max6x_init())
    {
        log_debug("Init done");
    }
    else
    {
        log_error("Init failed");
    }

    max6x_ant = MAX6X_ANTENNA_PATCH;
    max6x_select_antenna(max6x_ant);
    button_set_handler(button_handler, NULL);
    uart_set_rx_handler(uart_print, uart_char_recv, NULL);

    if (!max6x_coldstart())
    {
        log_debug("Init done");
    }
    else
    {
        log_error("Init failed");
    }

    if (!max6x_send_periodically(MAX6x_CLASS_NAV, MAX6x_ID_POSLLH))
    {
        log_debug("Cold start");
    }
    else
    {
        log_error("Cold start failed");
    }


    if (!max6x_send_periodically(MAX6x_CLASS_NAV, MAX6x_ID_SOL))
    {
        log_debug("Sending periodically position");
    }
    else
    {
        log_error("Sending periodically position failed");
    }

    if (!max6x_send_periodically(MAX6x_CLASS_NAV, MAX6x_ID_SVINFO))
    {
        log_debug("Sending periodically navigation solution");
    }
    else
    {
        log_error("Sending periodically navigation solution failed");
    }


    if (!max6x_set_rate(200))
    {
        log_debug("Set sending rate to 5Hz");
    }
    else
    {
        log_error("Set rate failed");
    }

    max6x_register_nav_handler(hdl);
    log_debug("Set handler");

    ansi_clear_screen();

    max6x_start(20);
    platform_run();

    return 0;
}

