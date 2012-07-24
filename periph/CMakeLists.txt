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

# Include the headers
include_directories(rf2xx l3g4200d lsm303dlhc)

# Add the RF2xx library
add_library(rf2xx STATIC rf2xx/rf2xx)
target_link_libraries(rf2xx platform)

# Add the LSM303DLHC library
add_library(lsm303dlhc STATIC lsm303dlhc/lsm303dlhc)
target_link_libraries(lsm303dlhc platform)

# Add the L3G4200D library
add_library(l3g4200d STATIC l3g4200d/l3g4200d)
target_link_libraries(l3g4200d platform)

# Add the LPS331 library
add_library(lps331 STATIC lps331/lps331)
target_link_libraries(lps331 platform)

# Add the BATFB library
add_library(batfb STATIC batfb/batfb)
target_link_libraries(batfb platform)

# Add the PGA308 library
add_library(pga308 STATIC pga308/pga308)
target_link_libraries(pga308 platform)

# Add the ADG759 library
add_library(adg759 STATIC adg759/adg759)
target_link_libraries(adg759 platform)

# Add the TriLED library
add_library(triled STATIC triled/triled)
target_link_libraries(triled platform)

# Add the PCA9685 LED driver
if( ${PLATFORM_HAS_PCA9685} )
add_library(pca9685 STATIC pca9685/pca9685)
target_link_libraries(pca9685 platform)
endif( ${PLATFORM_HAS_PCA9685} )


