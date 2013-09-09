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
# Copyright (C) 2011-2013 HiKoB.
#

if(${PLATFORM_HAS_PHY})

# Add the executables
add_executable(rf_cwave cwave)
target_link_libraries(rf_cwave platform)

add_executable(rf_s11 s11)
target_link_libraries(rf_s11 platform)

# Add the executables for RSSI testing
add_executable(rf_rssi_tx rssi_tx)
add_executable(rf_rssi_rx rssi_rx)
target_link_libraries(rf_rssi_tx platform)
target_link_libraries(rf_rssi_rx platform)

endif(${PLATFORM_HAS_PHY})
