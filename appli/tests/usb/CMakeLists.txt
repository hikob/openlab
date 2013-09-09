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
# Copyright (C) 2012 HiKoB.
#

if(${PLATFORM_HAS_USB})
  
  add_executable(test_usb_cdc_acm usb_cdc_acm)
  target_link_libraries(test_usb_cdc_acm usb platform_usb)
	
  add_executable(test_usb_hid usb_hid)
  target_link_libraries(test_usb_hid usb platform_usb)

  add_executable(test_usb_hid_kbd usb_hid_kbd)
  target_link_libraries(test_usb_hid_kbd usb platform_usb)

  add_executable(test_usb_msc usb_msc)
  target_link_libraries(test_usb_msc usb platform_usb)

  add_executable(test_usb_led usb_led)
  target_link_libraries(test_usb_led usb platform_usb)
  
endif(${PLATFORM_HAS_USB})
