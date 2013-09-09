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
 * \file ansi.h
 * \date Oct 04, 2012
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef ANSI_H_
#define ANSI_H_

/**
 * \addtogroup lib
 * @{
 */

/**
 * \defgroup ansi Enhanced ANSI text formatting 
 *
 *
 * @{
 */

typedef enum
{
    ANSI_RED     = 1,
    ANSI_GREEN   = 2,
    ANSI_YELLOW  = 3,
    ANSI_BLUE    = 4,
    ANSI_MAGENTA = 5,
    ANSI_CYAN    = 6,
    ANSI_WHITE   = 7
} ansi_color_t;

/** Clears the screen
 */
static inline void ansi_clear_screen()
{
    printf("\x1b[2J");
}

/** Go to a given coordinate
 * This function puts the cursor to the given (x, y) coordinate on the screen.
 * The coordinates are expressed in a "character unit" (i.e. not pixel), meaning
 * that the coordinate (3, 7) puts the cursor on the 3rd character from the left
 * and 7th from the top of the screen
 * \param x The X coordinate of the cursor
 * \param y The Y coordiante of the cursor
 */
static inline void ansi_goto(uint8_t x, uint8_t y)
{
    printf("\x1b[%u;%uH", y, x);
}

/** Set the text color 
 * Set the text color. Once this function is called, every text written after the call will
 * be displayed with the given color.
 * \param c The color (\see \ref ansi_color_t) 
 */
static inline void ansi_set_text_color(ansi_color_t c)
{
    printf("\x1b[%02um", 30 + c);
}

/** Set the background color 
 * Set the background color. Once this function is called, every text written after the call will
 * be displayed with the given background color.
 * \param c The color (\see \ref ansi_color_t) 
 */
static inline void ansi_set_background_color(ansi_color_t c)
{
    printf("\x1b[%02um", 40 + c);
}

/**
 * @}
 */

/**
 * @}
 */

#endif
