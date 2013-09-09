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
 * ublox_class.h
 *
 *  Created on: Feb 27, 2012
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef UBLOX_CLASS_H_
#define UBLOX_CLASS_H_

/* UBX Class */
#define MAX6x_CLASS_NAV   0x01 // Navigation Results: Position, Speed, Time, Acc, Heading, DOP, SVs used
#define MAX6x_CLASS_RXM   0x02 // Receiver Manager Messages: Satellite Status, RTC Status
#define MAX6x_CLASS_INF   0x04 // Information Messages: Printf-Style Messages, with IDs such as Error, Warning, Notice
#define MAX6x_CLASS_ACK   0x05 // Ack/Nack Messages: as replies to CFG Input Messages
#define MAX6x_CLASS_CFG   0x06 // Configuration Input Messages: Set Dynamic Model, Set DOP Mask, Set Baud Rate, etc.
#define MAX6x_CLASS_MON   0x0A // Monitoring Messages: Comunication Status, CPU Load, Stack Usage, Task Status
#define MAX6x_CLASS_AID   0x0B // AssistNow Aiding Messages: Ephemeris, Almanac, other A-GPS data input
#define MAX6x_CLASS_TIM   0x0D // Timing Messages: Timepulse Output, Timemark Results
#define MAX6x_CLASS_ESF   0x10 // External Sensor Fusion Messages: External sensor measurements and status information

/* UBX Class ACK Ack/Nack Messages */
#define MAX6x_ID_ACK      0x01 // Answer Message Acknowledged
#define MAX6x_ID_NAK      0x00 // Answer Message Not-Acknowledged

/* UBX Class AID AssistNow Aiding Messages */
#define MAX6x_ID_ALM      0x30
#define MAX6x_ID_ALPSRV   0x32
#define MAX6x_ID_ALP      0x50
#define MAX6x_ID_DATA     0x10
#define MAX6x_ID_EPH      0x31
#define MAX6x_ID_HUI      0x02
#define MAX6x_ID_INI      0x01
#define MAX6x_ID_REQ      0x00

/* UBX Class CFG Configuration Input Messages */
#define MAX6x_ID_ANT      0x13
#define MAX6x_ID_CFG      0x09
#define MAX6x_ID_DAT      0x06
#define MAX6x_ID_FXN      0x0E
#define MAX6x_ID_INF      0x02
#define MAX6x_ID_MSG      0x01
#define MAX6x_ID_NAV5     0x24
#define MAX6x_ID_NAVX5    0x23
#define MAX6x_ID_NMEA     0x17
#define MAX6x_ID_PM       0x32
#define MAX6x_ID_PRT      0x00
#define MAX6x_ID_RATE     0x08
#define MAX6x_ID_RINV     0x34
#define MAX6x_ID_RST      0x04
#define MAX6x_ID_RXM      0x11
#define MAX6x_ID_CFG_SBAS 0x16
#define MAX6x_ID_TMODE    0x1D
#define MAX6x_ID_CFG_TP   0x07
#define MAX6x_ID_USB      0x1B

/* UBX Class INF Information Messages */
#define MAX6x_ID_DEBUG    0x04
#define MAX6x_ID_ERROR    0x00
#define MAX6x_ID_NOTICE   0x02
#define MAX6x_ID_TEST     0x03
#define MAX6x_ID_WARNING  0x01

/* UBX Class MON Monitoring Messages */
#define MAX6x_ID_HW2      0x0B
#define MAX6x_ID_HW       0x09
#define MAX6x_ID_IO       0x02
#define MAX6x_ID_MSGPP    0x06
#define MAX6x_ID_RXBUF    0x07
#define MAX6x_ID_RXR      0x21
#define MAX6x_ID_TXBUF    0x08
#define MAX6x_ID_VER      0x04

/* UBX Class NAV Navigation Results*/
#define MAX6x_ID_CLOCK    0x22
#define MAX6x_ID_DOP      0x04
#define MAX6x_ID_POSECEF  0x01
#define MAX6x_ID_POSLLH   0x02
#define MAX6x_ID_NAV_SBAS 0x32
#define MAX6x_ID_SOL      0x06
#define MAX6x_ID_STATUS   0x03
#define MAX6x_ID_SVINFO   0x30
#define MAX6x_ID_TIMEGPS  0x20
#define MAX6x_ID_TIMEUTC  0x21
#define MAX6x_ID_VELECEF  0x11
#define MAX6x_ID_VELNED   0x12

/* UBX Class RXM Receiver Manager Messages */
#define MAX6x_ID_PMREQ    0x41
#define MAX6x_ID_POSREQ   0x40
#define MAX6x_ID_RAW      0x10
#define MAX6x_ID_SFRB     0x11
#define MAX6x_ID_SVSI     0x20

/* UBX Class TIM Timing Messages */
#define MAX6x_ID_SVIN     0x04
#define MAX6x_ID_TM2      0x03
#define MAX6x_ID_TIM_TP   0x01

#endif
