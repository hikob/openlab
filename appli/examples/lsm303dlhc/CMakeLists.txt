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

if(${PLATFORM_HAS_LSM303DLHC})
	add_executable(example_lsm303dlhc_polling example_lsm303dlhc_polling)
	target_link_libraries(example_lsm303dlhc_polling platform lsm303dlhc printf)
	
    add_executable(example_lsm303dlhc_mag_interrupt example_lsm303dlhc_mag_interrupt)
	target_link_libraries(example_lsm303dlhc_mag_interrupt platform lsm303dlhc printf)
	
    add_executable(example_lsm303dlhc_acc_interrupt example_lsm303dlhc_acc_interrupt)
	target_link_libraries(example_lsm303dlhc_acc_interrupt platform lsm303dlhc printf)
endif(${PLATFORM_HAS_LSM303DLHC})
