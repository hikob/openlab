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

if("${DRIVERS}" STREQUAL "stm32f1xx")
	include_directories(cortex-m3)
	include_directories(stm32)
	include_directories(stm32f1xx)

	add_library(drivers_stm32f1xx STATIC
		cortex-m3/boot
		cortex-m3/nvic
		stm32/spi
		stm32/i2c
		stm32/uart
		stm32/timer
		stm32/dma
		stm32/unique_id
		stm32/usb
		stm32/crc
		stm32/watchdog
		stm32f1xx/boot
		stm32f1xx/rcc
		stm32f1xx/rcc_clock
		stm32f1xx/rcc_sysclk
		stm32f1xx/afio
		stm32f1xx/exti
		stm32f1xx/gpio
		stm32f1xx/adc
		stm32f1xx/flash
		stm32f1xx/sdio
	)
endif("${DRIVERS}" STREQUAL "stm32f1xx")
