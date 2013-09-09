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
 * test_lwip.c
 *
 *  Created on: Oct 17, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include "platform.h"
#include "printf.h"
#include "soft_timer.h"

#include "hkb-lwip.h"
#include "debug.h"

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "ipv4/lwip/ip_addr.h"

#define WEB_THREAD_PORT         80

static void web_task(void*);

int main()
{
    // Initialize the platform
    platform_init();
    soft_timer_init();

    log_info("Initializing HiKoB port of LwIP");
    hkb_lwip_init(NULL);

    // Create a Task for web server
    xTaskCreate(web_task, (const signed char*) "web", configMINIMAL_STACK_SIZE,
            NULL, 1, NULL);

    platform_run();
    return 0;
}

static const char http_html_hdr[] =
        "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
static const char http_index_html[] =
        "<html><head><title>HiKoB!</title></head><body><h1>Welcome to the HiKoB lwIP HTTP server (LwIP + STM32F4evalboard)!</h1><p>Un jour tu verras du contact sec ici.</body></html>";

static void http_server_serve(struct netconn *conn)
{
    struct netbuf *inbuf;
    char *buf;
    u16_t buflen;
    err_t err;

    /* Read the data from the port, blocking if nothing yet there.
     We assume the request (the part we care about) is in one netbuf */
    err = netconn_recv(conn, &inbuf);

    if (err == ERR_OK)
    {
        netbuf_data(inbuf, (void **) &buf, &buflen);

        ip_addr_t ip;
        uint16_t port;
        netconn_getaddr(conn, &ip, &port, 0);
        log_info(
                "Got request from %u.%u.%u.%u:%u",
                ip.addr & 0xFF,
                (ip.addr >> 8) & 0xFF,
                (ip.addr >> 16) & 0xFF,
                (ip.addr >> 24) & 0xFF, port);

        /* Is this an HTTP GET command? (only check the first 5 chars, since
         there are other formats for GET, and we're keeping it very simple )*/
        if (buflen >= 5 && buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T'
                && buf[3] == ' ' && buf[4] == '/')
        {

            /* Send the HTML header
             * subtract 1 from the size, since we dont send the \0 in the string
             * NETCONN_NOCOPY: our data is const static, so no need to copy it
             */
            netconn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1,
                    NETCONN_NOCOPY);

            /* Send our HTML page */
            netconn_write(conn, http_index_html, sizeof(http_index_html) - 1,
                    NETCONN_NOCOPY);
        }
    }
    /* Close the connection (server closes in HTTP) */
    netconn_close(conn);

    /* Delete the buffer (netconn_recv gives us ownership,
     so we have to make sure to deallocate the buffer) */
    netbuf_delete(inbuf);
}

static void web_task(void* p)
{
    struct netconn *conn, *newconn;
    err_t err;

    /* Create a new TCP connection handle */
    conn = netconn_new(NETCONN_TCP);
    LWIP_ERROR("http_server: invalid conn", (conn != NULL),
            while (1){vTaskDelay(configTICK_RATE_HZ);});

    /* Bind to port 80 (HTTP) with default IP address */
    netconn_bind(conn, NULL, WEB_THREAD_PORT);

    /* Put the connection into LISTEN state */
    netconn_listen(conn);

    while (1)
    {
        err = netconn_accept(conn, &newconn);
        if (err != ERR_OK)
            continue;
        http_server_serve(newconn);
        netconn_delete(newconn);
    }
}
