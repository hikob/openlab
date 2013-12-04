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
# Copyright (C) 2011,2012 HiKoB.
#

if(${PLATFORM_HAS_INA226})
    add_executable(iotlab_controlnode 
        controlnode
        iotlab-serial
        iotlab-control
        iotlab-polling
        iotlab-radio)
    target_link_libraries(iotlab_controlnode
        packet
        platform)
endif(${PLATFORM_HAS_INA226})

include_directories(.)
add_executable(iotlab_serial_test
    test/iotlab-serial-test
    iotlab-serial)
target_link_libraries(iotlab_serial_test
    packet
    platform)