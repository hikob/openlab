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

if(${PLATFORM_HAS_PHY})
	# Add the executables
	add_executable(phy_simpletx simpletx)
	target_link_libraries(phy_simpletx platform)

	add_executable(phy_simplerx simplerx)
	target_link_libraries(phy_simplerx platform)
	
	add_executable(phy_ed ed)
	target_link_libraries(phy_ed platform)
endif(${PLATFORM_HAS_PHY})
