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


if ("${DRIVERS}" STREQUAL "stm32l1xx")

include_directories(
	${PROJECT_SOURCE_DIR}/drivers/cortex-m3
	${PROJECT_SOURCE_DIR}/drivers/stm32
	${PROJECT_SOURCE_DIR}/drivers/stm32l1xx)

# Add the executables

add_executable(lp_run lp_run)
target_link_libraries(lp_run platform)

add_executable(lp_lprun lp_lprun)
target_link_libraries(lp_lprun platform)

add_executable(lp_lpsleep lp_lpsleep)
target_link_libraries(lp_lpsleep platform)

add_executable(lp_sleep lp_sleep)
target_link_libraries(lp_sleep platform)

add_executable(lp_stop lp_stop)
target_link_libraries(lp_stop platform)

endif ("${DRIVERS}" STREQUAL "stm32l1xx")
