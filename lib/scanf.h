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

/**
 * \file scanf.h
 * \date Jan 28, 2013
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef SCANF_H_
#define SCANF_H_

/**
 * \addtogroup lib
 * @{
 */

/**
 * \defgroup scanf Light-weight scanf
 *
 *
 * @{
 */

#include <stdarg.h>

/** Read input string according to format
 * This light-weight version of sscanf allows only a subset of formats. The allowed formats are:
 *   - %u: read an unsigned int
 *   - %d: read a signed int
 *   - %x and %X: read an int in hexadecimal form
 *   - %c: read a single character
 *   - %s: read a string
 *   - %f: read a float, NaN and Inf values are not supported
 * \param format The format string consists of a sequence of directives which describe how to process the
 *               sequence of input characters. If processing of a directive fails, no further input is
 *               read, and sscanf() returns. A "failure" can be either of the following: input failure,
 *               meaning that input characters were unavailable, or matching failure, meaning that the
 *               input was inappropriate.
 * \param ... the list of variables to be read. The number of variable must be consistent with the format string otherwise the result is not specified.
 * \return the number of input items successfully matched and assigned, which can be fewer than provided for, or even zero in the event of an early matching failure.
 * \see \ref example_scanf.c
 */
int sscanf(const char *str, const char *format, ...);

/**
 * @}
 */

/**
 * @}
 */

#endif // SCANF_H_
