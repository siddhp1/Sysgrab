set (SOURCES
	art.cpp
	config.cpp
	main.cpp
)

add_executable(sysgrab ${SOURCES})

target_compile_definitions(sysgrab PRIVATE SYSGRAB_VER="${PROJECT_VERSION}")

target_include_directories(sysgrab PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
										  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

add_custom_target(copy_art_file ALL
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/resources/art.txt"
		"$<TARGET_FILE_DIR:sysgrab>/art.txt"
	COMMENT "Copying art.txt to output directory"
	VERBATIM
)

add_custom_target(copy_config_file ALL
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_SOURCE_DIR}/resources/config.cfg"
		"$<TARGET_FILE_DIR:sysgrab>/config.cfg"
	COMMENT "Copying config.cfg to output directory"
	VERBATIM
)

add_dependencies(sysgrab copy_art_file copy_config_file)
