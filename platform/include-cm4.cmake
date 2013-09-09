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

set(CMAKE_C_COMPILER   "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
set(CMAKE_OBJDUMP      "arm-none-eabi-objdump")
set(CMAKE_AR           "arm-none-eabi-ar")
set(CMAKE_RANLIB       "arm-none-eabi-ranlib")

# FreeRTOS port
set(FREERTOS_PORT "ARM_CM4F")
set(FREERTOS_MEMMANG heap_2)

# GCC target specific flags
set(MY_C_FLAGS   "${MY_C_FLAGS} -DGCC_ARMCM4F")

# If RELEASE is not set, set the debug option to gcc
if(RELEASE EQUAL 0)
	set(MY_C_FLAGS "${MY_C_FLAGS} -gdwarf-2")
endif(RELEASE EQUAL 0)

set(MY_C_FLAGS   "${MY_C_FLAGS} -fno-builtin -fsingle-precision-constant")
set(MY_C_FLAGS   "${MY_C_FLAGS} -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb -mthumb-interwork")
set(MY_C_FLAGS   "${MY_C_FLAGS} -ffunction-sections -fdata-sections -Wl,--gc-sections")

# LD target specific flags
set(MY_LD_FLAGS  "${MY_LD_FLAGS} -nostartfiles -nodefaultlibs")
set(MY_LD_FLAGS  "${MY_LD_FLAGS} -T${PROJECT_SOURCE_DIR}/platform/${PLATFORM}/${LINKSCRIPT}")
set(MY_LD_FLAGS  "${MY_LD_FLAGS} -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb -mthumb-interwork")
set(MY_LD_FLAGS  "${MY_LD_FLAGS} -ffunction-sections -fdata-sections -Wl,--gc-sections")

# Set OOCD target
if(NOT OOCD_ITF)
	set(OOCD_ITF ${PLATFORM_OOCD_ITF})
endif(NOT OOCD_ITF)

# Compute random Ports
string(RANDOM LENGTH 3 ALPHABET "0123456789" PORT)
set(GDB_PORT "3${PORT}")
set(TELNET_PORT "4${PORT}")
set(TCL_PORT "6${PORT}")

# Redefine the "add_executable" macro so that each time a
# new executable is added, a rule to flash it is defined
macro(add_executable)
add_custom_target(flash_${ARGV0} openocd -f "${OOCD_ITF}"
	-f "target/${OOCD_TARGET}.cfg"
	-c "gdb_port ${GDB_PORT}"
	-c "telnet_port ${TELNET_PORT}"
	-c "tcl_port ${TCL_PORT}"
	-c "init" 
	-c "targets"
	-c "reset halt"
	-c "reset init"
	-c "flash write_image erase ${EXECUTABLE_OUTPUT_PATH}/${ARGV0}.elf" 
	-c "verify_image ${EXECUTABLE_OUTPUT_PATH}/${ARGV0}.elf" 
	-c "reset run"
	-c "shutdown" DEPENDS ${ARGV0})
add_custom_target(dump_${ARGV0} ${CMAKE_OBJDUMP} -S -d 
	${EXECUTABLE_OUTPUT_PATH}/${ARGV0}.elf DEPENDS ${ARGV0})
add_custom_target(mmap_${ARGV0} 
        ${PROJECT_SOURCE_DIR}/tools/memmap.py -j 
	-t "MemMap for ${ARGV0}"
	-m "${EXECUTABLE_OUTPUT_PATH}/out.map"
	-o "${EXECUTABLE_OUTPUT_PATH}/memmap.html"
        DEPENDS ${ARGV0})
_add_executable(${ARGN})
endmacro(add_executable)

add_custom_target(debug openocd -f "${OOCD_ITF}" 
	-f "target/${OOCD_TARGET}.cfg" 
	-c "tcl_port ${TCL_PORT}"
	-c "telnet_port ${TELNET_PORT}"
	-c "init" 
	-c "targets"
	-c "reset halt" )

add_custom_target(reset openocd -f "${OOCD_ITF}" 
	-f "target/${OOCD_TARGET}.cfg" 
	-c "gdb_port ${GDB_PORT}"
	-c "tcl_port ${TCL_PORT}"
	-c "telnet_port ${TELNET_PORT}"
	-c "init" 
	-c "targets"
	-c "reset run"
	-c "shutdown")

