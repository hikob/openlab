set(OOCD_TARGET stm32f1x)

set(PLATFORM_OOCD_ITF ${PROJECT_SOURCE_DIR}/platform/scripts/fiteco-m3.cfg)

set(LINKSCRIPT ../scripts/stm32f103rey6.ld)

set(DRIVERS stm32f1xx)

set(PLATFORM_RAM_KB 64)

# Set the flags to select the application that may be compiled
set(PLATFORM_HAS_L3G4200D 1)
set(PLATFORM_HAS_LSM303DLHC 1)
set(PLATFORM_HAS_LPS331AP 1)
set(PLATFORM_HAS_ISL29020 1)
set(PLATFORM_HAS_N25XXX 1)
set(PLATFORM_HAS_RF231 1)

set(PLATFORM_HAS_PHY 1)
set(PLATFORM_HAS_SYSTICK 1)

include(${PROJECT_SOURCE_DIR}/platform/include-cm3.cmake)
