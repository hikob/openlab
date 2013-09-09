#
# This file is part of HiKoB Openlab. 
# 
# HiKoB Openlab is free software: you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# as published by the Free Software Foundation, version 3.
# 
# HiKoB Openlab is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with HiKoB Openlab. If not, see
# <http://www.gnu.org/licenses/>.
#
# Copyright (C) 2011 HiKoB.
#

set(OOCD_TARGET stm32f4x_stlink)
set(PLATFORM_OOCD_ITF interface/stlink-v2.cfg)

set(LINKSCRIPT ../scripts/stm32f407.ld)

set(DRIVERS stm32f4xx)

set(PLATFORM_RAM_KB 128)

# Set the flags to select the application that may be compiled
set(PLATFORM_HAS_SYSTICK 1)
set(PLATFORM_HAS_SOFTTIM 1)
set(PLATFORM_HAS_ETHERNET 1)

include(${PROJECT_SOURCE_DIR}/platform/include-cm4.cmake)
