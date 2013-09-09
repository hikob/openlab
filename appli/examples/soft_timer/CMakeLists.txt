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

add_executable(example_soft_timer_delay example_soft_timer_delay)
target_link_libraries(example_soft_timer_delay platform)

add_executable(example_soft_timer_alarm example_soft_timer_alarm)
target_link_libraries(example_soft_timer_alarm platform)

add_executable(example_soft_timer_priority example_soft_timer_priority)
target_link_libraries(example_soft_timer_priority platform)

