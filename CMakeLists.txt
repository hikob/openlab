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

project(HiKoB_Openlab)
cmake_minimum_required(VERSION 2.6)

# Check if platform is defined
if(NOT DEFINED PLATFORM)
	message(FATAL_ERROR "PLATFORM not defined, please specify the platform with the following argument to cmake: -DPLATFORM=<platform>")
endif(NOT DEFINED PLATFORM)

# Check if platform exits
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/platform/${PLATFORM}/include.cmake")
	message(FATAL_ERROR "PLATFORM ${PLATFORM} hasn't a valid include.cmake file")
endif(NOT EXISTS "${PROJECT_SOURCE_DIR}/platform/${PLATFORM}/include.cmake")

# Put all binaries in the same directory as well as libraries
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH    "${PROJECT_BINARY_DIR}/lib")

# Include the platform specific variables
include(${PROJECT_SOURCE_DIR}/platform/${PLATFORM}/include.cmake)

# Set optimization
if(NOT DEFINED OPT)
	set(OPT "s")
endif(NOT DEFINED OPT)

# Set RELEASE flag if variable set
if(DEFINED RELEASE AND RELEASE)
	set(MY_C_FLAGS "${MY_C_FLAGS} -DRELEASE=1")
else(DEFINED RELEASE AND RELEASE)
	set(MY_C_FLAGS "${MY_C_FLAGS} -DRELEASE=0")
endif(DEFINED RELEASE AND RELEASE)

# Set the DEBUG flag if variable set
if(DEFINED DEBUG)
	set(MY_C_FLAGS "${MY_C_FLAGS} -DENABLE_DEBUG=${DEBUG}")
endif(DEFINED DEBUG)

# Set KEEP_UART flag if variable set
if(KEEP_UART)
set(MY_C_FLAGS "${MY_C_FLAGS} -DKEEP_UART=1")
endif(KEEP_UART)

# Remove dynamic linking
SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)

# Set compiler and linker flags
set(MY_C_FLAGS "${MY_C_FLAGS} -O${OPT}")
set(MY_C_FLAGS "${MY_C_FLAGS} -Wall")
set(MY_C_FLAGS "${MY_C_FLAGS} -Wimplicit -Wcast-align -Wpointer-arith")
set(MY_C_FLAGS "${MY_C_FLAGS} -Wredundant-decls -Wshadow -Wcast-qual -Wcast-align")
set(MY_C_FLAGS "${MY_C_FLAGS} -DPLATFORM_TYPE=${PLATFORM_TYPE} -DPLATFORM_VERSION=${PLATFORM_VERSION}")

set(CMAKE_C_FLAGS          "${MY_C_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${MY_LD_FLAGS}")

# Add a .elf suffix to all executables
set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# Include the global header directories
include_directories(
	${PROJECT_SOURCE_DIR}/drivers
	${PROJECT_SOURCE_DIR}/lib
	${PROJECT_SOURCE_DIR}/net
	${PROJECT_SOURCE_DIR}/os
	${PROJECT_SOURCE_DIR}/os/freertos/Source/include
	${PROJECT_SOURCE_DIR}/periph
	${PROJECT_SOURCE_DIR}/platform
	${PROJECT_SOURCE_DIR}/platform/${PLATFORM})

# Build the periph folder
add_subdirectory(drivers)

# Build the lib folder
add_subdirectory(lib)

# Include the net folder
add_subdirectory(net)

# Include the os folder
add_subdirectory(os)

# Build the periph folder
add_subdirectory(periph)

# Build the platform folder
add_subdirectory(platform/${PLATFORM})

# Build the appli directory
add_subdirectory(appli)
