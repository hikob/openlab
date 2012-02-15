if(${PLATFORM_HAS_L3G4200D})
	add_executable(test_l3g4200d l3g4200d)
	target_link_libraries(test_l3g4200d platform l3g4200d printf)
endif(${PLATFORM_HAS_L3G4200D})
