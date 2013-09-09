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
 * exti_.h
 *
 *  Created on: Feb 24, 2012
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef EXTI__H_
#define EXTI__H_

#include "exti.h"

/** Call this function to handle an EXTI interrupt if the line is known*/
void exti_handle_interrupt(exti_line_t line);

/** Call this function to handle an EXTI interrupt if the line is in the 9-5 range */
void exti_handle_9_5_interrupt();

/** Call this function to handle an EXTI interrupt if the line is in the 15-10 range */
void exti_handle_15_10_interrupt();

#endif /* EXTI__H_ */
