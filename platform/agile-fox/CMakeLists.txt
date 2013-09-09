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

# Include the STM32 drivers folder
include_directories(
	${PROJECT_SOURCE_DIR}/drivers/cortex-m3
	${PROJECT_SOURCE_DIR}/drivers/stm32
	${PROJECT_SOURCE_DIR}/drivers/stm32f1xx)

# Add the platform library
add_library(platform STATIC
	${PROJECT_SOURCE_DIR}/platform/platform
	agilefox 
	agilefox_leds
	agilefox_button
	agilefox_drivers
	agilefox_periph
	agilefox_lib
	agilefox_net
    )

add_library(platform_usb STATIC
	${PROJECT_SOURCE_DIR}/platform/platform
	agilefox 
	agilefox_leds
	agilefox_button
	agilefox_drivers
	agilefox_periph
	agilefox_lib
    agilefox_net
	agilefox_usb
	)

# Allow for some more cyclic deps in libraries
set_property(TARGET platform APPEND PROPERTY LINK_INTERFACE_MULTIPLICITY 3)

# Link the library to the drivers and peripherals
target_link_libraries(platform
# Driver
	drivers_stm32f1xx
	
# Periph
	rf2xx
	lsm303dlhc
	l3g4200d
	lps331ap
	
# Lib
	freertos
	random
	printf
	event
	softtimer
	
# Net
	phy_rf2xx
	
	)

target_link_libraries(platform_usb
# Driver
	drivers_stm32f1xx
	
# Periph
	rf2xx
	lsm303dlhc
	l3g4200d
	lps331ap
	
# Lib
	freertos
	random
	printf
	event
	softtimer
	usb
	
# Net
	phy_rf2xx
	
	)
