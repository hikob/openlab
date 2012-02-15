if(${PLATFORM_HAS_LSM303DLHC})
	add_executable(test_lsm303dlhc lsm303dlhc)
	target_link_libraries(test_lsm303dlhc platform lsm303dlhc printf)
endif(${PLATFORM_HAS_LSM303DLHC})
