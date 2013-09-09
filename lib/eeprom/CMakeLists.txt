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

add_library(eeprom_fake STATIC eeprom_fake)

add_library(eeprom STATIC eeprom)
target_link_libraries(eeprom eeprom_fake)

if( (${PLATFORM_HAS_EEPROM}) )
    add_library(eeprom_internal STATIC eeprom_internal)
    target_link_libraries(eeprom_internal eeprom)
endif( (${PLATFORM_HAS_EEPROM}) )

add_library(eeprom_m24lrxx STATIC eeprom_m24lrxx)
target_link_libraries(eeprom_m24lrxx eeprom)
