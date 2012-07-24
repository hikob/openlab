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

if( ${PLATFORM_HAS_PCA9685} )
	add_executable(pca9685_chaser pca9685_chaser)
	target_link_libraries(pca9685_chaser platform)
	
	add_executable(pca9685_colors pca9685_colors)
	target_link_libraries(pca9685_colors platform)
endif( ${PLATFORM_HAS_PCA9685} )
