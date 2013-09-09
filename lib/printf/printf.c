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
 * \file printf.c
 * \date Jul 8, 2011
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include "printf_.h"

// External function that prints a character on the "standard output"
extern void xputc(char c);

// Wrapper that make xputc's interface compatible with the char_writer_t definition
static void xputc_wrap(char c, printf_param_t *p)
{
    if(c != 0)
    {
        xputc(c);
        p->cpt++;
    }
}

// Function that prints a character in a string
static void in_string(char c, printf_param_t *p)
{
    if(p->cpt < p->max - 1)
    {
        *(p->str) = c;
        (p->str)++;
    }
    else if(p->cpt == p->max - 1)
    {
        *(p->str) = 0;
    }

    if(c != 0)
    {
        p->cpt++;
    }
}

static void printi(printf_param_t *p, uint32_t i, uint8_t b, bool sg, uint16_t width, bool pad_zero)
{
    static const char digits[16] = "0123456789ABCDEF";
    char print_buf[12];
    char *s;
    int t;

    if (i == 0)
    {
        prints(p, "0", width, pad_zero);
    }
    else
    {
        if (sg && ((int32_t)i) < 0)
        {
            i = -i;
        }
        else
        {
            sg = false;
        }

        s = print_buf + sizeof(print_buf) / sizeof(char) - 1;
        *s = '\0';

        // Do a conversion and start filling the print buffer by the end
        while (i)
        {
            t = i % b;
            i /= b;

            s--;
            *s = digits[t];
        }

        // Print the sign if needed
        if (sg)
        {
            if (width && pad_zero)
            {
                p->out('-', p);
                width--;
            }
            else
            {
                s--;
                *s = '-';
            }
        }

        prints(p, s, width, pad_zero);
    }
}

static void print(printf_param_t *p, const char *format, va_list args)
{
    uint16_t width;
    bool pad_zero;
    char *s;
    char scr[2];

    for (; *format != 0; format++)
    {
        if (*format == '%')
        {
            format++;
            width = 0;
            pad_zero = false;

            if (*format == '\0')
            {
                break;
            }

            while (*format == '0')
            {
                format++;
                pad_zero = true;
            }

            while (*format >= '0' && *format <= '9')
            {
                width = 10 * width + *format - '0';
                format++;
            }

            switch (*format)
            {
                case 's':
                    s = va_arg(args, char *);
                    prints(p, s ? s : "(null)", width, pad_zero);
                    continue;

                case 'd':
                    printi(p, va_arg(args, int), 10, true, width, pad_zero);
                    continue;

                case 'x':
                case 'X':
                case 'p':
                case 'P':
                    printi(p, va_arg(args, int), 16, false, width, pad_zero);
                    continue;

                case 'u':
                    printi(p, va_arg(args, int), 10, false, width, pad_zero);
                    continue;

                case 'f':
#ifdef USE_PRINTFLOAT
                    printfloat(p, va_arg(args, double));
#else
                    va_arg(args, double); // ignore this parameter as floats are not handled
                    prints(p, "(float)", width, pad_zero);
#endif
                    continue;

                case 'c':
                    // char are converted to int then pushed on the stack
                    scr[0] = (char)va_arg(args, int);
                    scr[1] = '\0';
                    prints(p, scr, width, pad_zero);
                    continue;
            }
        }

        p->out(*format, p);
    }

    p->out('\0', p);

    va_end(args);
}

int printf(const char *format, ...)
{
    printf_param_t p;
    va_list args;

    va_start(args, format);

    p.out = (char_writer_t)xputc_wrap;
    p.cpt = 0;
    p.max = (1 << 30) - 1;
    p.str = 0;

    print(&p, format, args);

    return p.cpt;
}

int sprintf(char *buf, const char *format, ...)
{
    printf_param_t p;
    va_list args;

    va_start(args, format);

    p.out = (char_writer_t)in_string;
    p.cpt = 0;
    p.max = (1 << 30) - 1;
    p.str = buf;

    print(&p, format, args);

    return p.cpt;
}

int snprintf(char *buf, unsigned int count, const char *format, ...)
{
    printf_param_t p;
    va_list args;

    va_start(args, format);

    p.out = (char_writer_t)in_string;
    p.cpt = 0;
    p.max = count;
    p.str = buf;

    print(&p, format, args);

    return p.cpt;
}
int fprintf(const struct fprintf_stream *stream, const char *format, ...)
{
    printf_param_t p;
    va_list args;

    va_start(args, format);

    p.out = (char_writer_t)stream->putc;
    p.cpt = 0;
    p.max = (1 << 30) - 1;
    p.str = 0;

    print(&p, format, args);

    return p.cpt;
}
