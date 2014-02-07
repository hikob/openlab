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

if (${PLATFORM_HAS_TDMA})

    # Create the mac_tdma library
    add_library(mac_tdma STATIC
        tdma
        frame
        data
        slot
        coord
        node
        )
    target_link_libraries(mac_tdma packet random softtimer event platform)

endif (${PLATFORM_HAS_TDMA})
