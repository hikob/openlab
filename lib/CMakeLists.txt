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

# Add the include directories
include_directories(delay printf event packet random)

# Create the printf library
add_library(printf STATIC printf/printf)

# Create the delay library
add_library(delay STATIC delay/delay)

# Create the random library
add_library(random STATIC random/random)

# Create the event library
add_library(event STATIC event/event)
target_link_libraries(event freertos)

# Create the packet library
#add_library(packet STATIC packet/packet)
#target_link_libraries(packet freertos)
