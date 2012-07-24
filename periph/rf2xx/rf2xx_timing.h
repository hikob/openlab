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
 * rf2xx_timing.h
 *
 *  Created on: Aug 23, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef RF2XX_TIMING_H_
#define RF2XX_TIMING_H_

enum
{
    RF2XX_TIMING__VCC_TO_P_ON = 330,
    RF2XX_TIMING__SLEEP_TO_TRX_OFF = 380,
    RF2XX_TIMING__TRX_OFF_TO_PLL_ON = 110,
    RF2XX_TIMING__TRX_OFF_TO_RX_ON = 110,
    RF2XX_TIMING__PLL_ON_TO_BUSY_TX = 16,

    RF2XX_TIMING__RESET = 100,
    RF2XX_TIMING__RESET_TO_TRX_OFF = 37,
};

#endif /* RF2XX_TIMING_H_ */
