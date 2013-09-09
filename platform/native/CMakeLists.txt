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

# Include the STM32 drivers folder
include_directories(${PROJECT_SOURCE_DIR}/drivers/native)

# Add the platform library
add_library(platform STATIC
	native
	native_leds
	native_drivers
	native_periph
	native_lib)

# Link the library to the drivers and peripherals
target_link_libraries(platform drivers_native freertos random printf)

