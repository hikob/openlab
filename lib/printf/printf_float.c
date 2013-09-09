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
 * \file printf_float.c
 * \date Oct 4, 2012
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include "printf_.h"

typedef uint32_t uint160_t[5];

/**
 * Shift a 32-bit integer by i bits and store the result in a 160-bit integer
 *
 * @param n the integer to be shifted
 * @param i the number of bits to shift
 * @param r the result of the shift operation
 */
static void shift(uint32_t n, uint16_t i, uint160_t r)
{
    if (i < 32)
    {
        r[0] = n << i;
        r[1] = n >> (32 - i);
        r[2] = r[3] = r[4] = 0;
    }
    else if (i < 64)
    {
        r[0] = r[3] = r[4] = 0;
        r[1] = n << (i - 32);
        r[2] = n >> (64 - i);
    }
    else if (i < 96)
    {
        r[0] = r[1] = r[4] = 0;
        r[2] = n << (i - 64);
        r[3] = n >> (96 - i);
    }
    else if (i < 128)
    {
        r[0] = r[1] = r[2] = 0;
        r[3] = n << (i - 96);
        r[4] = n >> (128 - i);
    }
    else
    {
        r[0] = r[1] = r[2] = r[3] = 0;
        r[4] = n << (i - 128);
    }
}

/**
 * Multiply by 2 a 160-bit integer (shift 1 bit to the left)
 *
 * @param r the integer to shift
 */
static inline void mul2(uint160_t r)
{
    int16_t i, c = 0, t;

    for (i = 0; i < 5; i++)
    {
        /* See if the last bit of the i-th cluster if bits is 1
         * If it is one, there is a carry to save for the next cluster
         */
        t = r[i] >> 31;
        // Shift the cluster and add the carry
        r[i] = (r[i] << 1) | c;
        // Update the carry for the next cluster
        c = t;
    }
}

/**
 * Multiply by 10 a 160-bit integer
 *
 * @param r the integer to multiply
 */
static inline void mul10(uint160_t r)
{
    int16_t i, c = 0, t;

    for (i = 0; i < 5; i++)
    {
        if (r[i] > 0xFFFFFFFF / 10)
        {
            t = ((r[i] >> 4) * 10) >> 28;
        }
        else
        {
            t = 0;
        }

        r[i] = r[i] * 10 + c;
        c = t;
    }
}

/**
 * @param r
 * @param s
 * @return 1 if r > s, 0 if r == s and -1 if r < s
 */
static inline int16_t cmp(uint160_t r, uint160_t s)
{
    int16_t i;

    for (i = 4; i >= 0; i--)
    {
        if (r[i] > s[i])
        {
            return 1;
        }
        else if (r[i] < s[i])
        {
            return -1;
        }
    }

    return 0;
}

static inline void sub(uint160_t r, uint160_t s)
{
    int16_t i, c = 0;

    for (i = 0; i < 5; i++)
    {
        if ((r[i] >= c) && ((r[i] - c) >= s[i]))
        {
            r[i] -= s[i] + c;
            c = 0;
        }
        else
        {
            r[i] = (0xFFFFFFFF - s[i] - c) + r[i];
            c = 1;
        }
    }
}

static inline void euclid10(uint160_t r, uint160_t s, char *u)
{
    for (*u = 0; *u < 10; (*u)++)
    {
        if (cmp(r, s) >= 0)
        {
            sub(r, s);
        }
        else
        {
            break;
        }
    }

    *u += '0';
}

static inline void from32(uint32_t n, uint160_t r)
{
    r[0] = n;
    r[1] = r[2] = r[3] = r[4] = 0;
}

static inline void copy(uint160_t src, uint160_t dst)
{
    uint32_t i;

    for (i = 0; i < 5; i++)
    {
        dst[i] = src[i];
    }
}

static inline bool cond1(uint160_t r, uint160_t s)
{
    // r < s / 10
    uint160_t t;

    copy(r, t);
    mul10(t);

    // s > 10 * r
    return cmp(s, t) == 1;
}

static inline int16_t cond2(uint160_t r, uint160_t s, uint160_t mp)
{
    // 2 * r + mp >= 2 * s
    uint160_t t;

    if (cmp(r, s) >= 0)
    {
        // r >= s => 2 * r + mp >= 2 * s as mp >= 0
        return 1;
    }
    else
    {
        // r < s
        copy(s, t);
        sub(t, r);
        mul2(t);

        // mp >= 2 * (s - r)
        return cmp(mp, t);
    }
}

/** Condition 3 for Dragon4 algorithm
 * This function evaluate if the remainder is more than twice the scale
 * \param r remainder
 * \param s scale
 * \return 1 if 2 * r < mm
 */
static inline bool cond3(uint160_t r, uint160_t mm)
{
    uint160_t t;

    copy(r, t);
    mul2(t);

    // mm > 2 * r
    return cmp(mm, t) == 1;
}

/** Condition 4 for Dragon4 algorithm
 * This function evaluate if the remainder is more than twice the scale
 * \param r remainder
 * \param s scale
 * \return 1 if 2 * r > s
 */
static inline bool cond4(uint160_t r, uint160_t s)
{
    uint160_t t;

    copy(r, t);
    mul2(t);

    return cmp(s, t) < 0;
}

/** Dragon4 algorithm from Steele & White paper
 * This function is adapted from Guy L. Steele Jr. and Jon L. White article
 * "How to Print Floating-Point Numbers Accurately" ACM SIGPLAN'90
 * Conference on Programming Language Design and Implementation.
 *
 * The value to be printed is v = f * 2 ^ e
 *
 * \param f mantissa (significand)
 * \param e exponent
 * \param buf output buffer
 */
int16_t dragon4(uint32_t f, int16_t e, char *buf)
{
    uint160_t r /* Remainder */, s /* Shift */, mm /* Gap between v and its predecessor */, mp /* Gap between v and its successor */;
    int16_t expt = 0; /* The exponent to be returned */
    bool low = false, high = false;

    if (e > 0)
    {
        shift(f, e, r);  // r = f * 2 ^ e
        from32(1, s);    // s = 1
        shift(1, e, mm); // mm = 2 ^ e
    }
    else
    {
        from32(f, r);    // r = f
        shift(1, -e, s); // s = 2 ^ (-e)
        from32(1, mm);   // mm = 1
    }

    copy(mm, mp);

    /*** Compute exponent ***/
    while (cond1(r, s)) // r < s / 10
    {
        expt--;
        mul10(r);
        mul10(mm);
        mul10(mp);
    }

    while (cond2(r, s, mp) >= 0) // 2 * r + mp >= 2 * s
    {
        mul10(s);
        expt++;
    }

    expt--;
    /*** End of exponent computation ***/

    // Do a traditional decomposition by successive euclidian divisions
    while (!low && !high)
    {
        mul10(r);
        mul10(mm);
        mul10(mp);
        euclid10(r, s, buf);
        buf++;

        // Compute the two escape conditions
        low = cond3(r, mm);            // 2 * r < mm
        high = (cond2(r, s, mp) == 1); // 2 * r > 2 * s - mp
    }

    // Adjust the last decimal to round more accurately
    if (high && !low)
    {
        buf[-1]++;
    }
    else if ((low && high) || (!low && !high))
    {
        if (cond4(r, s)) // 2 * r > s
        {
            buf[-1]++;
        }
    }

    *buf = 0;

    return expt;
}

/**
 *
 * The value to be printed is v = f * 2 ^ e
 *
 * \param f mantissa
 * \param e exponent
 */
void free_format_exponential(printf_param_t *p, uint32_t f, int16_t e)
{
    char buf[32];
    uint16_t k = 1;
    int16_t expt;

    expt = dragon4(f, e, buf);

    p->out(buf[0], p);

    if (buf[k] != 0)
    {
        p->out('.', p);

        while (buf[k] != 0)
        {
            p->out(buf[k], p);
            k++;
        }
    }

    // Print out the exponent if it is not null
    if (expt != 0)
    {
        p->out('E', p);

        // Print out the exponent sign
        if (expt < 0)
        {
            p->out('-', p);
            expt = -expt;
        }

        // Print out the exponent absolute value
        for (k = 10; k <= expt; k *= 10);

        while (k > 1)
        {
            k /= 10;
            p->out('0' + expt / k, p);
            expt %= 10;
        }
    }
}

void printfloat(printf_param_t *p, float f)
{
    uint32_t *fi = (uint32_t *)&f;
    uint8_t s = (*fi >> 31);
    uint16_t e = (*fi >> 23) & 0xFF;
    uint32_t m = *fi & 0x007FFFFF;

    if (e == 0)
    {
        // Zero of denormalized numbers
        // Do not handle particular case of denormalized numbers (m != 0)
        p->out('0', p);
    }
    else if (e == 0xFF)
    {
        // Infinities or "Not a Number"
        if (m == 0)
        {
            prints(p, s ? "-inf" : "inf", 0, 0);
        }
        else
        {
            prints(p, "NaN", 0, 0);
        }
    }
    else
    {
        // Print out the sign
        if (s)
        {
            p->out('-', p);
        }

        free_format_exponential(p, m | (1 << 23), e - 150);
    }
}
