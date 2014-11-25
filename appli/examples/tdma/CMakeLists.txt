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

    add_executable(example_tdma_coord example_tdma_coord)
    target_link_libraries(example_tdma_coord mac_tdma platform)

    add_executable(example_tdma_node example_tdma_node)
    target_link_libraries(example_tdma_node mac_tdma platform)

    add_executable(example_tdma_coord_static example_tdma_coord_static)
    target_link_libraries(example_tdma_coord_static mac_tdma platform)

    add_executable(example_tdma_node_cb example_tdma_node_cb)
    target_link_libraries(example_tdma_node_cb mac_tdma platform)

endif (${PLATFORM_HAS_TDMA})
