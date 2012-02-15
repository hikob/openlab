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

if(${PLATFORM_HAS_RF231})
	add_executable(rf231_registers rf231_registers)
	target_link_libraries(rf231_registers platform rf2xx printf)
	
	add_executable(rf231_spi rf231_spi)
	target_link_libraries(rf231_spi platform rf2xx printf)
	
	add_executable(rf231_timings rf231_timings)
	target_link_libraries(rf231_timings platform rf2xx printf)
	
	
endif(${PLATFORM_HAS_RF231})

if(${PLATFORM_HAS_RF212})
	add_executable(rf212_registers rf212_registers)
	target_link_libraries(rf212_registers platform rf2xx printf)
endif(${PLATFORM_HAS_RF212})
