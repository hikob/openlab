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

if(${PLATFORM_HAS_ETHERNET})
    include_directories(
        ${PROJECT_SOURCE_DIR}/net/lwip/src/include
        ${PROJECT_SOURCE_DIR}/net/lwip/src/include/ipv4
        ${PROJECT_SOURCE_DIR}/net/lwip/port
        )
	add_executable(test_lwip test_lwip)
	target_link_libraries(test_lwip platform lwip)
endif(${PLATFORM_HAS_ETHERNET})
