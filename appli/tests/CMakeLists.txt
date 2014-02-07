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
# Copyright (C) 2011,2012 HiKoB.
#

# Add the drivers directory
add_subdirectory(drivers)

# Add the printf directory
add_subdirectory(printf)

# Add the printf directory
add_subdirectory(scanf)

# Add the rtos directory
add_subdirectory(os)

# Add the l3g4200d directory
add_subdirectory(l3g4200d)

# Add the lsm303dlhc directory
add_subdirectory(lsm303dlhc)

# Add the rf2xx directory
add_subdirectory(rf2xx)

# Add the phy directory
add_subdirectory(phy)

# Add the softtim directory
add_subdirectory(softtim)

# Add the usb directory
add_subdirectory(usb)

# Add the lps331 directory
add_subdirectory(lps331)

# Add the batfb directory
add_subdirectory(batfb)

# Add the PCA9685 directory
add_subdirectory(pca9685)

# Add the pga308 directory
add_subdirectory(pga308)

# Add the n25xxx directory
add_subdirectory(n25xxx)

# Add the ina226 directory
add_subdirectory(ina226)

# Add the fat32 directory
add_subdirectory(fat32)

# Add the lowpower directory
add_subdirectory(lowpower)

# Add the ublox_max6 directory
add_subdirectory(ublox_max6)

# Add the imu
add_subdirectory(imu)

# Add the lwip tests
add_subdirectory(lwip)

# Add the i2c_slave directory
add_subdirectory(i2c_slave)
