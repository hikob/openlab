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
 * constants.h
 *
 *  Created on: Aug 12, 2013
 *      Author: burindes
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

// Command Frames
enum {

    OPENNODE_START = 0x15,
    OPENNODE_STARTBATTERY = 0x16,
    OPENNODE_STOP = 0x17,

    BATTERY_CHARGE = 0x18,
    BATTERY_DISCHARGE = 0x19,

    CONFIG_POWERPOLL = 0x42,

    SET_TIME = 0x52,

    RADIO_OFF = 0x60,
    RADIO_SNIFFER = 0x61,
    RADIO_POLLING = 0x62,
    RADIO_INJECTION = 0x63,
    RADIO_JAMMING = 0x64,
};

// Notification Frames
enum
{
    RADIO_NOTIF_SNIFFED= 0xA1,
    RADIO_NOTIF_POLLING = 0xA2,

    POWERPOLL_NOTIF = 0xB1,
};


#endif /* CONSTANTS_H_ */
