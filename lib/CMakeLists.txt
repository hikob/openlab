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

# Create the printf library
add_library(printf STATIC printf/printf)

# Create the event library
add_library(event STATIC event/event)
target_link_libraries(event freertos)

# Create the software timer library
add_library(softtimer STATIC softtimer/soft_timer_core softtimer/soft_timer_delay)

# Create the random library
add_library(random STATIC random/random)

# Create the USB library
if (${PLATFORM_HAS_USB})
  set (LIBUSB_SRC usb/usb_core usb/cdc_acm usb/hid usb/msc usb/scsi )
  if (${PLATFORM_HAS_SD})
    set(LIBUSB_SRC ${LIBUSB_SRC} usb/scsi_sd)
  endif (${PLATFORM_HAS_SD})
  add_library(usb STATIC  ${LIBUSB_SRC})
endif (${PLATFORM_HAS_USB})

