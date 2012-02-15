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
target_link_libraries(rf2xx delay platform)

# Add the LSM303DLHC library
add_library(lsm303dlhc STATIC lsm303dlhc/lsm303dlhc)
target_link_libraries(lsm303dlhc platform)

# Add the L3G4200D library
add_library(l3g4200d STATIC l3g4200d/l3g4200d)
target_link_libraries(l3g4200d platform)


