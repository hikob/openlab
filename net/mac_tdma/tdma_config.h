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
 * tdma_config.h
 *
 * \date Jan 09, 2013
 * \author: Damien Hedde <damien.hedde.at.hikob.com>
 */

#ifndef MAC_TDMA_CONFIG_H_
#define MAC_TDMA_CONFIG_H_

/* maximum size of slots-frame */
#define TDMA_MAX_SLOTS 50

/* maximium number of frames */
#define MAC_TDMA_MAX_FRAMES 10

/* time between 2 beacons */
#define TDMA_BEACON_PERIOD_S 2

/* max number of slots-frame waited to request association*/
#define TDMA_ASSOC_BACKOFF_WINDOW 16

/* reserved number of data-frames that can be sent after each beacon */
#define TDMA_BEACON_BACKOFF_COUNT 1

/* slot time unit */
#define TDMA_SLOT_DURATION_FACTOR_US 100u

/* delay when starting slots-frame */
#define TDMA_STARTUP_DELAY_MS 100u

/* software margin for each slot */
#define TDMA_SLOT_WAKEUP_US 800u

/* half-windows size for listening during rx slot */
#define TDMA_SLOT_RX_MARGIN_US 500u

#endif /* MAC_TDMA_CONFIG_H_ */
