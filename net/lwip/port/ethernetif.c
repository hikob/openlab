/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#include "hkb-lwip.h"

#include "lwip/opt.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/tcpip.h"
#include "lwip/dhcp.h"

#include "netif/etharp.h"

#include "ethmac.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

/* Forward declarations. */
static void ethernetif_input(struct netif *netif);

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif)
{
    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* maximum transfer unit */
    netif->mtu = 1500;

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

    /* Do whatever else is needed to initialize interface. */
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    ethmac_tx_t tx;
    struct pbuf *q;

    // Try to get a tx descriptor
    if (!ethmac_tx_init(&tx))
    {
        log_error("Failed to get a TX descriptor");
        return ERR_TIMEOUT;
    }

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    for (q = p; q != NULL; q = q->next)
    {
        /* Send the data from the pbuf to the interface, one pbuf at a
         time. The size of the data in each pbuf is kept in the ->len
         variable. */

        // Append this pbuf to the TX descriptor
        ethmac_tx_write(&tx, q->payload, q->len);
    }

    // Send the current TX descriptor
    ethmac_tx_send(&tx);

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.xmit);

    return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
    ethmac_rx_t rx;
    struct pbuf *p, *q;
    u16_t len;

    // Check if a packet is available
    if (!ethmac_rx_init(&rx))
    {
        log_error("Failed to get a RX descriptor");
        return NULL;
    }

    /* Obtain the size of the packet and put it into the "len"
     variable. */
    len = ethmac_rx_get_len(&rx);

#if ETH_PAD_SIZE
    len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

    if (p != NULL)
    {

#if ETH_PAD_SIZE
        pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

        /* We iterate over the pbuf chain until we have read the entire
         * packet into the pbuf. */
        for (q = p; q != NULL; q = q->next)
        {
            /* Read enough bytes to fill this pbuf in the chain. The
             * available data in the pbuf is given by the q->len
             * variable.
             * This does not necessarily have to be a memcpy, you can also preallocate
             * pbufs for a DMA-enabled MAC and after receiving truncate it to the
             * actually received size. In this case, ensure the tot_len member of the
             * pbuf is the sum of the chained pbuf len members.
             */
            ethmac_rx_read(&rx, q->payload, q->len);
        }

#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

        LINK_STATS_INC(link.recv);
    }
    else
    {
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
    }

    // Release the descriptor
    ethmac_rx_release(&rx);

    return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
static void ethernetif_input(struct netif *netif)
{
    struct eth_hdr *ethhdr;
    struct pbuf *p;

    /* move received packet into a new pbuf */
    p = low_level_input(netif);
    /* no packet could be read, silently ignore this */
    if (p == NULL)
        return;
    /* points to packet payload, which starts with an Ethernet header */
    ethhdr = p->payload;

    switch (htons(ethhdr->type))
    {
        /* IP or ARP packet? */
        case ETHTYPE_IP:
        case ETHTYPE_ARP:
#if PPPOE_SUPPORT
            /* PPPoE packet? */
            case ETHTYPE_PPPOEDISC:
            case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
            /* full packet send to tcpip_thread to process */
            if (netif->input(p, netif) != ERR_OK)
            {
                LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
                pbuf_free(p);
                p = NULL;
            }
            break;

        default:
            pbuf_free(p);
            p = NULL;
            break;
    }
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

    netif->state = NULL;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}
/* ***************************** Here takes place real things ************** */
#include "event.h"
#include "unique_id.h"
#include "soft_timer.h"

void check_status(handler_arg_t arg);

static struct netif ethif;
static struct ip_addr ip, gateway, netmask;

static soft_timer_t status_tim;

void hkb_lwip_init(struct lwip_config *config)
{
    /* Initializes the lwIP modules */
    sys_init();
    mem_init();
    memp_init();
    pbuf_init();
    netif_init();
    ip_init();
    tcpip_init(NULL, NULL);

    /* TCP/IP parameters, runtime or compile time.*/
    if (config)
    {
        unsigned i;

        for (i = 0; i < 6; i++)
            ethif.hwaddr[i] = config->macaddress[i];
        ip.addr = config->address;
        gateway.addr = config->gateway;
        netmask.addr = config->netmask;
    }
    else
    {
        /* set MAC hardware address */
        ethif.hwaddr[0] = 0xba;
        ethif.hwaddr[1] = 0xdb;
        ethif.hwaddr[2] = 0x0b;
        ethif.hwaddr[3] = uid->uid8[9];
        ethif.hwaddr[4] = uid->uid8[10];
        ethif.hwaddr[5] = uid->uid8[11];

        IP4_ADDR(&ip, 0, 0, 0, 0);
        IP4_ADDR(&gateway, 0, 0, 0, 0);
        IP4_ADDR(&netmask, 0, 0, 0, 0);
    }

    log_debug("Starting MAC driver");

    // Start the MAC
    ethmac_enable();
    ethmac_start(ethif.hwaddr);

    // Add the interface
    netif_add(&ethif, &ip, &netmask, &gateway, NULL, ethernetif_init,
            tcpip_input);

    // Set default
    netif_set_default(&ethif);
    netif_set_up(&ethif);

    // Start DHCP
    if (ip.addr == 0)
    {
        log_info("Starting DHCP");
        dhcp_start(&ethif);
    }

    // Done!

    // Set soft timer for periodic link checks
    soft_timer_set_handler(&status_tim, check_status, NULL);
    soft_timer_start(&status_tim, soft_timer_s_to_ticks(2), 1);
}

void check_status(handler_arg_t arg)
{
    if (ethmac_get_link_status())
    {
        if (netif_is_link_up(&ethif) == 0)
        {
            // Status changed to UP
            log_debug("Link went UP");
            tcpip_callback_with_block((tcpip_callback_fn) netif_set_link_up,
                    &ethif, 0);
        }
    }
    else if (netif_is_link_up(&ethif))
    {
        // Status changed to DOWN
        log_debug("Link went DOWN");
        tcpip_callback_with_block((tcpip_callback_fn) netif_set_link_down,
                &ethif, 0);
    }
}

void ethmac_frame_received_isr()
{
    event_post_from_isr(EVENT_QUEUE_APPLI, (handler_t) ethernetif_input,
            &ethif);
}
void ethmac_frame_sent_isr()
{
}
