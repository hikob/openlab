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
set(PLATFORM_OOCD_ITF ${PROJECT_SOURCE_DIR}/platform/scripts/mysticjtag.cfg)

set(LINKSCRIPT ../scripts/stm32f103rey6.ld)

set(DRIVERS stm32f1xx)

set(MY_C_FLAGS "${MY_C_FLAGS} -DAGILE_FOX")
set(PLATFORM_TYPE    SERIAL_TYPE_AGILEFOX)
set(PLATFORM_VERSION 0x13)

set(PLATFORM_RAM_KB 64)

# Set the flags to select the application that may be compiled
set(PLATFORM_HAS_BUTTON 1)
set(PLATFORM_HAS_L3G4200D 1)
set(PLATFORM_HAS_LSM303DLHC 1)
set(PLATFORM_HAS_LPS331AP 1)
set(PLATFORM_HAS_RF231 1)
set(PLATFORM_HAS_SD 1)
set(PLATFORM_HAS_PHY 1)
set(PLATFORM_HAS_SYSTICK 1)
set(PLATFORM_HAS_SOFTTIM 1)
set(PLATFORM_HAS_USB 1)
set(PLATFORM_HAS_TSCH 1)
set(PLATFORM_HAS_CSMA 1)
set(PLATFORM_HAS_TDMA 1)
set(PLATFORM_HAS_I2C_EXTERNAL 1)

#uncomment to add i2c slave support
#set(PLATFORM_HAS_I2C_SLAVE 1)
#set(MY_C_FLAGS "${MY_C_FLAGS} -DI2C__SLAVE_SUPPORT")

# Flags for daughter boards
set(PLATFORM_HAS_UBLOX 1)

if( ${EXTERNAL_SYNCHRO} )
set(PLATFORM_HAS_EXTSYNC 1)
endif( ${EXTERNAL_SYNCHRO} )

include(${PROJECT_SOURCE_DIR}/platform/include-cm3.cmake)
