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

set(OOCD_TARGET stm32f1x)
set(PLATFORM_OOCD_ITF ${PROJECT_SOURCE_DIR}/platform/scripts/dangerousnewt.cfg)

set(LINKSCRIPT ../scripts/stm32f103rey6.ld)

set(DRIVERS stm32f1xx)

set(PLATFORM_RAM_KB 64)

set(MY_C_FLAGS "${MY_C_FLAGS} -DDANGEROUS_NEWT")
set(PLATFORM_TYPE    0x08)
set(PLATFORM_VERSION 0x01)

# Set the flags to select the application that may be compiled
set(PLATFORM_HAS_RF231 1)
set(PLATFORM_HAS_SYSTICK 1)
set(PLATFORM_HAS_PHY 1)
set(PLATFORM_HAS_SOFTTIM 1)

set(PLATFORM_HAS_LSM303DLHC 1)
set(PLATFORM_HAS_PCA9685 1)
set(PLATFORM_HAS_PGA308 1)

include(${PROJECT_SOURCE_DIR}/platform/include-cm3.cmake)
