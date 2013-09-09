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
 * \file printf.h
 * \date Jul 8, 2011
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef PRINTF_H_
#define PRINTF_H_

/**
 * \addtogroup lib
 * @{
 */

/**
 * \defgroup printf Light-weight printf
 *
 *
 * @{
 */

#include <stdarg.h>

#define USE_PRINTFLOAT

/** Format and print data
 * This light-weight version of printf allows only a subset of formats. The allowed formats are:
 *   - %u: display an unsigned int, padding with zeros or spaces is allowed (e.g. %05u or %2u)
 *   - %d: display a signed int, padding with zeros or spaces is allowed (e.g. %04d or %5d)
 *   - %x and %X: display an int in hexadecimal form padding with zeros or spaces is allowed (e.g. %4X or %06x)
 *   - %c: display a character
 *   - %s: display a string padding with spaces is allowed (e.g. %4s)
 *   - %f: display a float, no padding is allowed, NaN and Inf values are allowed
 * \param format the string to be printed with special control sequences that are listed above.
 * \param ... the list of variables to be displayed. The number of variable must be consistent with the format string
 * \return the number of written characters excluding the terminating zero
 * \see \ref example_printf.c
 */
int printf(const char *format, ...);

/** Format and print data in a string
 * This function act as printf but instead of writing to the "standard output", writes the
 * output to a buffer regardless of its size.
 * \warning The function does not check the size of the output buffer. For safer behaviour
 * snprintf should be prefered.
 * \param out the output buffer
 * \param format the string to be printed with special control sequences that are listed above.
 * \param ... the list of variables to be displayed. The number of variable must be consistent with the format string
 * \return the number of written characters excluding the terminating zero
 * \see printf() for allowed formats
 * \see \ref example_sprintf.c
 */
int sprintf(char *out, const char *format, ...);

/** Format and print data in a string with limited size
 * This function act as sprintf but do not write more than "count" bytes (including
 * the terminating null byte ('\0')). If the output was truncated due to this limit
 * then the return value is the number of characters (excluding the terminating null byte)
 * which would have been written to the final string if enough space had  been  available.
 * Thus, a return value of "count" or more means that the output was truncated.
 * \param out the output buffer
 * \param count the maximum number of character that can be written in the output buffer (including the terminating zero)
 * \param format the string to be printed with special control sequences that are listed above.
 * \param ... the list of variables to be displayed. The number of variable must be consistent with the format string
 * \return the number of characters in the output buffer if it had enough space (excluding the terminating zero)
 * \see printf() for allowed formats
 * \see \ref example_snprintf.c
 */
int snprintf(char *out, unsigned int count, const char *format, ...);

struct fprintf_stream
{
    /** Function called for each character to output */
    void (*putc)(char c, void*);
};

/** Format and print to a custom outout stream
 * \param stream the output stream
 * \param format the string to be printed with special control sequences that are listed above.
 * \param ... the list of variables to be displayed. The number of variable must be consistent with the format string
 * \return the number of characters in the output buffer if it had enough space (excluding the terminating zero)
 * \see printf() for allowed formats
 */
int fprintf(const struct fprintf_stream *stream, const char *format, ...);

/**
 * @}
 */

/**
 * @}
 */

#endif // PRINTF_H_
