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
 * boot.h
 *
 *  Created on: Jul 6, 2011
 *      Author: Christophe Braillon <christophe.braillon.at.hikob.com>
 */

#ifndef BOOT_H_
#define BOOT_H_

#if defined(RELEASE) && RELEASE==1
#define DEBUG_HANDLER(isr)\
    void __attribute__((weak)) isr();
#else // defined(RELEASE) && RELEASE==1
#define DEBUG_HANDLER(isr)\
    void __attribute__((weak)) isr()\
    {\
        debug_handler(__FUNCTION__);\
    }
#endif // defined(RELEASE) && RELEASE==1
typedef void (*vector_t)();

void debug_handler(const char *name);

#endif /* BOOT_H_ */
