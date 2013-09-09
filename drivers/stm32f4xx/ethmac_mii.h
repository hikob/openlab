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
 * ethmac_mii.h
 *
 *  Created on: Sep 20, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef ETHMAC_MII_H_
#define ETHMAC_MII_H_

enum
{
    MII_BMCR = 0x00 /**< Basic mode control register.   */,
    MII_BMSR = 0x01 /**< Basic mode status register.    */,
    MII_PHYSID1 = 0x02 /**< PHYS ID 1.                     */,
    MII_PHYSID2 = 0x03 /**< PHYS ID 2.                     */,
    MII_ADVERTISE = 0x04 /**< Advertisement control reg.     */,
    MII_LPA = 0x05 /**< Link partner ability reg.      */,
    MII_EXPANSION = 0x06 /**< Expansion register.            */,
    MII_ANNPTR = 0x07 /**< 1000BASE-T control.            */,
    MII_CTRL1000 = 0x09 /**< 1000BASE-T control.            */,
    MII_STAT1000 = 0x0a /**< 1000BASE-T status.             */,
    MII_ESTATUS = 0x0f /**< Extended Status.               */,
    MII_PHYSTS = 0x10 /**< PHY Status register.           */,
    MII_MICR = 0x11 /**< MII Interrupt ctrl register.   */,
    MII_DCOUNTER = 0x12 /**< Disconnect counter.            */,
    MII_FCSCOUNTER = 0x13 /**< False carrier counter.         */,
    MII_NWAYTEST = 0x14 /**< N-way auto-neg test reg.       */,
    MII_RERRCOUNTER = 0x15 /**< Receive error counter.         */,
    MII_SREVISION = 0x16 /**< Silicon revision.              */,
    MII_RESV1 = 0x17 /**< Reserved.                      */,
    MII_LBRERROR = 0x18 /**< Lpback, rx, bypass error.      */,
    MII_PHYADDR = 0x19 /**< PHY address.                   */,
    MII_RESV2 = 0x1a /**< Reserved.                      */,
    MII_TPISTATUS = 0x1b /**< TPI status for 10Mbps.         */,
    MII_NCONFIG = 0x1c /**< Network interface config.      */,
};

/*
 * Basic mode control register.
 */
enum
{
    MII_BMCR__RESV = 0x007f /**< Unused.                        */,
    MII_BMCR__CTST = 0x0080 /**< Collision test.                */,
    MII_BMCR__FULLDPLX = 0x0100 /**< Full duplex.                   */,
    MII_BMCR__ANRESTART = 0x0200 /**< Auto negotiation restart.      */,
    MII_BMCR__ISOLATE = 0x0400 /**< Disconnect DP83840 from MII.   */,
    MII_BMCR__PDOWN = 0x0800 /**< Powerdown.                     */,
    MII_BMCR__ANENABLE = 0x1000 /**< Enable auto negotiation.       */,
    MII_BMCR__SPEED100 = 0x2000 /**< Select 100Mbps.                */,
    MII_BMCR__LOOPBACK = 0x4000 /**< TXD loopback bit.              */,
    MII_BMCR__RESET = 0x8000 /**< Reset.                         */,
};

/*
 * Basic mode status register.
 */
enum
{
    MII_BMSR__ERCAP = 0x0001 /**< Ext-reg capability.            */,
    MII_BMSR__JCD = 0x0002 /**< Jabber detected.               */,
    MII_BMSR__LSTATUS = 0x0004 /**< Link status.                   */,
    MII_BMSR__ANEGCAPABLE = 0x0008 /**< Able to do auto-negotiation.   */,
    MII_BMSR__RFAULT = 0x0010 /**< Remote fault detected.         */,
    MII_BMSR__ANEGCOMPLETE = 0x0020 /**< Auto-negotiation complete.     */,
    MII_BMSR__MFPRESUPPCAP = 0x0040 /**< Able to suppress preamble.     */,
    MII_BMSR__RESV = 0x0780 /**< Unused.                        */,
    MII_BMSR__10HALF = 0x0800 /**< Can do 10mbps, half-duplex.    */,
    MII_BMSR__10FULL = 0x1000 /**< Can do 10mbps, full-duplex.    */,
    MII_BMSR__100HALF = 0x2000 /**< Can do 100mbps, half-duplex.   */,
    MII_BMSR__100FULL = 0x4000 /**< Can do 100mbps, full-duplex.   */,
    MII_BMSR__100BASE4 = 0x8000 /**< Can do 100mbps, 4k packets.    */,
};

/*
 * Link partner ability register.
 */
enum
{
    MII_LPA__SLCT = 0x001f /**< Same as advertise selector.    */,
    MII_LPA__10HALF = 0x0020 /**< Can do 10mbps half-duplex.     */,
    MII_LPA__10FULL = 0x0040 /**< Can do 10mbps full-duplex.     */,
    MII_LPA__100HALF = 0x0080 /**< Can do 100mbps half-duplex.    */,
    MII_LPA__100FULL = 0x0100 /**< Can do 100mbps full-duplex.    */,
    MII_LPA__100BASE4 = 0x0200 /**< Can do 100mbps 4k packets.     */,
    MII_LPA__PAUSE_CAP = 0x0400 /**< Can pause.                     */,
    MII_LPA__PAUSE_ASYM = 0x0800 /**< Can pause asymetrically.       */,
    MII_LPA__RESV = 0x1000 /**< Unused.                        */,
    MII_LPA__RFAULT = 0x2000 /**< Link partner faulted.          */,
    MII_LPA__LPACK = 0x4000 /**< Link partner acked us.         */,
    MII_LPA__NPAGE = 0x8000 /**< Next page bit.                 */,

    MII_LPA__DUPLEX = (MII_LPA__10FULL | MII_LPA__100FULL),
    MII_LPA__100 = (MII_LPA__100FULL | MII_LPA__100HALF | MII_LPA__100BASE4),
};

#endif /* ETHMAC_MII_H_ */
