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
 * \file debug.h
 * \date Oct 04, 2012
 * \author Christophe Braillon <christophe.braillon.at.hikob.com>
 * \author Clément Burin des Roziers <clement.burin-des-roziers.at.hikob.com>
 */

#ifndef LOG_H_
#define LOG_H_

#include "nvic.h"
#include "event.h"
#include "soft_timer.h"
#include "printf.h"

#ifndef RELEASE
#define RELEASE 0
#endif // RELEASE

#if RELEASE > 0
// Undefine the log level to set it at very high level
#ifdef LOG_LEVEL
#undef LOG_LEVEL
#endif // LOG_LEVEL

#define LOG_LEVEL 4
#endif // RELEASE > 0

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR 3
#define LOG_LEVEL_DISABLED 4

#ifndef NO_DEBUG_HEADER
#ifndef NO_DEBUG_COLOR
#define DEBUG_HEADER()             printf("\x1b[32m[in %s() DEBUG] ", __FUNCTION__)
#define INFO_HEADER()              printf("\x1b[33m[in %s() INFO] ", __FUNCTION__)
#define WARNING_HEADER()           printf("\x1b[35m[in %s() WARNING] ", __FUNCTION__)
#define ERROR_HEADER()             printf("\x1b[31m[in %s() ERROR] ", __FUNCTION__)
#define NOT_IMPLEMENTED_HEADER()   printf("\x1b[36m[in %s() NOT IMPLEMENTED] ", __FUNCTION__)
#else
#define DEBUG_HEADER()             printf("[in %s() DEBUG] ", __FUNCTION__)
#define INFO_HEADER()              printf("[in %s() INFO] ", __FUNCTION__)
#define WARNING_HEADER()           printf("[in %s() WARNING] ", __FUNCTION__)
#define ERROR_HEADER()             printf("[in %s() ERROR] ", __FUNCTION__)
#define NOT_IMPLEMENTED_HEADER()   printf("[in %s() NOT IMPLEMENTED] ", __FUNCTION__)
#endif
#else
#ifndef NO_DEBUG_COLOR
#define DEBUG_HEADER()             printf("\x1b[32m")
#define INFO_HEADER()              printf("\x1b[33m")
#define WARNING_HEADER()           printf("\x1b[35m")
#define ERROR_HEADER()             printf("\x1b[31m")
#define NOT_IMPLEMENTED_HEADER()   printf("\x1b[36m")
#else
#define DEBUG_HEADER()
#define INFO_HEADER()
#define WARNING_HEADER()
#define ERROR_HEADER()
#define NOT_IMPLEMENTED_HEADER()
#endif
#endif

#ifndef NO_DEBUG_COLOR
#define DEBUG_ENDL()               printf("\x1b[0m\n")
#else
#define DEBUG_ENDL()               printf("\n")
#endif

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG
#endif // LOG_LEVEL
#if (LOG_LEVEL <= LOG_LEVEL_DEBUG)
#define log_debug(...) do {DEBUG_HEADER(); printf(__VA_ARGS__);DEBUG_ENDL();}while(0)
#else // (LOG_LEVEL <= LOG_LEVEL_DEBUG)
#define log_debug(...)
#endif // (LOG_LEVEL <= LOG_LEVEL_DEBUG)
#if (LOG_LEVEL <= LOG_LEVEL_INFO)
#define log_info(...) do {INFO_HEADER(); printf(__VA_ARGS__);DEBUG_ENDL();}while(0)
#else // (LOG_LEVEL <= LOG_LEVEL_INFO)
#define log_info(...)
#endif // (LOG_LEVEL <= LOG_LEVEL_INFO)
#if (LOG_LEVEL <= LOG_LEVEL_WARNING)
#define log_warning(...) do {WARNING_HEADER(); printf(__VA_ARGS__);DEBUG_ENDL();}while(0)
#else // (LOG_LEVEL <= LOG_LEVEL_INFO)
#define log_warning(...)
#endif // (LOG_LEVEL <= LOG_LEVEL_INFO)
#if (LOG_LEVEL <= LOG_LEVEL_ERROR)
#define log_error(...) do {ERROR_HEADER(); printf(__VA_ARGS__);DEBUG_ENDL();}while(0)
#define log_not_implemented(...) do {NOT_IMPLEMENTED_HEADER(); printf(__VA_ARGS__);DEBUG_ENDL();}while(0)
#else // (LOG_LEVEL <= LOG_LEVEL_ERROR)
#define log_error(...)
#define log_not_implemented(...)
#endif // (LOG_LEVEL <= LOG_LEVEL_ERROR)

#if RELEASE == 0
#define log_printf(...) printf(__VA_ARGS__)
#else
#define log_printf(...)
#endif

#if defined(NATIVE)
inline static void HALT()
{
    while (1)
    {
    }
}
#else // defined(NATIVE)
static volatile int block_me;
static void inline HALT()
{
#if RELEASE || (defined(AUTO_RESET) && AUTO_RESET)
    // Reset the chip through the NVIC
    NVIC_RESET();
#else // RELEASE > 0
    // Deactivate interrupts
    asm volatile("cpsid i");

    event_debug();
    soft_timer_debug();

    block_me = 1;

    while (block_me)
    {
        asm volatile("bkpt 0");
    }

    // Re activate
    asm volatile("cpsie i");
#endif // RELEASE > 0
}
#endif // defined(NATIVE)

#endif
