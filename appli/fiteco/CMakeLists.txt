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
# Copyright (C) 2012 HiKoB.
#

# Add the fiteco_gwt test apps
if (${PLATFORM} STREQUAL "fiteco-gwt")
    
    add_executable(fiteco_gwt_test_power fiteco_gwt_test_power)
    target_link_libraries(fiteco_gwt_test_power platform)
    
endif (${PLATFORM} STREQUAL "fiteco-gwt")