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

/**
 * A header class defining the handler function type and handler function
 * argument type;
 *
 *  Created on: Jul 8, 2011
 *      Author: Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef HANDLER_H_
#define HANDLER_H_

#include <stddef.h>

/**
 * A handler argument is in fact an abstract pointer.
 */
typedef void *handler_arg_t;

/**
 * A handler function is a function taking a handler argument in
 * parameter and returning void.
 */
typedef void (*handler_t)(handler_arg_t arg);

#endif /* HANDLER_H_ */
