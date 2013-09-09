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
 * Copyright (C) 2011 HiKoB.
 */

/**
 * \file platform_leds.h
 *
 *  \date Jul 6, 2011
 *  \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 *  \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef PLATFORM_LEDS_H_
#define PLATFORM_LEDS_H_

/**
 * Enumeration of the available LEDs.
 *
 * They may be used as a bitmask for the LEDs control functions.
 */
enum
{
    LED_0 = 0x1,
    LED_1 = 0x2,
    LED_2 = 0x4,
};


#if !defined(PLATFORM_OS) || (PLATFORM_OS != CONTIKI)

/**
 * Turn on some LEDs.
 *
 * \param leds a bitmask selecting the LEDs
 */
void leds_on(uint8_t leds);
/**
 * Turn off some LEDs.
 *
 * \param leds a bitmask selecting the LEDs
 */
void leds_off(uint8_t leds);
/**
 * Toggle on some LEDs.
 *
 * \param leds a bitmask selecting the LEDs
 */
void leds_toggle(uint8_t leds);

#endif /* PLATFORM_OS != CONTIKI */
#endif /* PLATFORM_LEDS_H_ */
