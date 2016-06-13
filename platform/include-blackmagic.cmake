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
# Copyright (C) 2014 HiKoB.
#

if   (NOT DEFINED BLACKMAGIC_TTY)
    set(BLACKMAGIC_TTY /dev/ttyACM0)
endif(NOT DEFINED BLACKMAGIC_TTY)
if   (NOT DEFINED BLACKMAGIC_TGT)
    set(BLACKMAGIC_TGT 1)
endif(NOT DEFINED BLACKMAGIC_TGT)

set(GDB_ARGS -ex 'target ext ${BLACKMAGIC_TTY}')

if (DEFINED BLACKMAGIC_VSUPPLY)
    set(GDB_ARGS ${GDB_ARGS} -ex 'mon target_vsupply ${BLACKMAGIC_VSUPPLY}')
elseif (DEFINED BLACKMAGIC_VSUPPLY)
    set(GDB_ARGS ${GDB_ARGS} -ex 'mon target_vsupply')
endif (DEFINED BLACKMAGIC_VSUPPLY)

if (DEFINED BLACKMAGIC_VIO)
    set(GDB_ARGS ${GDB_ARGS} -ex 'mon target_vio ${BLACKMAGIC_VIO}')
elseif (DEFINED BLACKMAGIC_VIO)
    set(GDB_ARGS ${GDB_ARGS} -ex 'mon target_vio')
endif (DEFINED BLACKMAGIC_VIO)

if (DEFINED BLACKMAGIC_RESET_BEFORE_ATTACH)
    set(GDB_ARGS ${GDB_ARGS} -ex 'mon reset')
endif (DEFINED BLACKMAGIC_RESET_BEFORE_ATTACH)

set(GDB_ARGS ${GDB_ARGS} -ex 'mon swdp_scan' -ex 'att ${BLACKMAGIC_TGT}')

add_custom_target(debug arm-none-eabi-gdb ${GDB_ARGS})
add_custom_target(debug_warm arm-none-eabi-gdb ${GDB_ARGS})
add_custom_target(reset gdb -batch ${GDB_ARGS} -ex 'mon reset')

macro(add_executable)
    add_custom_target(flash_${ARGV0} gdb -batch ${GDB_ARGS} 
        -ex load ${EXECUTABLE_OUTPUT_PATH}/${ARGV0}.elf 
        -ex 'printf \"Verifying firmware\\n\"' 
        -ex 'set remotetimeout 10' 
        -ex compare-sections
        -ex 'mon reset'

         DEPENDS ${ARGV0}
    )

    add_custom_target(hex_${ARGV0} ${CMAKE_OBJCOPY} -Oihex ${EXECUTABLE_OUTPUT_PATH}/${ARGV0}.elf ${EXECUTABLE_OUTPUT_PATH}/${ARGV0}.hex DEPENDS ${ARGV0})

    add_custom_target(dump_${ARGV0} ${CMAKE_OBJDUMP} -S -d ${EXECUTABLE_OUTPUT_PATH}/${ARGV0}.elf DEPENDS ${ARGV0})

    _add_executable(${ARGN})
endmacro(add_executable)
