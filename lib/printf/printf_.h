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
 * \file printf_.h
 * \date Oct 4, 2012
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef PRINTF__H_
#define PRINTF__H_

#include "printf.h"

#define USE_PRINTFLOAT

// Prototype of function to write characters
typedef void (*char_writer_t)(char, void*);

// Parameters to pass from function to function
typedef struct
{
    char_writer_t out;
    char *str;
    int cpt;
    int max;
} printf_param_t;

void prints(printf_param_t *p, const char *string, uint16_t width, bool pad_zero);
void printfloat(printf_param_t *p, float f);

#endif // PRINTF__H_
