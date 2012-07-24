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
 * adg759.h
 *
 *  Created on: Dec 21, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef ADG759_H_
#define ADG759_H_

typedef enum
{
    ADG759_INPUT_1 = 1,
    ADG759_INPUT_2 = 2,
    ADG759_INPUT_3 = 3,
    ADG759_INPUT_4 = 4,
} adg759_input_t;

/**
 * Enable the multiplexer.
 */
void adg759_enable();
/**
 * Disable the multiplexer.
 */
void adg759_disable();
/**
 * Select a given input of the multiplexer.
 *
 * \param input the input to select
 */
void adg759_select(adg759_input_t input);

#endif /* ADG759_H_ */
