set(CMAKE_C_COMPILER   "gcc")
set(CMAKE_CXX_COMPILER "g++")

# FreeRTOS port
set(FREERTOS_PORT "Native")
set(FREERTOS_MEMMANG heap_3)

# GCC target specific flags
set(MY_C_FLAGS   "${MY_C_FLAGS} -DGCC_NATIVE")

# LD target specific flags
set(MY_LD_FLAGS  "${MY_LD_FLAGS} ")


