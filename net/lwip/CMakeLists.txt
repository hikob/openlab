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
# Copyright (C) 2011-2013 HiKoB.
#

include_directories(src/include)
include_directories(src/include/ipv4)
include_directories(port)

add_library(lwip STATIC
    src/api/api_lib
    src/api/api_msg
    src/api/err
    src/api/netbuf
    src/api/netdb
    src/api/netifapi
    src/api/sockets
    src/api/tcpip
    
    src/core/ipv4/autoip
    src/core/ipv4/icmp
    src/core/ipv4/igmp
    src/core/ipv4/inet_chksum
    src/core/ipv4/inet
    src/core/ipv4/ip_addr
    src/core/ipv4/ip_frag
    src/core/ipv4/ip
    
    src/core/snmp/asn1_dec
    src/core/snmp/asn1_enc
    src/core/snmp/mib_structs
    src/core/snmp/mib2
    src/core/snmp/msg_in
    src/core/snmp/msg_out
    
    src/core/def
    src/core/dhcp
    src/core/dns
    src/core/init
    src/core/mem
    src/core/memp
    src/core/netif
    src/core/pbuf
    src/core/raw
    src/core/stats
    src/core/sys
    src/core/tcp_in
    src/core/tcp_out
    src/core/tcp
    src/core/timers
    src/core/udp
    
    src/netif/etharp
    src/netif/ethernetif
    src/netif/slipif
    )

add_library(lwip_port STATIC
    port/arch/sys_arch
    port/ethernetif
    )

target_link_libraries(lwip lwip_port)
target_link_libraries(lwip_port lwip)

set(ignore_warning_flags "-Wno-cast-qual -Wno-cast-align -Wno-uninitialized -Wno-address")
set_property(TARGET lwip APPEND PROPERTY COMPILE_FLAGS "${ignore_warning_flags}")
