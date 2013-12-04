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
 * phy_rf2xx.h
 *
 *  Created on: Jul 11, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef PHY_RF2XX_H_
#define PHY_RF2XX_H_

#include "phy.h"
#include "timer.h"
#include "rf2xx.h"
#include "soft_timer.h"

typedef enum
{
    PHY_STATE_SLEEP = 0,
    PHY_STATE_IDLE = 1,
    PHY_STATE_RX_WAIT = 2,
    PHY_STATE_RX = 3,
    PHY_STATE_TX_WAIT = 4,
    PHY_STATE_TX = 5,
    PHY_STATE_JAMMING = 6,
} phy_rf2xx_state_t;

#define PHY_TIMING__TX_OFFSET soft_timer_us_to_ticks(16 + 192 + 9)

typedef struct
{
    // Pointer to the radio chip
    rf2xx_t radio;

    // Timer for timed alarm
    openlab_timer_t timer;
    // Timer Channel for alarm
    timer_channel_t channel;

    /** 1 if the timer channel controls the SLP_TR pin */
    uint32_t timer_is_slptr;

    // Pointers to the packet used in TX or RX
    phy_packet_t *pkt;

    // Running State
    volatile phy_rf2xx_state_t state;

    // Handler
    phy_handler_t handler;

    // RX timeout
    uint32_t rx_timeout;
} phy_rf2xx_t;

/**
 * Initialize the PHY layer
 *
 * \param phy the PHY to init.
 * \param radio the RF2xx radio used by the PHY;
 * \param timer a timer to provide timeouts and so;
 * \param channel a timer channel for timeouts and so;
 */
void phy_rf2xx_init(phy_rf2xx_t *phy, rf2xx_t radio, openlab_timer_t timer,
                    timer_channel_t channel);

#endif /* PHY_RF2XX_H_ */
