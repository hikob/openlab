set(DRIVERS   native)

set(MY_C_FLAGS "${MY_C_FLAGS} -DNATIVE")
set(PLATFORM_TYPE    0xaa)
set(PLATFORM_VERSION 0x01)

set(PLATFORM_RAM_KB 1000000)

# Set the flags to select the application that may be compiled

include(${PROJECT_SOURCE_DIR}/platform/include-ntv.cmake)
