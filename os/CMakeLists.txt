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

# Define the library
add_library(freertos STATIC
	freertos/Source/croutine
	freertos/Source/list
	freertos/Source/queue
	freertos/Source/tasks
	freertos/Source/timers
	freertos/Source/portable/MemMang/${FREERTOS_MEMMANG}
	freertos/Source/portable/GCC/${FREERTOS_PORT}/port
)

# Link to the drivers
target_link_libraries(freertos platform)

set(ignore_warning_flags "-Wno-cast-qual -Wno-cast-align")
set_property(TARGET freertos APPEND PROPERTY COMPILE_FLAGS "${ignore_warning_flags}")
