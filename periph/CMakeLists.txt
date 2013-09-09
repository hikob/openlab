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

# Add the RF2xx library
add_library(rf2xx STATIC rf2xx/rf2xx)

# Add the LSM303DLHC library
add_library(lsm303dlhc STATIC lsm303dlhc/lsm303dlhc)

# Add the L3G4200D library
add_library(l3g4200d STATIC l3g4200d/l3g4200d)

# Add the LPS331AP library
add_library(lps331ap STATIC lps331ap/lps331ap)

# Add the BATFB library
add_library(batfb STATIC batfb/batfb)
add_library(batfb_itf STATIC batfb/batfb_itf)

# Add the PGA308 library
add_library(pga308 STATIC pga308/pga308)

# Add the ADG759 library
add_library(adg759 STATIC adg759/adg759)

# Add the PCA9685 LED driver
if( ${PLATFORM_HAS_PCA9685} )
add_library(pca9685 STATIC pca9685/pca9685)
endif( ${PLATFORM_HAS_PCA9685} )

# Add the N25xxx library
add_library(n25xxx STATIC n25xxx/n25xxx)
target_link_libraries(n25xxx platform)

# Add the INA226 library
add_library(ina226 STATIC ina226/ina226)

# Add the U-Blox MAX6 GPS
if( ${PLATFORM_HAS_UBLOX} )
add_library(ublox_max6 STATIC ublox_max6/max6x ublox_max6/max6x_utils)
endif( ${PLATFORM_HAS_UBLOX} )

# Add the ISL29020 library
add_library(isl29020 STATIC isl29020/isl29020)
