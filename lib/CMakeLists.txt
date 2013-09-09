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
add_library(printf STATIC printf/printf printf/prints printf/printf_float)

# Create the scanf library
add_library(scanf STATIC scanf/scanf)

# Create the event library
add_library(event STATIC event/event)
add_library(event_priorities STATIC event/event_priorities)
target_link_libraries(event event_priorities freertos)

# Create the software timer library
add_library(softtimer STATIC softtimer/soft_timer_core softtimer/soft_timer_delay)

# Create the random library
add_library(random STATIC random/random)

# Create the USB library
if (${PLATFORM_HAS_USB})
  set (LIBUSB_SRC usb/usb_core usb/cdc_acm usb/hid usb/hid_kbd usb/msc usb/scsi )
  set (LIBUSB_SRC ${LIBUSB_SRC} usb/dfu usb/scsi_mmapfs usb/storage )
  if (${PLATFORM_HAS_SD})
    set(LIBUSB_SRC ${LIBUSB_SRC} usb/scsi_sd)
  endif (${PLATFORM_HAS_SD})
  add_library(usb STATIC  ${LIBUSB_SRC})
endif (${PLATFORM_HAS_USB})

# Include the fiteco library
add_subdirectory(fiteco)

# Create the fat32 library
if(${PLATFORM_HAS_SD})
add_library(fat32 STATIC fat32/buf_util fat32/fat32 fat32/file fat32/fs)
endif(${PLATFORM_HAS_SD})

add_library(packet STATIC packet/packet packet/packet_storage)
target_link_libraries(packet freertos)
