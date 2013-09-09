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
 * \file scanf.c
 * \date Jan 28, 2013
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "debug.h"

inline static bool isspace(char c)
{
    return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r') || (c == '\f') || (c == '\v');
}

inline static void skip_spaces(const char **str)
{
    while(isspace(**str))
    {
        (*str)++;
    }
}

static int scans(const char **str, char *s)
{
    int l = 0;

    skip_spaces(str);

    while(true)
    {
        if(isspace(**str) || (**str == '\0'))
        {
            *s = '\0';
            return (l != 0);
        }
        else
        {
            *s = **str;
            s++;
        }

        (*str)++;
        l++;
    }

    return 0;
}

static int scani(const char **str, int *i, bool hex, bool sign)
{
    int l = 0;
    bool neg = false;

    *i = 0;

    skip_spaces(str);

    while(true)
    {
        if((**str >= '0') && (**str <= '9'))
        {
            if(!hex)
            {
                *i = *i * 10 + (**str - '0');
            }
            else
            {
                *i = ((*i) << 4) + (**str - '0');
            }
        }
        else if(hex && (**str >= 'A') && (**str <= 'F'))
        {
            *i = ((*i) << 4) + (**str - 'A' + 10);
        }
        else if((l == 0) && sign && (**str == '-'))
        {
            neg = true;
        }
        else
        {
            if(neg)
            {
                *i = - *i;
            }

            return (l != 0);
        }

        (*str)++;
        l++;
    }

    return 0;
}

static int scanfl(const char **str, float *f)
{
    int l = 0, e = 0;
    bool negm = false, nege = false, frac = false, expo = false;
    float div = 1.0, m = 0.0;

    skip_spaces(str);

    *f = 3.14;

    while(true)
    {
        if((**str >= '0') && (**str <= '9'))
        {
            if(!expo)
            {
                m = m * 10.0 + (**str - '0');

                if(frac)
                {
                    div /= 10.0;
                }
            }
            else
            {
                e = e * 10 + (**str - '0');
            }
        }
        else if((**str == '.') && (!frac) && (!expo))
        {
            frac = true;
        }
        else if(((**str == 'E') || (**str == 'e')) && (!expo))
        {
            expo = true;
            l = -1;
        }
        else if((l == 0) && (**str == '-'))
        {
            if(!expo)
            {
                negm = true;
            }
            else
            {
                nege = true;
            }
        }
        else
        {
            if(negm)
            {
                m = -m;
            }

            if(nege)
            {
                e = -e;
            }

            *f = m * div * powf(10.0, e);

            return (l != 0);
        }

        (*str)++;
        l++;
    }

    return 0;
}

static int scanc(const char **str, char *c)
{
    skip_spaces(str);

    if(**str != '\0')
    {
        *c = **str;
        (*str)++;
        return 1;
    }
    else
    {
        return 0;
    }
}

int sscanf(const char *str, const char *format, ...)
{
    va_list args;
    const char *in = str;
    int r  = 0, v = 0;

    va_start(args, format);

    for (; *format != 0; format++)
    {
        if(*format == '%')
        {
            format++;

            v = 0;

            switch (*format)
            {
                case 'n':
                    {
                        int *n = va_arg(args, int*);
                        *n = (in - str);
                        v = 1;
                    }
                    break;

                case 's':
                    v = scans(&in, va_arg(args, char*));
                    break;

                case 'd':
                    v = scani(&in, va_arg(args, int*), false, true);
                    break;

                case 'x':
                case 'X':
                    v = scani(&in, va_arg(args, int*), true, false);
                    break;

                case 'u':
                    v = scani(&in, va_arg(args, int*), false, false);
                    break;

                case 'f':
                    v = scanfl(&in, va_arg(args, float*));
                    break;;

                case 'c':
                    v = scanc(&in, va_arg(args, char*));
                    break;
            }

            if(v)
            {
                r++;
            }
            else
            {
                break;
            }
        }
        else if(isspace(*format))
        {
            //a space match a sequence of space of any length (no space is OK)
            skip_spaces(&in);
        }
        else if(*in == *format)
        {
            in++;
        }
        else
        {
            log_warning("\"%c\" != \"%c\"", *in, *format);
            break;
        }
    }

    va_end(args);

    return r;
}
