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

# Include the STM32L drivers folder
include_directories(
	${PROJECT_SOURCE_DIR}/drivers/cortex-m3
	${PROJECT_SOURCE_DIR}/drivers/stm32
	${PROJECT_SOURCE_DIR}/drivers/stm32l1xx)

# Add the platform library
add_library(platform STATIC
	${PROJECT_SOURCE_DIR}/platform/platform
	azurelion
	azurelion_leds
	azurelion_drivers
	azurelion_periph
	azurelion_lib
	azurelion_net
	azurelion_io
)

add_library(platform_usb STATIC
	${PROJECT_SOURCE_DIR}/platform/platform
	azurelion
	azurelion_leds
	azurelion_drivers
	azurelion_periph
	azurelion_lib
	azurelion_net
	azurelion_io
	azurelion_usb
)

# Allow for some more cyclic deps in libraries
set_property(TARGET platform APPEND PROPERTY LINK_INTERFACE_MULTIPLICITY 3)

# Link the library to the drivers and peripherals
target_link_libraries(platform
# Driver
	drivers_stm32l1xx

# Periph
	rf2xx
	batfb
	pga308
	adg759
	
# Lib
	freertos
	random
	printf
	packet
	event
	softtimer

# Net
	phy_rf2xx)

target_link_libraries(platform_usb
# Driver
	drivers_stm32l1xx

# Periph
	rf2xx
	batfb
	pga308
	adg759
	
# Lib
	freertos
	random
	printf
	packet
	event
	softtimer
	usb

# Net
	phy_rf2xx)
	
